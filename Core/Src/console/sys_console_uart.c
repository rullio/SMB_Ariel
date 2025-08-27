/**
 ******************************************************************************
 * @file           : sys_console_uart.c
 * @brief          : Copied from other STM project
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 Infinity Energy.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include "main.h"

const SYS_CONSOLE_DEV_DESC sysConsoleUARTDevDesc =
{
    .consoleDevice              = SYS_CONSOLE_DEV_USART,
    .init                       = Console_UART_Initialize,
    .read                       = Console_UART_Read,
    .readFreeBufferCountGet     = Console_UART_ReadFreeBufferCountGet,
    .readCountGet               = Console_UART_ReadCountGet,
    .write                      = Console_UART_Write,
    .writeFreeBufferCountGet    = Console_UART_WriteFreeBufferCountGet,
    .writeCountGet              = Console_UART_WriteCountGet,
    .status                     = Console_UART_Status,
    .flush                      = Console_UART_Flush,
};

static CONSOLE_UART_DATA gConsoleUartData[SYS_CONSOLE_UART_MAX_INSTANCES];
#define CONSOLE_UART_GET_INSTANCE(index)    (index >= SYS_CONSOLE_UART_MAX_INSTANCES)? NULL : &gConsoleUartData[index]

static bool Console_UART_ResourceLock(CONSOLE_UART_DATA* pConsoleUartData)
{
    if(osMutexAcquire(pConsoleUartData->mutexTransferObjects, osWaitForever) != osOK)
    {
        return false;
    }
    else
    {
        return true;
    }
}

static void Console_UART_ResourceUnlock(CONSOLE_UART_DATA* pConsoleUartData)
{
    /* Release mutex */
	osMutexRelease (pConsoleUartData->mutexTransferObjects);
}

void Console_UART_Initialize(uint32_t index, const void* initData)
{
    CONSOLE_UART_DATA* pConsoleUartData = CONSOLE_UART_GET_INSTANCE(index);
    const SYS_CONSOLE_UART_INIT_DATA* consoleUsartInitData = (const SYS_CONSOLE_UART_INIT_DATA*)initData;

    if (pConsoleUartData == NULL)
    {
        return;
    }

    pConsoleUartData->mutexTransferObjects = osMutexNew (NULL);
    if (pConsoleUartData->mutexTransferObjects == NULL)
    {
        return;
    }

    /* Assign the USART PLIB instance APIs to use */
    pConsoleUartData->uartPLIB = consoleUsartInitData->uartPLIB;

    pConsoleUartData->status = SYS_CONSOLE_STATUS_CONFIGURED;
}

/* Read out the data from the RX Ring Buffer */
size_t Console_UART_Read(uint32_t index, void* pRdBuffer, size_t count)
{
    size_t nBytesRead = 0;
    CONSOLE_UART_DATA* pConsoleUartData = CONSOLE_UART_GET_INSTANCE(index);

    if (pConsoleUartData == NULL)
    {
        return -1;
    }

    if (Console_UART_ResourceLock(pConsoleUartData) == false)
    {
        return -1;
    }

    nBytesRead = pConsoleUartData->uartPLIB->read(pRdBuffer, count);

    Console_UART_ResourceUnlock(pConsoleUartData);

    return nBytesRead;
}

/* Return number of unread bytes available in the RX Ring Buffer */
size_t Console_UART_ReadCountGet(uint32_t index)
{
    size_t nUnreadBytesAvailable = 0;

    CONSOLE_UART_DATA* pConsoleUartData = CONSOLE_UART_GET_INSTANCE(index);

    if (pConsoleUartData == NULL)
    {
        return -1;
    }

    if (Console_UART_ResourceLock(pConsoleUartData) == false)
    {
        return -1;
    }

    nUnreadBytesAvailable = pConsoleUartData->uartPLIB->readCountGet();

    Console_UART_ResourceUnlock(pConsoleUartData);

    return nUnreadBytesAvailable;
}

/* Return free space available in the RX Ring Buffer */
size_t Console_UART_ReadFreeBufferCountGet(uint32_t index)
{
    size_t nFreeBufferAvailable = 0;

    CONSOLE_UART_DATA* pConsoleUartData = CONSOLE_UART_GET_INSTANCE(index);

    if (pConsoleUartData == NULL)
    {
        return -1;
    }

    if (Console_UART_ResourceLock(pConsoleUartData) == false)
    {
        return -1;
    }

    nFreeBufferAvailable = pConsoleUartData->uartPLIB->readFreeBufferCountGet();

    Console_UART_ResourceUnlock(pConsoleUartData);

    return nFreeBufferAvailable;
}

size_t Console_UART_Write(uint32_t index, const void* pWrBuffer, size_t count )
{
    size_t nBytesWritten = 0;

    CONSOLE_UART_DATA* pConsoleUartData = CONSOLE_UART_GET_INSTANCE(index);

    if (pConsoleUartData == NULL)
    {
        return -1;
    }

    if (Console_UART_ResourceLock(pConsoleUartData) == false)
    {
        return -1;
    }

    nBytesWritten = pConsoleUartData->uartPLIB->write((uint8_t*)pWrBuffer, count);

    Console_UART_ResourceUnlock(pConsoleUartData);

    return nBytesWritten;
}

size_t Console_UART_WriteFreeBufferCountGet(uint32_t index)
{
    size_t nFreeBufferAvailable = 0;

    CONSOLE_UART_DATA* pConsoleUartData = CONSOLE_UART_GET_INSTANCE(index);

    if (pConsoleUartData == NULL)
    {
        return -1;
    }

    if (Console_UART_ResourceLock(pConsoleUartData) == false)
    {
        return -1;
    }

    nFreeBufferAvailable = pConsoleUartData->uartPLIB->writeFreeBufferCountGet();

    Console_UART_ResourceUnlock(pConsoleUartData);

    return nFreeBufferAvailable;
}

size_t Console_UART_WriteCountGet(uint32_t index)
{
    size_t nPendingTxBytes = 0;

    CONSOLE_UART_DATA* pConsoleUartData = CONSOLE_UART_GET_INSTANCE(index);

    if (pConsoleUartData == NULL)
    {
        return -1;
    }

    if (Console_UART_ResourceLock(pConsoleUartData) == false)
    {
        return -1;
    }

    nPendingTxBytes = pConsoleUartData->uartPLIB->writeCountGet();

    Console_UART_ResourceUnlock(pConsoleUartData);

    return nPendingTxBytes;
}

bool Console_UART_Flush(uint32_t index)
{
    /* Data is not buffered, nothing to flush */
    return true;
}

SYS_CONSOLE_STATUS Console_UART_Status(uint32_t index)
{
    CONSOLE_UART_DATA* pConsoleUartData = CONSOLE_UART_GET_INSTANCE(index);

    if (pConsoleUartData == NULL)
    {
        return SYS_CONSOLE_STATUS_ERROR;
    }
    else
    {
        return pConsoleUartData->status;
    }
}
