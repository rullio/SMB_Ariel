/**
 ******************************************************************************
 * @file           : wjb_func_cmd_parser.c
 * @brief          :
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

const SYS_CMD_INIT sysCmdInit =
{
		.moduleInit = {0},
		.consoleCmdIOParam = SYS_CMD_SINGLE_CHARACTER_READ_CONSOLE_IO_PARAM,
		.consoleIndex = 0,
};

const SYS_CONSOLE_UART_PLIB_INTERFACE sysConsole0UARTPlibAPI =
{
		.read = (SYS_CONSOLE_UART_PLIB_READ)UART_Read,
		.readCountGet = (SYS_CONSOLE_UART_PLIB_READ_COUNT_GET)UART_ReadCountGet,
		.readFreeBufferCountGet = (SYS_CONSOLE_UART_PLIB_READ_FREE_BUFFFER_COUNT_GET)UART_ReadFreeBufferCountGet,
		.write = (SYS_CONSOLE_UART_PLIB_WRITE)UART_Write,
		.writeCountGet = (SYS_CONSOLE_UART_PLIB_WRITE_COUNT_GET)UART_WriteCountGet,
		.writeFreeBufferCountGet = (SYS_CONSOLE_UART_PLIB_WRITE_FREE_BUFFER_COUNT_GET)UART_WriteFreeBufferCountGet,
};

const SYS_CONSOLE_UART_INIT_DATA sysConsole0UARTInitData =
{
		.uartPLIB = &sysConsole0UARTPlibAPI,
};

extern const SYS_CONSOLE_DEV_DESC sysConsoleUARTDevDesc;

const SYS_CONSOLE_INIT sysConsole0Init =
{
		.deviceInitData = (const void*)&sysConsole0UARTInitData,
		.consDevDesc = &sysConsoleUARTDevDesc,
		.deviceIndex = 0,
};


SYS_CMD_DEVICE_LIST cmdIODevList = {0, 0};
extern const SYS_CMD_API sysConsoleApi;
SYS_CMD_INIT _cmdInitData;       // data the command processor has been initialized with
static char printBuff[SYS_CMD_PRINT_BUFFER_SIZE];
static int printBuffPtr = 0;

static SYS_CMD_DESCRIPTOR_TABLE   _usrCmdTbl[MAX_CMD_GROUP] = { {0} };    // current command table

// function processing the VT100 escape sequence
typedef void (*_keySeqProcess)(SYS_CMD_IO_DCPT* pCmdIO, const struct _KEY_SEQ_DCPT* pSeqDcpt);

typedef struct _KEY_SEQ_DCPT
{
	const char*     keyCode;    // pointer to the key code sequence
	_keySeqProcess  keyFnc;     // key processing functions
	int             keySize;    // # of characters in the sequence
}KEY_SEQ_DCPT;


static void _keyUpProcess(SYS_CMD_IO_DCPT* pCmdIO, const KEY_SEQ_DCPT* pSeqDcpt);
static void _keyDownProcess(SYS_CMD_IO_DCPT* pCmdIO, const KEY_SEQ_DCPT* pSeqDcpt);
static void _keyRightProcess(SYS_CMD_IO_DCPT* pCmdIO, const KEY_SEQ_DCPT* pSeqDcpt);
static void _keyLeftProcess(SYS_CMD_IO_DCPT* pCmdIO, const KEY_SEQ_DCPT* pSeqDcpt);
static void _keyHomeProcess(SYS_CMD_IO_DCPT* pCmdIO, const KEY_SEQ_DCPT* pSeqDcpt);
static void _keyEndProcess(SYS_CMD_IO_DCPT* pCmdIO, const KEY_SEQ_DCPT* pSeqDcpt);

// dummy table holding the escape sequences + expected sequence size
// detection of a sequence is done using only the first 3 characters
#define         VT100_DETECT_SEQ_SIZE    3
static const KEY_SEQ_DCPT keySeqTbl[] =
{
		// keyCode      keyFnc              keySize
		{"\x1b[A",      _keyUpProcess,      sizeof("\x1b[A") - 1},
		{"\x1b[B",      _keyDownProcess,    sizeof("\x1b[B") - 1},
		{"\x1b[C",      _keyRightProcess,   sizeof("\x1b[C") - 1},
		{"\x1b[D",      _keyLeftProcess,    sizeof("\x1b[D") - 1},
		{"\x1b[1~",     _keyHomeProcess,    sizeof("\x1b[1~") - 1},
		{"\x1b[4~",     _keyEndProcess,     sizeof("\x1b[4~") - 1},
};

// prototypes

//static void     CommandQuit(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);              // command quit
static void     CommandReset (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);              // help
static void     CommandHelp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);              // help

static int      StringToArgs(char *str, char *argv[], size_t argvSize);
static void     ParseCmdBuffer(SYS_CMD_IO_DCPT* pCmdIO);      // parse the command buffer

static void     DisplayNodeMsg(SYS_CMD_IO_DCPT* pCmdIO, histCmdNode* pNext);

static void     CmdAddHead(histCmdList* pL, histCmdNode* pN);
static histCmdNode* CmdRemoveTail(histCmdList* pL);

static void     CmdAdjustPointers(SYS_CMD_IO_DCPT* pCmdIO);

static void SendCommandMessage(const void* cmdIoParam, const char* str);
static void SendCommandPrint(const void* cmdIoParam, const char* format, ...);
static void SendCommandCharacter(const void* cmdIoParam, char c);
static int IsCommandReady(const void* cmdIoParam);
static char GetCommandCharacter(const void* cmdIoParam);

const SYS_CMD_API sysConsoleApi =
{
		.msg = SendCommandMessage,
		.print = SendCommandPrint,
		.putc = SendCommandCharacter,
		.isRdy = IsCommandReady,
		.getc = GetCommandCharacter,
};

// built-in command table
static const SYS_CMD_DESCRIPTOR    _builtinCmdTbl[]=
{
		{"reset",	CommandReset,	": System Reset"},
		{"help",    CommandHelp,	": help"},
		{"?",		CommandHelp,	": help"},
};

// *****************************************************************************
// *****************************************************************************
// Section: SYS CMD Operation Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    bool SYS_CMD_Initialize( const SYS_MODULE_INIT * const init  )

  Summary:
    Initializes data for the instance of the Command Processor module.

  Description:
    This function initializes the Command Processor module.
    It also initializes any internal data structures.

  Precondition:
    None.

  Parameters:
    init            - Pointer to a data structure containing any data necessary
                      to initialize the sys command. This pointer may be null if no
                      data is required because static overrides have been
                      provided.

  Returns:
    If successful, returns true.
    If there is an error, returns false.

  Remarks:
    This routine should only be called once during system initialization.
 */
bool SYS_CMD_Initialize(const SYS_MODULE_INIT * const init )
{
	SYS_CMD_INIT *initConfig = (SYS_CMD_INIT*)init;

	if (initConfig == 0)
	{
		return false;
	}

	_cmdInitData = *initConfig; // save a copy of the initialization data


	cmdIODevList.head = cmdIODevList.tail = 0;

	_cmdInitData.consoleIndex = initConfig->consoleIndex;

	return true;
}


// add new command group
bool  SYS_CMD_ADDGRP(const SYS_CMD_DESCRIPTOR* pCmdTbl, int nCmds, const char* groupName, const char* menuStr)
{
	int i, groupIx = -1, emptyIx = -1;
	int insertIx;

	// Check if there is space for new command group; If this table already added, also simply update.
	for (i=0; i<MAX_CMD_GROUP; i++)
	{
		if(_usrCmdTbl[i].pCmd == 0)
		{   // empty slot
			emptyIx = i;
		}
		else if(_usrCmdTbl[i].pCmd == pCmdTbl)
		{   // already have this group; sanity check against the group name
			if(strcmp(groupName, _usrCmdTbl[i].cmdGroupName) != 0)
			{   // name mismatch
				return false;
			}

			groupIx = i;
			break;
		}
	}

	// reference the command group
	if (groupIx != -1)
	{
		insertIx = groupIx;
	}
	else if(emptyIx != -1)
	{
		insertIx = emptyIx;
	}
	else
	{
		return false;
	}

	_usrCmdTbl[insertIx].pCmd = pCmdTbl;
	_usrCmdTbl[insertIx].nCmds = nCmds;
	_usrCmdTbl[insertIx].cmdGroupName = groupName;
	_usrCmdTbl[insertIx].cmdMenuStr = menuStr;
	return true;

}

void RunCmdTask(SYS_CMD_IO_DCPT* pCmdIO)
{
	char newCh;
	int ix, len;
	const KEY_SEQ_DCPT *pKeyDcpt, *pFoundSeq;
	const SYS_CMD_API* pCmdApi = pCmdIO->devNode.pCmdApi;
	const void* cmdIoParam = pCmdIO->devNode.cmdIoParam;

	// Check if there's characters available
	if(!(*pCmdApi->isRdy)(cmdIoParam))
	{
		return;
	}

	// read the character
	newCh = (*pCmdApi->getc)(cmdIoParam); /* Read data from console. */

	if(pCmdIO->seqChars != 0)
	{   // in the middle of escape sequence
		pCmdIO->seqBuff[pCmdIO->seqChars] = newCh;
		pCmdIO->seqChars++;

		if(pCmdIO->seqChars == VT100_DETECT_SEQ_SIZE)
		{   // detect the exact escape sequence
			pCmdIO->seqBuff[pCmdIO->seqChars] = 0;
			pKeyDcpt = keySeqTbl;
			pFoundSeq = 0;
			for(ix = 0; ix < sizeof(keySeqTbl) / sizeof(*keySeqTbl); ix++, pKeyDcpt++)
			{
				if(strncmp(pCmdIO->seqBuff, pKeyDcpt->keyCode, VT100_DETECT_SEQ_SIZE) == 0)
				{   // found it
					pFoundSeq = pKeyDcpt;
					break;
				}
			}

			if(pFoundSeq == 0)
			{   // unknown escape sequence
				pCmdIO->seqChars = 0;
				return;
			}

			pCmdIO->pSeqDcpt = pFoundSeq;
		}

		if(pCmdIO->seqChars >= VT100_DETECT_SEQ_SIZE && pCmdIO->seqChars == pCmdIO->pSeqDcpt->keySize)
		{   // check for complete sequence
			if(strcmp(pCmdIO->seqBuff, pCmdIO->pSeqDcpt->keyCode) == 0)
			{   // process sequence
				(*pCmdIO->pSeqDcpt->keyFnc)(pCmdIO, pCmdIO->pSeqDcpt);
			}

			// else unknown sequence ?
			pCmdIO->seqChars = 0;
			return;
		}

		return;

	}
	else if((newCh == '\r') || (newCh == '\n'))
	{   // new command assembled
		if(pCmdIO->cmdEnd ==  pCmdIO->cmdBuff)
		{   // just an extra \n or \r
			(*pCmdApi->msg)(cmdIoParam, LINE_TERM _promptStr);
			return;
		}
		(*pCmdApi->msg)(cmdIoParam, LINE_TERM);
		*pCmdIO->cmdEnd = 0;
		pCmdIO->cmdPnt = pCmdIO->cmdEnd = pCmdIO->cmdBuff;

		ParseCmdBuffer(pCmdIO);
		(*pCmdApi->msg)(cmdIoParam, _promptStr);
	}
	else if(newCh == '\b')
	{
		if(pCmdIO->cmdPnt > pCmdIO->cmdBuff)
		{
			if(pCmdIO->cmdEnd > pCmdIO->cmdPnt)
			{
				char* pSrc = pCmdIO->cmdPnt; // current
				char* pDst = pCmdIO->cmdPnt - 1;
				len = pCmdIO->cmdEnd - pSrc;
				for(ix = 0; ix < len; ix++)
				{
					*pDst++ = *pSrc++;
				}
				pCmdIO->cmdPnt--; pCmdIO->cmdEnd--;
				// update the display; erase to the end of line(<ESC>[K) and move cursor backwards (<ESC>[{COUNT}D)
				*pCmdIO->cmdEnd = '\0';
				sprintf(pCmdIO->ctrlBuff, "\b\x1b[K%s\x1b[%dD", pCmdIO->cmdPnt, len);
				(*pCmdApi->msg)(cmdIoParam, pCmdIO->ctrlBuff);
			}
			else
			{   // delete char under cursor
				(*pCmdApi->msg)(cmdIoParam, "\b\x1b[K");
				pCmdIO->cmdPnt--; pCmdIO->cmdEnd--;
			}
		}
	}
	else if(newCh == 0x7f)
	{   // delete
		if(pCmdIO->cmdEnd > pCmdIO->cmdPnt)
		{
			char* pSrc = pCmdIO->cmdPnt + 1;
			char* pDst = pCmdIO->cmdPnt;
			len = pCmdIO->cmdEnd - pSrc;
			for(ix = 0; ix < len; ix++)
			{
				*pDst++ = *pSrc++;
			}
			pCmdIO->cmdEnd--;
			// update the display; erase to the end of line(<ESC>[K) and move cursor backwards (<ESC>[{COUNT}D)
			*pCmdIO->cmdEnd = '\0';
			sprintf(pCmdIO->ctrlBuff, "\x1b[K%s\x1b[%dD", pCmdIO->cmdPnt, len);
			(*pCmdApi->msg)(cmdIoParam, pCmdIO->ctrlBuff);
		}
	}
	else if(newCh == 0x1b)
	{   // start escape sequence... wait for complete sequence
		pCmdIO->seqBuff[0] = newCh;
		pCmdIO->seqChars = 1;
	}
	else if(pCmdIO->cmdEnd - pCmdIO->cmdBuff < sizeof(pCmdIO->cmdBuff) - 1)
	{   // valid char; insert and echo it back
		int n_chars = pCmdIO->cmdEnd - pCmdIO->cmdPnt;  // existent chars
		if(n_chars != 0)
		{   // move the existing chars to the right, for insertion...
			char* pSrc = pCmdIO->cmdEnd - 1;
			char* pDst = pCmdIO->cmdEnd;
			for(ix = 0; ix < n_chars; ix++)
			{
				*pDst-- = *pSrc--;
			}
			pCmdIO->cmdEnd++;
			*pCmdIO->cmdEnd = '\0';
			sprintf(pCmdIO->ctrlBuff + 1, "%s\x1b[%dD", pCmdIO->cmdPnt + 1, n_chars);
		}
		else
		{
			pCmdIO->ctrlBuff[1] = 0;
		}
		pCmdIO->ctrlBuff[0] = newCh;

		(*pCmdApi->msg)(cmdIoParam, pCmdIO->ctrlBuff);
		*pCmdIO->cmdPnt++ = newCh;
		CmdAdjustPointers(pCmdIO);
	}
	else
	{
		(*pCmdApi->msg)(cmdIoParam, " *** Command Processor buffer exceeded. Retry. ***" LINE_TERM);
		pCmdIO->cmdPnt = pCmdIO->cmdEnd = pCmdIO->cmdBuff;
		(*pCmdApi->msg)(cmdIoParam, _promptStr);
	}
}

// *****************************************************************************
/* Function:
    void SYS_CMD_MESSAGE (const char* message)

  Summary:
    Outputs a message to the Command Processor System Service console.

  Description:
    This function outputs a message to the Command Processor System Service
    console.
.
  Precondition:
    SYS_CMD_Initialize was successfully run once.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
 */
void SYS_CMD_MESSAGE(const char* message)
{
	SendCommandMessage(NULL, message);
}

// *****************************************************************************
/* Function:
    void SYS_CMD_PRINT(const char *format, ...)

  Summary:
    Outputs a printout to the Command Processor System Service console.

  Description:
    This function outputs a printout to the Command Processor System Service
    console.
.
  Precondition:
    SYS_CMD_Initialize was successfully run once.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
 */
void SYS_CMD_PRINT(const char* format, ...)
{
	char tmpBuf[SYS_CMD_PRINT_BUFFER_SIZE];
	size_t len = 0;
	size_t padding = 0;
	va_list args = {0};
	va_start( args, format );

	len = vsnprintf(tmpBuf, SYS_CMD_PRINT_BUFFER_SIZE, format, args);

	va_end( args );

	if (len > 0 && len < SYS_CMD_PRINT_BUFFER_SIZE)
	{
		tmpBuf[len] = '\0';

		if (len + printBuffPtr >= SYS_CMD_PRINT_BUFFER_SIZE)
		{
			printBuffPtr = 0;
		}

		strcpy(&printBuff[printBuffPtr], tmpBuf);
		SendCommandMessage(NULL, &printBuff[printBuffPtr]);

		padding = len % 4;

		if (padding > 0)
		{
			padding = 4 - padding;
		}

		printBuffPtr += len + padding;
	}
}

SYS_CMD_DEVICE_NODE* SYS_CMDIO_GET_HANDLE(short num)
{
	SYS_CMD_IO_DCPT* pNode = cmdIODevList.head;

	while(num && pNode)
	{
		pNode = pNode->next;
		num--;
	}

	return pNode == 0 ? 0 : &pNode->devNode;
}

SYS_CMD_DEVICE_NODE* SYS_CMDIO_ADD(const SYS_CMD_API* opApi, const void* cmdIoParam, int unused)
{
	int ix;

	// Create new node
	SYS_CMD_IO_DCPT* pNewIo;

	pNewIo = (SYS_CMD_IO_DCPT*)malloc (sizeof(*pNewIo));
	if (!pNewIo) {
		return 0;
	}
	memset(pNewIo, 0, sizeof(*pNewIo));
	pNewIo->devNode.pCmdApi = opApi;
	pNewIo->devNode.cmdIoParam = cmdIoParam;
	pNewIo->cmdPnt = pNewIo->cmdEnd = pNewIo->cmdBuff;

	// construct the command history list
	for(ix = 0; ix < sizeof(pNewIo->histArray) / sizeof(*pNewIo->histArray); ix++) {
		CmdAddHead(&pNewIo->histList, pNewIo->histArray + ix);
	}

	// Insert node at end
	pNewIo->next = 0;
	if(cmdIODevList.head == 0) {
		cmdIODevList.head = pNewIo;
		cmdIODevList.tail = pNewIo;
	}
	else {
		cmdIODevList.tail->next = pNewIo;
		cmdIODevList.tail = pNewIo;
	}

	return &pNewIo->devNode;
}


bool SYS_CMD_DELETE(SYS_CMD_DEVICE_NODE* pDeviceNode)
{
	SYS_CMD_IO_DCPT* p_listnode = cmdIODevList.head;
	SYS_CMD_IO_DCPT* pre_listnode;
	SYS_CMD_IO_DCPT* pDevNode = (SYS_CMD_IO_DCPT*)pDeviceNode;

	// root list is empty or null node to be deleted
	if((p_listnode == NULL) || (pDevNode == NULL))
	{
		return false;
	}

	if(p_listnode == pDevNode)
	{   // delete the head
		//Root list has only one node
		if(cmdIODevList.tail == pDevNode)
		{
			cmdIODevList.head = NULL;
			cmdIODevList.tail = NULL;
		}
		else
		{
			cmdIODevList.head = p_listnode->next;
		}
		free(pDevNode);
		return true;
	}

	// delete mid node
	pre_listnode = p_listnode;
	while (p_listnode)
	{
		if(p_listnode == pDevNode)
		{
			pre_listnode->next = p_listnode->next;
			// Deleted node is tail
			if (cmdIODevList.tail==pDevNode) {
				cmdIODevList.tail = pre_listnode;
			}
			free(pDevNode);
			return true;
		}
		pre_listnode = p_listnode;
		p_listnode   = p_listnode->next;
	}


	return false;
}

// ignore the console handle for now, we support a single system console
static void SendCommandMessage(const void* cmdIoParam, const char* message)
{
	SYS_CONSOLE_Write(_cmdInitData.consoleIndex, message, strlen(message));
}

static void SendCommandPrint(const void* cmdIoParam, const char* format, ...)
{
	char tmpBuf[SYS_CMD_PRINT_BUFFER_SIZE];
	size_t len = 0;
	size_t padding = 0;
	va_list args = {0};
	va_start( args, format );

	len = vsnprintf(tmpBuf, SYS_CMD_PRINT_BUFFER_SIZE, format, args);

	va_end( args );


	if (len > 0 && len < SYS_CMD_PRINT_BUFFER_SIZE)
	{
		tmpBuf[len] = '\0';

		if (len + printBuffPtr >= SYS_CMD_PRINT_BUFFER_SIZE)
		{
			printBuffPtr = 0;
		}

		strcpy(&printBuff[printBuffPtr], tmpBuf);
		SendCommandMessage(NULL, &printBuff[printBuffPtr]);

		padding = len % 4;

		if (padding > 0)
		{
			padding = 4 - padding;
		}

		printBuffPtr += len + padding;
	}
}

static void SendCommandCharacter(const void* cmdIoParam, char c)
{
	if (SYS_CONSOLE_Status((SYS_MODULE_OBJ)_cmdInitData.consoleIndex) == SYS_STATUS_READY)
	{
		SYS_CONSOLE_Write(_cmdInitData.consoleIndex, (const char*)&c, 1);
	}
}


static int IsCommandReady(const void* cmdIoParam)
{
	return (int)SYS_CONSOLE_ReadCountGet(_cmdInitData.consoleIndex);
}

static char GetCommandCharacter(const void* cmdIoParam)
{
	char new_c;

	SYS_CONSOLE_Read(_cmdInitData.consoleIndex, &new_c, 1);

	return new_c;
}

bool cli_routine_job_timer_stop ();

void doSystemReset()
{
	__disable_irq ();

	/* Disable Systick timer */
	SysTick->CTRL = 0;
	SysTick->LOAD = 0;
	SysTick->VAL = 0;

	for (IRQn_Type i = 0 ; i < FPU_IRQn ; i++) {
		__NVIC_DisableIRQ(i);
	}

	// 여기서 boot loader 에서 사용했던 uart 를 포함한 모든 controller 를 DeInit 한다.
	assert (HAL_UART_AbortReceive_IT(&huart1) == HAL_OK);		// 이것을 넣어 주니 app jump 한 다음에 app 의 console 에서 2char 받아야 반응하는 현상이 없어졌다..
	HAL_UART_MspDeInit(&huart1);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 여기까지 모든 io contoller 를 시스템 boot 와 같은 상태로 만들기 위한 작업을 완료한다.
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	__HAL_RCC_GPIOA_CLK_DISABLE();
	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOC_CLK_DISABLE();
	__HAL_RCC_GPIOD_CLK_DISABLE();
	__HAL_RCC_GPIOE_CLK_DISABLE();
	__HAL_RCC_GPIOF_CLK_DISABLE();
	__HAL_RCC_GPIOG_CLK_DISABLE();
	__HAL_RCC_GPIOH_CLK_DISABLE();
	__HAL_RCC_GPIOI_CLK_DISABLE();

	assert (HAL_RCC_DeInit() == HAL_OK);
	assert (HAL_DeInit() == HAL_OK);

	__enable_irq();
	HAL_NVIC_SystemReset ();
}

static void CommandReset (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
	const void* cmdIoParam = pCmdIO->cmdIoParam;

	if (argc > 2) {
		//ShitMessage (pCmdIO);
		(*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM " reset (fac) \r\n" );
		return;
	}
	if (argc == 1) {
		(*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, CUI_ESC_CLR);
		(*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, CUI_ESC_CUR_HOME);
		(*pCmdIO->pCmdApi->msg)(pCmdIO->cmdIoParam, LINE_TERM"====== MANUAL SYSTEM RESET ======="LINE_TERM);
		HAL_Delay(4);
		doSystemReset();
		while(1) {};
	}
	else if (argc == 2) {
		if (!strcmp(argv[1], "fac") || !strcmp(argv[1], "facory")) {
			(*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM " *** System Reboot with FACTORY DEFAULT ***\r\n" );

			//boardObj.board_info_valid_code = ~BOARD_INFO_VALID_CODE;

			//assert (EEPROM_WriteBuffer (EEPROM_ENV_DATA_ADDR, (uint8_t *)&boardObj, sizeof(boardObj_t)) == true);
			(*pCmdIO->pCmdApi->msg)(cmdIoParam, "env data gone out !!" LINE_TERM);
			vTaskDelay(3 / portTICK_PERIOD_MS);
			__disable_irq ();
			HAL_NVIC_SystemReset ();
			return;
		}
		else {
			//ShitMessage (pCmdIO);
			(*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM " reset (fac) \r\n" );
			return;
		}
	}
}

static void CommandHelp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
	int ix, groupIx;
	const SYS_CMD_DESCRIPTOR*  pDcpt;
	const SYS_CMD_DESCRIPTOR_TABLE* pTbl, *pDTbl;
	const void* cmdIoParam = pCmdIO->cmdIoParam;

	if(argc == 1)
	{   // no params help; display basic info
		bool hadHeader = false;
		pTbl = _usrCmdTbl;
		for (groupIx=0; groupIx < MAX_CMD_GROUP; groupIx++)
		{
			if (pTbl->pCmd)
			{
				if(!hadHeader)
				{
					(*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM "------- Supported command groups ------");
					hadHeader = true;
				}
				(*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM " *** ");
				(*pCmdIO->pCmdApi->msg)(cmdIoParam, pTbl->cmdGroupName);
				(*pCmdIO->pCmdApi->msg)(cmdIoParam, pTbl->cmdMenuStr);
				//(*pCmdIO->pCmdApi->msg)(cmdIoParam, " ***");
			}
			pTbl++;
		}

		(*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM "---------- Built in commands ----------");
		for(ix = 0, pDcpt = _builtinCmdTbl; ix < sizeof(_builtinCmdTbl)/sizeof(*_builtinCmdTbl); ix++, pDcpt++)
		{
			(*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM " *** ");
			(*pCmdIO->pCmdApi->msg)(cmdIoParam, pDcpt->cmdStr);
			(*pCmdIO->pCmdApi->msg)(cmdIoParam, pDcpt->cmdDescr);
			//(*pCmdIO->pCmdApi->msg)(cmdIoParam, " ***");
		}
		(*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM);
	}
	else
	{   // we have a command group name
		pDTbl = 0;
		pTbl = _usrCmdTbl;
		for (groupIx=0; groupIx < MAX_CMD_GROUP; groupIx++)
		{
			if (pTbl->pCmd)
			{
				if(strcmp(pTbl->cmdGroupName, argv[1]) == 0)
				{   // match
					pDTbl = pTbl;
					break;
				}
			}
			pTbl++;
		}

		if(pDTbl)
		{
			for(ix = 0, pDcpt = pDTbl->pCmd; ix < pDTbl->nCmds; ix++, pDcpt++)
			{
				(*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM " *** ");
				(*pCmdIO->pCmdApi->msg)(cmdIoParam, pDcpt->cmdStr);
				(*pCmdIO->pCmdApi->msg)(cmdIoParam, pDcpt->cmdDescr);
				//(*pCmdIO->pCmdApi->msg)(cmdIoParam, " ***");
			}

			(*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM);
		}
		else
		{
			(*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM "Unknown command group. Try help" LINE_TERM );
		}
	}
}

static void ParseCmdBuffer(SYS_CMD_IO_DCPT* pCmdIO)
{
	int  argc = 0;
	char *argv[MAX_CMD_ARGS] = {0};
	char saveCmd[SYS_CMD_MAX_LENGTH + 1];
	const void* cmdIoParam = pCmdIO->devNode.cmdIoParam;

	int            ix, grp_ix;
	const SYS_CMD_DESCRIPTOR* pDcpt;

	strncpy(saveCmd, pCmdIO->cmdBuff, sizeof(saveCmd));     // make a copy of the command

	// parse a command string to *argv[]
	argc = StringToArgs(saveCmd, argv, MAX_CMD_ARGS);

	if(argc > MAX_CMD_ARGS)
	{
		(*pCmdIO->devNode.pCmdApi->print)(cmdIoParam, "\n\r Too many arguments. Maximum args supported: %d!\r\n", MAX_CMD_ARGS);
	}
	else if(argc == 0)
	{
		(*pCmdIO->devNode.pCmdApi->msg)(cmdIoParam, " *** Command Processor: Please type in a command***" LINE_TERM);
	}
	else
	{
		if(argc > 0)
		{   // ok, there's smth here
			// add it to the history list
			histCmdNode* pN = CmdRemoveTail(&pCmdIO->histList);
			strncpy(pN->cmdBuff, pCmdIO->cmdBuff, sizeof(saveCmd)); // Need save non-parsed string
			CmdAddHead(&pCmdIO->histList, pN);
			pCmdIO->currHistN = 0;

			// try built-in commands first
			for(ix = 0, pDcpt = _builtinCmdTbl; ix < sizeof(_builtinCmdTbl)/sizeof(*_builtinCmdTbl); ix++, pDcpt++)
			{
				if(!strcmp(argv[0], pDcpt->cmdStr))
				{   // command found
					(*pDcpt->cmdFnc)(&pCmdIO->devNode, argc, argv);     // call command handler
					return;
				}
			}

			// search user commands
			for (grp_ix=0; grp_ix < MAX_CMD_GROUP; grp_ix++)
			{
				if (_usrCmdTbl[grp_ix].pCmd == 0)
				{
					continue;
				}

				for(ix = 0, pDcpt = _usrCmdTbl[grp_ix].pCmd; ix < _usrCmdTbl[grp_ix].nCmds; ix++, pDcpt++)
				{
					if(!strcmp(argv[0], pDcpt->cmdStr))
					{
						// command found
						(*pDcpt->cmdFnc)(&pCmdIO->devNode, argc, argv);
						return;
					}
				}
			}
		}

		// command not found
		(*pCmdIO->devNode.pCmdApi->msg)(cmdIoParam, " *** Command Processor: unknown command. ***\r\n");
	}
}

/*
  parse a string into '*argv[]' tokens
  token delimitor is space, tab or comma
  parts within quotes (") are parsed as a single token
  return number of parsed tokens
  < 0 if error
 */
static int StringToArgs(char *str, char *argv[], size_t argvSize)
{
	char* pTkn;
	char* qStart, *qEnd;   // special char '"' starting position;
	int nArgs = 0;

	while(str)
	{
		qStart = strchr(str, '"');
		if(qStart != 0)
		{
			*qStart = 0;
		}

		// parse until quote
		while((pTkn = strtok(str, " \t,")) != 0)
		{
			str = 0;
			if(nArgs < argvSize)
			{
				argv[nArgs] = pTkn;
			}
			nArgs++;
		}

		if(qStart == 0)
		{   // done
			break;
		}

		// get end quote
		qStart++;
		qEnd = strchr(qStart, '"');
		if(qEnd == 0 || qEnd - qStart == 0)
		{   // no matching quote end or empty string within quotes
			return -1;
		}
		*qEnd = 0;
		if(nArgs < argvSize)
		{
			argv[nArgs] = qStart;
		}
		nArgs++;

		// continue parsing
		str = qEnd + 1;
	}


	return nArgs;
}

static void _keyUpProcess(SYS_CMD_IO_DCPT* pCmdIO, const KEY_SEQ_DCPT* pSeqDcpt)
{   // up arrow
	histCmdNode *pNext;

	if(pCmdIO->currHistN)
	{
		pNext = pCmdIO->currHistN->next;
		if(pNext == pCmdIO->histList.head)
		{
			return; // reached the end of list
		}
	}
	else
	{
		pNext = pCmdIO->histList.head;
	}

	DisplayNodeMsg(pCmdIO, pNext);
}

static void _keyDownProcess(SYS_CMD_IO_DCPT* pCmdIO, const KEY_SEQ_DCPT* pSeqDcpt)
{   // down arrow
	histCmdNode *pNext;

	if(pCmdIO->currHistN)
	{
		pNext = pCmdIO->currHistN->prev;
		if(pNext != pCmdIO->histList.tail)
		{
			DisplayNodeMsg(pCmdIO, pNext);
		}
	}
}

static void _keyRightProcess(SYS_CMD_IO_DCPT* pCmdIO, const KEY_SEQ_DCPT* pSeqDcpt)
{   // right arrow
	const SYS_CMD_API* pCmdApi = pCmdIO->devNode.pCmdApi;
	const void* cmdIoParam = pCmdIO->devNode.cmdIoParam;

	if(pCmdIO->cmdPnt < pCmdIO->cmdEnd)
	{   // just advance
		(*pCmdApi->msg)(cmdIoParam, pSeqDcpt->keyCode);
		pCmdIO->cmdPnt++;
	}
}

static void _keyLeftProcess(SYS_CMD_IO_DCPT* pCmdIO, const KEY_SEQ_DCPT* pSeqDcpt)
{   // left arrow
	const SYS_CMD_API* pCmdApi = pCmdIO->devNode.pCmdApi;
	const void* cmdIoParam = pCmdIO->devNode.cmdIoParam;

	if(pCmdIO->cmdPnt > pCmdIO->cmdBuff)
	{
		pCmdIO->cmdPnt--;
		(*pCmdApi->msg)(cmdIoParam, pSeqDcpt->keyCode);
	}
}

static void _keyHomeProcess(SYS_CMD_IO_DCPT* pCmdIO, const KEY_SEQ_DCPT* pSeqDcpt)
{   // home key
	const SYS_CMD_API* pCmdApi = pCmdIO->devNode.pCmdApi;
	const void* cmdIoParam = pCmdIO->devNode.cmdIoParam;
	int nChars = pCmdIO->cmdPnt - pCmdIO->cmdBuff;
	if(nChars != 0)
	{
		// <ESC>[{COUNT}D
		char homeBuff[ 10 + 1];
		sprintf(homeBuff, "\x1b[%dD", nChars);
		(*pCmdApi->msg)(cmdIoParam, homeBuff);
		pCmdIO->cmdPnt = pCmdIO->cmdBuff;
	}
}

static void _keyEndProcess(SYS_CMD_IO_DCPT* pCmdIO, const KEY_SEQ_DCPT* pSeqDcpt)
{   // end key
	const SYS_CMD_API* pCmdApi = pCmdIO->devNode.pCmdApi;
	const void* cmdIoParam = pCmdIO->devNode.cmdIoParam;

	int nChars = pCmdIO->cmdEnd - pCmdIO->cmdPnt;
	if(nChars)
	{
		// "<ESC>[{COUNT}C"
		char endBuff[ 10 + 1];
		sprintf(endBuff, "\x1b[%dC", nChars);
		(*pCmdApi->msg)(cmdIoParam, endBuff);
		pCmdIO->cmdPnt = pCmdIO->cmdEnd;
	}

}

static void DisplayNodeMsg(SYS_CMD_IO_DCPT* pCmdIO, histCmdNode* pNext)
{
	int oCmdLen, nCmdLen;
	const SYS_CMD_API* pCmdApi = pCmdIO->devNode.pCmdApi;
	const void* cmdIoParam = pCmdIO->devNode.cmdIoParam;

	if((nCmdLen = strlen(pNext->cmdBuff)))
	{   // something there
		oCmdLen = pCmdIO->cmdEnd - pCmdIO->cmdBuff;
		while(oCmdLen > nCmdLen)
		{
			(*pCmdApi->msg)(cmdIoParam, "\b \b");     // clear the old command
			oCmdLen--;
		}
		while(oCmdLen--)
		{
			(*pCmdApi->msg)(cmdIoParam, "\b");
		}
		strcpy(pCmdIO->cmdBuff, pNext->cmdBuff);
		(*pCmdApi->msg)(cmdIoParam, "\r\n>");
		(*pCmdApi->msg)(cmdIoParam, pCmdIO->cmdBuff);
		pCmdIO->cmdPnt = pCmdIO->cmdEnd = pCmdIO->cmdBuff + nCmdLen;
		pCmdIO->currHistN = pNext;
	}
}


static void CmdAddHead(histCmdList* pL, histCmdNode* pN)
{
	if(pL->head == 0)
	{ // empty list, first node
		pL->head = pL->tail = pN;
		pN->next = pN->prev = pN;
	}
	else
	{
		pN->next = pL->head;
		pN->prev = pL->tail;
		pL->tail->next = pN;
		pL->head->prev = pN;
		pL->head = pN;
	}
}

static histCmdNode* CmdRemoveTail(histCmdList* pL)
{
	histCmdNode* pN;
	if(pL->head == pL->tail)
	{
		pN = pL->head;
		pL->head = pL->tail = 0;
	}
	else
	{
		pN = pL->tail;
		pL->tail = pN->prev;
		pL->tail->next = pL->head;
		pL->head->prev = pL->tail;
	}
	return pN;
}

static void CmdAdjustPointers(SYS_CMD_IO_DCPT* pCmdIO)
{
	if(pCmdIO->cmdPnt > pCmdIO->cmdEnd)
	{
		pCmdIO->cmdEnd = pCmdIO->cmdPnt;
	}
}
