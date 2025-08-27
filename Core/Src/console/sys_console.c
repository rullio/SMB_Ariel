/**
 ******************************************************************************
 * @file           : sys_console.c
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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "main.h"

static SYS_CONSOLE_OBJECT_INSTANCE consoleDeviceInstance[SYS_CONSOLE_DEVICE_MAX_INSTANCES];
static bool isConsoleMutexCreated = false;
#define SYS_CONSOLE_GET_INSTANCE(index)    (index >= SYS_CONSOLE_DEVICE_MAX_INSTANCES)? NULL : &consoleDeviceInstance[index]

SYS_MODULE_OBJ SYS_CONSOLE_Initialize (const SYS_MODULE_INDEX index, const SYS_MODULE_INIT* const init)
{
	const SYS_CONSOLE_INIT* initConfig = (const SYS_CONSOLE_INIT* )init;
	SYS_CONSOLE_OBJECT_INSTANCE* pConsoleObj;

	if (isConsoleMutexCreated == false) {
		if(osMutexNew(NULL) == NULL) {
			return SYS_MODULE_OBJ_INVALID;
		}
		else {
			isConsoleMutexCreated = true;
		}
	}

	/* Confirm valid arguments */
	if (index >= SYS_CONSOLE_DEVICE_MAX_INSTANCES || init == NULL) {
		return SYS_MODULE_OBJ_INVALID;
	}

	pConsoleObj = &consoleDeviceInstance[index];

	if ((pConsoleObj->status == SYS_STATUS_UNINITIALIZED) && initConfig) {
		pConsoleObj->devIndex = initConfig->deviceIndex;
		pConsoleObj->devDesc = initConfig->consDevDesc;
		pConsoleObj->status = SYS_STATUS_READY;
		pConsoleObj->devDesc->init( pConsoleObj->devIndex, initConfig->deviceInitData);

		return index;
	}

	return SYS_MODULE_OBJ_INVALID;
}

SYS_STATUS SYS_CONSOLE_Status ( SYS_MODULE_OBJ object )
{
	SYS_STATUS ret = SYS_STATUS_ERROR;
	SYS_CONSOLE_STATUS status;
	SYS_MODULE_INDEX index = (SYS_MODULE_INDEX)object;
	SYS_CONSOLE_OBJECT_INSTANCE* pConsoleObj;

	if (index >= SYS_CONSOLE_DEVICE_MAX_INSTANCES) {
		return SYS_STATUS_ERROR;
	}

	pConsoleObj = &consoleDeviceInstance[index];

	if (pConsoleObj->devDesc == NULL) {
		return SYS_STATUS_UNINITIALIZED;
	}

	status = pConsoleObj->devDesc->status(pConsoleObj->devIndex);

	switch (status)	{
	case SYS_CONSOLE_STATUS_NOT_CONFIGURED:
		ret = SYS_STATUS_UNINITIALIZED;
		break;
	case SYS_CONSOLE_STATUS_CONFIGURED:
		ret = SYS_STATUS_READY;
		break;
	case SYS_CONSOLE_STATUS_BUSY:
		ret = SYS_STATUS_BUSY;
		break;
	case SYS_CONSOLE_STATUS_ERROR:
		ret = SYS_STATUS_ERROR;
		break;
	default:
		break;
	}

	return ret;
}

size_t SYS_CONSOLE_Read(const SYS_CONSOLE_HANDLE handle, void* buf,	size_t count)
{
	SYS_CONSOLE_OBJECT_INSTANCE* pConsoleObj = SYS_CONSOLE_GET_INSTANCE(handle);

	if (pConsoleObj) {
		if (pConsoleObj->status == SYS_STATUS_UNINITIALIZED || pConsoleObj->devDesc == NULL) {
			return -1;
		}

		return pConsoleObj->devDesc->read(pConsoleObj->devIndex, buf, count);
	}
	else {
		return -1;
	}

}

size_t SYS_CONSOLE_ReadCountGet(const SYS_CONSOLE_HANDLE handle)
{
	SYS_CONSOLE_OBJECT_INSTANCE* pConsoleObj = SYS_CONSOLE_GET_INSTANCE(handle);

	if (pConsoleObj) {
		if (pConsoleObj->status == SYS_STATUS_UNINITIALIZED || pConsoleObj->devDesc == NULL) {
			return -1;
		}

		return pConsoleObj->devDesc->readCountGet(pConsoleObj->devIndex);
	}
	else {
		return -1;
	}
}

size_t SYS_CONSOLE_Write(const SYS_CONSOLE_HANDLE handle, const void* buf, size_t count)
{
	SYS_CONSOLE_OBJECT_INSTANCE* pConsoleObj = SYS_CONSOLE_GET_INSTANCE(handle);

	if (pConsoleObj) {
		if (pConsoleObj->status == SYS_STATUS_UNINITIALIZED || pConsoleObj->devDesc == NULL) {
			return -1;
		}

		return pConsoleObj->devDesc->write(pConsoleObj->devIndex, buf, count);
	}
	else {
		return -1;
	}
}


/*******************************************************************************
 End of File
 */
