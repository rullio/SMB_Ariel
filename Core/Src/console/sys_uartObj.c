/**
 ******************************************************************************
 * @file           : plib_uart6.c
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

// *****************************************************************************
// *****************************************************************************
// Section: UART6 Implementation
// *****************************************************************************
// *****************************************************************************

UART_RING_BUFFER_OBJECT uartObj;

#define UART_READ_BUFFER_SIZE      64		// 이것을 16 으로 두면 extAddr 을 copy&paste 로 넣을 때 죽을 수 있다..
static uint8_t UART_ReadBuffer[UART_READ_BUFFER_SIZE];

bool uartObj_Init( void )
{
	/* Initialize instance object */
	uartObj.rdCallback = NULL;
	uartObj.rdInIndex = 0;
	uartObj.rdOutIndex = 0;
	uartObj.isRdNotificationEnabled = false;
	uartObj.isRdNotifyPersistently = false;
	uartObj.rdThreshold = 0;

	uartObj.wrCallback = NULL;
	uartObj.wrInIndex = 0;
	uartObj.wrOutIndex = 0;
	uartObj.isWrNotificationEnabled = false;
	uartObj.isWrNotifyPersistently = false;
	uartObj.wrThreshold = 0;

	uartObj.errors = UART_ERROR_NONE;
	uartObj.rdBufferSize = UART_READ_BUFFER_SIZE;
	//uart6Obj.wrBufferSize = UART6_WRITE_BUFFER_SIZE;
	return true;
}


/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static inline bool UART_RxPushByte(uint8_t rdByte)
{
	uint32_t tempInIndex;
	bool isSuccess = false;

	tempInIndex = uartObj.rdInIndex + 1;

	if (tempInIndex >= uartObj.rdBufferSize)
	{
		tempInIndex = 0;
	}

	if (tempInIndex == uartObj.rdOutIndex)
	{
		/* Queue is full - Report it to the application. Application gets a chance to free up space by reading data out from the RX ring buffer */
		if(uartObj.rdCallback != NULL)
		{
			uartObj.rdCallback(UART_EVENT_READ_BUFFER_FULL, uartObj.rdContext);

			/* Read the indices again in case application has freed up space in RX ring buffer */
			tempInIndex = uartObj.rdInIndex + 1;

			if (tempInIndex >= uartObj.rdBufferSize)
			{
				tempInIndex = 0;
			}
		}
	}

	/* Attempt to push the data into the ring buffer */
	if (tempInIndex != uartObj.rdOutIndex)
	{

		UART_ReadBuffer[uartObj.rdInIndex] = (uint8_t)rdByte;
		uartObj.rdInIndex = tempInIndex;

		isSuccess = true;
	}
	else
	{
		/* Queue is full. Data will be lost. */
	}

	return isSuccess;
}


bool RxPushByte (uint8_t rdByte)
{
	assert (UART_RxPushByte( rdByte ) == true);
    return true;
}

size_t UART_Read(uint8_t* pRdBuffer, const size_t size)
{
	size_t nBytesRead = 0;
	uint32_t rdOutIndex = 0;
	uint32_t rdInIndex = 0;

	/* Take a snapshot of indices to avoid creation of critical section */
	rdOutIndex = uartObj.rdOutIndex;
	rdInIndex = uartObj.rdInIndex;

	while (nBytesRead < size)
	{
		if (rdOutIndex != rdInIndex)
		{

			pRdBuffer[nBytesRead++] = UART_ReadBuffer[rdOutIndex++];
			if (rdOutIndex >= uartObj.rdBufferSize)
			{
				rdOutIndex = 0;
			}
		}
		else
		{
			/* No more data available in the RX buffer */
			break;
		}
	}

	uartObj.rdOutIndex = rdOutIndex;

	return nBytesRead;
}

size_t UART_ReadCountGet(void)
{
	size_t nUnreadBytesAvailable;
	uint32_t rdInIndex;
	uint32_t rdOutIndex;

	/* Take a snapshot of indices to avoid processing in critical section */
	rdInIndex = uartObj.rdInIndex;
	rdOutIndex = uartObj.rdOutIndex;

	if ( rdInIndex >=  rdOutIndex)
	{
		nUnreadBytesAvailable =  rdInIndex -  rdOutIndex;
	}
	else
	{
		nUnreadBytesAvailable =  (uartObj.rdBufferSize -  rdOutIndex) + rdInIndex;
	}

	return nUnreadBytesAvailable;
}

size_t UART_ReadFreeBufferCountGet(void)
{
	return (uartObj.rdBufferSize - 1) - UART_ReadCountGet();
}


size_t UART_WriteCountGet(void)
{
	size_t nPendingTxBytes = 0;

	//nPendingTxBytes = UART6_WritePendingBytesGet();

	return nPendingTxBytes;
}

size_t UART_Write(uint8_t* pWrBuffer, const size_t size )
{
	size_t nBytesWritten  = 0;

	nBytesWritten = DbgTraceWrite (1, pWrBuffer, size);
	return nBytesWritten;
}

size_t UART_WriteFreeBufferCountGet(void)
{
	return (uartObj.wrBufferSize - 1) - UART_WriteCountGet();
}
