/***********************************************************************************************************

  Author:     Nima Askari
  Github:     https://www.github.com/NimaLTD
  LinkedIn:   https://www.linkedin.com/in/nimaltd
  Youtube:    https://www.youtube.com/@nimaltd
  Instagram:  https://instagram.com/github.NimaLTD

  Version:    2.2.0

  History:
							2.2.0
              - Add SPI_Trasmit and SPI_Receive again :)

              2.1.0
              - Add Support HAL-DMA
              - Remove SPI_Trasmit function

              2.0.1
              - Remove SPI_Receive function

              2.0.0
              - Rewrite again
              - Support STM32CubeMx Packet installer

 ***********************************************************************************************************/
#include "main.h"

extern SPI_HandleTypeDef hspi1;

#if SPIF_DEBUG == SPIF_DEBUG_DISABLE
#define dprintf(...)
#else
#define dprintf(...) printf(__VA_ARGS__)
#endif

#define SPIF_DUMMY_BYTE 0xA5

#define SPIF_CMD_READSFDP 0x5A
#define SPIF_CMD_ID 0x90
#define SPIF_CMD_JEDECID 0x9F
#define SPIF_CMD_UNIQUEID 0x4B
#define SPIF_CMD_WRITEDISABLE 0x04
#define SPIF_CMD_READSTATUS1 0x05
#define SPIF_CMD_READSTATUS2 0x35
#define SPIF_CMD_READSTATUS3 0x15
#define SPIF_CMD_WRITESTATUSEN 0x50
#define SPIF_CMD_WRITESTATUS1 0x01
#define SPIF_CMD_WRITESTATUS2 0x31
#define SPIF_CMD_WRITESTATUS3 0x11
#define SPIF_CMD_WRITEENABLE 0x06
#define SPIF_CMD_ADDR4BYTE_EN 0xB7
#define SPIF_CMD_ADDR4BYTE_DIS 0xE9
#define SPIF_CMD_PAGEPROG3ADD 0x02
#define SPIF_CMD_PAGEPROG4ADD 0x12
#define SPIF_CMD_READDATA3ADD 0x03
#define SPIF_CMD_READDATA4ADD 0x13
#define SPIF_CMD_FASTREAD3ADD 0x0B
#define SPIF_CMD_FASTREAD4ADD 0x0C
#define SPIF_CMD_SECTORERASE3ADD 0x20
#define SPIF_CMD_SECTORERASE4ADD 0x21
#define SPIF_CMD_BLOCKERASE3ADD 0xD8
#define SPIF_CMD_BLOCKERASE4ADD 0xDC
#define SPIF_CMD_CHIPERASE1 0x60
#define SPIF_CMD_CHIPERASE2 0xC7
#define SPIF_CMD_SUSPEND 0x75
#define SPIF_CMD_RESUME 0x7A
#define SPIF_CMD_POWERDOWN 0xB9
#define SPIF_CMD_RELEASE 0xAB
#define SPIF_CMD_FRAMSERNO 0xC3

#define SPIF_STATUS1_BUSY (1 << 0)
#define SPIF_STATUS1_WEL (1 << 1)
#define SPIF_STATUS1_BP0 (1 << 2)
#define SPIF_STATUS1_BP1 (1 << 3)
#define SPIF_STATUS1_BP2 (1 << 4)
#define SPIF_STATUS1_TP (1 << 5)
#define SPIF_STATUS1_SEC (1 << 6)
#define SPIF_STATUS1_SRP0 (1 << 7)

#define SPIF_STATUS2_SRP1 (1 << 0)
#define SPIF_STATUS2_QE (1 << 1)
#define SPIF_STATUS2_RESERVE1 (1 << 2)
#define SPIF_STATUS2_LB0 (1 << 3)
#define SPIF_STATUS2_LB1 (1 << 4)
#define SPIF_STATUS2_LB2 (1 << 5)
#define SPIF_STATUS2_CMP (1 << 6)
#define SPIF_STATUS2_SUS (1 << 7)

#define SPIF_STATUS3_RESERVE1 (1 << 0)
#define SPIF_STATUS3_RESERVE2 (1 << 1)
#define SPIF_STATUS3_WPS (1 << 2)
#define SPIF_STATUS3_RESERVE3 (1 << 3)
#define SPIF_STATUS3_RESERVE4 (1 << 4)
#define SPIF_STATUS3_DRV0 (1 << 5)
#define SPIF_STATUS3_DRV1 (1 << 6)
#define SPIF_STATUS3_HOLD (1 << 7)

SPIF_HandleTypeDef spif_handle;

/***********************************************************************************************************/

void SPIF_Lock(SPIF_HandleTypeDef *Handle)
{
	while (Handle->Lock)
	{
		SPIF_Delay(1);
	}
	Handle->Lock = 1;
}

/***********************************************************************************************************/

void SPIF_UnLock(SPIF_HandleTypeDef *Handle)
{
	Handle->Lock = 0;
}

/***********************************************************************************************************/

void SPIF_CsPin(SPIF_HandleTypeDef *Handle, bool Select)
{
	HAL_GPIO_WritePin(Handle->Gpio, Handle->Pin, (GPIO_PinState)Select);
	for (int i = 0; i < 10; i++);
}

/***********************************************************************************************************/

bool SPIF_TransmitReceive(SPIF_HandleTypeDef *Handle, uint8_t *Tx, uint8_t *Rx, size_t Size, uint32_t Timeout)
{
	bool retVal = false;
#if (SPIF_PLATFORM == SPIF_PLATFORM_HAL)
	if (HAL_SPI_TransmitReceive(Handle->HSpi, Tx, Rx, Size, Timeout) == HAL_OK)
	{
		retVal = true;
	}
	else
	{
		dprintf("SPIF TIMEOUT\r\n");
	}
#elif (SPIF_PLATFORM == SPIF_PLATFORM_HAL_DMA)
	uint32_t startTime = HAL_GetTick();
	if (HAL_SPI_TransmitReceive_DMA(Handle->HSpi, Tx, Rx, Size) != HAL_OK)
	{
		dprintf("SPIF TRANSFER ERROR\r\n");
	}
	else
	{
		while (1)
		{
			SPIF_Delay(1);
			if (HAL_GetTick() - startTime >= Timeout)
			{
				dprintf("SPIF TIMEOUT\r\n");
				HAL_SPI_DMAStop(Handle->HSpi);
				break;
			}
			if (HAL_SPI_GetState(Handle->HSpi) == HAL_SPI_STATE_READY)
			{
				retVal = true;
				break;
			}
		}
	}
#endif
	return retVal;
}

/***********************************************************************************************************/

bool SPIF_Transmit(SPIF_HandleTypeDef *Handle, uint8_t *Tx, size_t Size, uint32_t Timeout)
{
	bool retVal = false;
#if (SPIF_PLATFORM == SPIF_PLATFORM_HAL)
	if (HAL_SPI_Transmit(Handle->HSpi, Tx, Size, Timeout) == HAL_OK)
	{
		retVal = true;
	}
	else
	{
		dprintf("SPIF TIMEOUT\r\n");
	}
#elif (SPIF_PLATFORM == SPIF_PLATFORM_HAL_DMA)
	uint32_t startTime = HAL_GetTick();
	if (HAL_SPI_Transmit_DMA(Handle->HSpi, Tx, Size) != HAL_OK)
	{
		dprintf("SPIF TRANSFER ERROR\r\n");
	}
	else
	{
		while (1)
		{
			SPIF_Delay(1);
			if (HAL_GetTick() - startTime >= Timeout)
			{
				dprintf("SPIF TIMEOUT\r\n");
				HAL_SPI_DMAStop(Handle->HSpi);
				break;
			}
			if (HAL_SPI_GetState(Handle->HSpi) == HAL_SPI_STATE_READY)
			{
				retVal = true;
				break;
			}
		}
	}
#endif
	return retVal;
}

/***********************************************************************************************************/

bool SPIF_Receive(SPIF_HandleTypeDef *Handle, uint8_t *Rx, size_t Size, uint32_t Timeout)
{
	bool retVal = false;
#if (SPIF_PLATFORM == SPIF_PLATFORM_HAL)
	if (HAL_SPI_Receive(Handle->HSpi, Rx, Size, Timeout) == HAL_OK)
	{
		retVal = true;
	}
	else
	{
		dprintf("SPIF TIMEOUT\r\n");
	}
#elif (SPIF_PLATFORM == SPIF_PLATFORM_HAL_DMA)
	uint32_t startTime = HAL_GetTick();
	if (HAL_SPI_Receive_DMA(Handle->HSpi, Rx, Size) != HAL_OK)
	{
		dprintf("SPIF TRANSFER ERROR\r\n");
	}
	else
	{
		while (1)
		{
			SPIF_Delay(1);
			if (HAL_GetTick() - startTime >= Timeout)
			{
				dprintf("SPIF TIMEOUT\r\n");
				HAL_SPI_DMAStop(Handle->HSpi);
				break;
			}
			if (HAL_SPI_GetState(Handle->HSpi) == HAL_SPI_STATE_READY)
			{
				retVal = true;
				break;
			}
		}
	}
#endif
	return retVal;
}

/***********************************************************************************************************/

bool SPIF_WriteEnable(SPIF_HandleTypeDef *Handle)
{
	bool retVal = true;
	uint8_t tx[1] = {SPIF_CMD_WRITEENABLE};
	SPIF_CsPin(Handle, 0);
	if (SPIF_Transmit(Handle, tx, 1, 100) == false)
	{
		retVal = false;
		dprintf("SPIF_WriteEnable() Error\r\n");
	}
	SPIF_CsPin(Handle, 1);
	return retVal;
}

/***********************************************************************************************************/

bool SPIF_WriteDisable(SPIF_HandleTypeDef *Handle)
{
	bool retVal = true;
	uint8_t tx[1] = {SPIF_CMD_WRITEDISABLE};
	SPIF_CsPin(Handle, 0);
	if (SPIF_Transmit(Handle, tx, 1, 100) == false)
	{
		retVal = false;
		dprintf("SPIF_WriteDisable() Error\r\n");
	}
	SPIF_CsPin(Handle, 1);
	return retVal;
}

/***********************************************************************************************************/

uint8_t SPIF_ReadReg1(SPIF_HandleTypeDef *Handle)
{
	uint8_t retVal = 0;
	uint8_t tx[2] = {SPIF_CMD_READSTATUS1, SPIF_DUMMY_BYTE};
	uint8_t rx[2];
	SPIF_CsPin(Handle, 0);
	if (SPIF_TransmitReceive(Handle, tx, rx, 2, 100) == true)
	{
		retVal = rx[1];
	}
	SPIF_CsPin(Handle, 1);
	return retVal;
}

/***********************************************************************************************************/

uint8_t SPIF_ReadReg2(SPIF_HandleTypeDef *Handle)
{
	uint8_t retVal = 0;
	uint8_t tx[2] = {SPIF_CMD_READSTATUS2, SPIF_DUMMY_BYTE};
	uint8_t rx[2];
	SPIF_CsPin(Handle, 0);
	if (SPIF_TransmitReceive(Handle, tx, rx, 2, 100) == true)
	{
		retVal = rx[1];
	}
	SPIF_CsPin(Handle, 1);
	return retVal;
}

/***********************************************************************************************************/

uint8_t SPIF_ReadReg3(SPIF_HandleTypeDef *Handle)
{
	uint8_t retVal = 0;
	uint8_t tx[2] = {SPIF_CMD_READSTATUS3, SPIF_DUMMY_BYTE};
	uint8_t rx[2];
	SPIF_CsPin(Handle, 0);
	if (SPIF_TransmitReceive(Handle, tx, rx, 2, 100) == true)
	{
		retVal = rx[1];
	}
	SPIF_CsPin(Handle, 1);
	return retVal;
}

/***********************************************************************************************************/

bool SPIF_WriteReg1(SPIF_HandleTypeDef *Handle, uint8_t Data)
{
	bool retVal = true;
	uint8_t tx[2] = {SPIF_CMD_WRITESTATUS1, Data};
	uint8_t cmd = SPIF_CMD_WRITESTATUSEN;
	do
	{
		SPIF_CsPin(Handle, 0);
		if (SPIF_Transmit(Handle, &cmd, 1, 100) == false)
		{
			retVal = false;
			SPIF_CsPin(Handle, 1);
			break;
		}
		SPIF_CsPin(Handle, 1);
		SPIF_CsPin(Handle, 0);
		if (SPIF_Transmit(Handle, tx, 2, 100) == false)
		{
			retVal = false;
			SPIF_CsPin(Handle, 1);
			break;
		}
		SPIF_CsPin(Handle, 1);
	} while (0);

	return retVal;
}

/***********************************************************************************************************/

bool SPIF_WriteReg2(SPIF_HandleTypeDef *Handle, uint8_t Data)
{
	bool retVal = true;
	uint8_t tx[2] = {SPIF_CMD_WRITESTATUS2, Data};
	uint8_t cmd = SPIF_CMD_WRITESTATUSEN;
	do
	{
		SPIF_CsPin(Handle, 0);
		if (SPIF_Transmit(Handle, &cmd, 1, 100) == false)
		{
			retVal = false;
			SPIF_CsPin(Handle, 1);
			break;
		}
		SPIF_CsPin(Handle, 1);
		SPIF_CsPin(Handle, 0);
		if (SPIF_Transmit(Handle, tx, 2, 100) == false)
		{
			retVal = false;
			SPIF_CsPin(Handle, 1);
			break;
		}
		SPIF_CsPin(Handle, 1);
	} while (0);

	return retVal;
}

/***********************************************************************************************************/

bool SPIF_WriteReg3(SPIF_HandleTypeDef *Handle, uint8_t Data)
{
	bool retVal = true;
	uint8_t tx[2] = {SPIF_CMD_WRITESTATUS3, Data};
	uint8_t cmd = SPIF_CMD_WRITESTATUSEN;
	do
	{
		SPIF_CsPin(Handle, 0);
		if (SPIF_Transmit(Handle, &cmd, 1, 100) == false)
		{
			retVal = false;
			SPIF_CsPin(Handle, 1);
			break;
		}
		SPIF_CsPin(Handle, 1);
		SPIF_CsPin(Handle, 0);
		if (SPIF_Transmit(Handle, tx, 2, 100) == false)
		{
			retVal = false;
			SPIF_CsPin(Handle, 1);
			break;
		}
		SPIF_CsPin(Handle, 1);
	} while (0);

	return retVal;
}

/***********************************************************************************************************/

bool SPIF_WaitForWriting(SPIF_HandleTypeDef *Handle, uint32_t Timeout)
{
	bool retVal = false;
	uint32_t startTime = HAL_GetTick();
	while (1)
	{
		SPIF_Delay(1);
		if (HAL_GetTick() - startTime >= Timeout)
		{
			dprintf("SPIF_WaitForWriting() TIMEOUT\r\n");
			break;
		}
		if ((SPIF_ReadReg1(Handle) & SPIF_STATUS1_BUSY) == 0)
		{
			retVal = true;
			break;
		}
	}
	return retVal;
}

/***********************************************************************************************************/

bool SPIF_FindChip(SPIF_HandleTypeDef *Handle)
{
	uint8_t tx[4] = {SPIF_CMD_JEDECID, 0xFF, 0xFF, 0xFF};
	uint8_t rx[4];
	bool retVal = false;
	do
	{
		SPIF_CsPin(Handle, 0);
		if (SPIF_TransmitReceive(Handle, tx, rx, 4, 100) == false)
		{
			SPIF_CsPin(Handle, 1);
			break;
		}
		SPIF_CsPin(Handle, 1);
		Handle->Manufactor = rx[1];
		Handle->MemType = rx[2];
		Handle->Size = rx[3];

		switch (Handle->Size)
		{
		case SPIF_SIZE_1MBIT:
			Handle->BlockCnt = 2;
			break;
		case SPIF_SIZE_2MBIT:
			Handle->BlockCnt = 4;
			break;
		case SPIF_SIZE_4MBIT:
			Handle->BlockCnt = 8;
			break;
		case SPIF_SIZE_8MBIT:
			Handle->BlockCnt = 16;
			break;
		case SPIF_SIZE_16MBIT:
			Handle->BlockCnt = 32;
			break;
		case SPIF_SIZE_32MBIT:
			Handle->BlockCnt = 64;
			break;
		case SPIF_SIZE_64MBIT:
			Handle->BlockCnt = 128;
			break;
		case SPIF_SIZE_128MBIT:
			Handle->BlockCnt = 256;
			break;
		case SPIF_SIZE_256MBIT:
			Handle->BlockCnt = 512;
			break;
		case SPIF_SIZE_512MBIT:
			Handle->BlockCnt = 1024;
			break;
		default:
			Handle->Size = SPIF_SIZE_ERROR;
			dprintf("ERROR\r\n");
			break;
		}

		Handle->SectorCnt = Handle->BlockCnt * 16;
		Handle->PageCnt = (Handle->SectorCnt * SPIF_SECTOR_SIZE) / SPIF_PAGE_SIZE;
		Handle->PageSize = SPIF_PAGE_SIZE;
		retVal = true;

	} while (0);

	return retVal;
}

/***********************************************************************************************************/

bool SPIF_WriteFn(SPIF_HandleTypeDef *Handle, uint32_t PageNumber, uint8_t *Data, uint32_t Size, uint32_t Offset)
{
	bool retVal = false;
	uint32_t address = 0, maximum = SPIF_PAGE_SIZE - Offset;
	uint8_t tx[5];
	do {
#if SPIF_DEBUG != SPIF_DEBUG_DISABLE
		uint32_t dbgTime = HAL_GetTick();
#endif
		dprintf("SPIF_WritePage() START PAGE %ld\r\n", PageNumber);
		if (PageNumber >= Handle->PageCnt) {
			dprintf("SPIF_WritePage() ERROR PageNumber\r\n");
			break;
		}
		if (Offset >= SPIF_PAGE_SIZE) {
			dprintf("SPIF_WritePage() ERROR Offset\r\n");
			break;
		}
		if (Size > maximum)	{
			Size = maximum;
		}
		address = SPIF_PageToAddress(PageNumber) + Offset;
		if (SPIF_WriteEnable(Handle) == false) {
			break;
		}
		SPIF_CsPin(Handle, 0);
		if (Handle->BlockCnt >= 512) {
			tx[0] = SPIF_CMD_PAGEPROG4ADD;
			tx[1] = (address & 0xFF000000) >> 24;
			tx[2] = (address & 0x00FF0000) >> 16;
			tx[3] = (address & 0x0000FF00) >> 8;
			tx[4] = (address & 0x000000FF);
			if (SPIF_Transmit(Handle, tx, 5, 100) == false)	{
				SPIF_CsPin(Handle, 1);
				break;
			}
		}
		else {
			tx[0] = SPIF_CMD_PAGEPROG3ADD;
			tx[1] = (address & 0x00FF0000) >> 16;
			tx[2] = (address & 0x0000FF00) >> 8;
			tx[3] = (address & 0x000000FF);
			if (SPIF_Transmit(Handle, tx, 4, 100) == false)	{
				SPIF_CsPin(Handle, 1);
				break;
			}
		}
		if (SPIF_Transmit(Handle, Data, Size, 1000) == false) {
			SPIF_CsPin(Handle, 1);
			break;
		}
		SPIF_CsPin(Handle, 1);
		if (SPIF_WaitForWriting(Handle, 100)) {
			dprintf("SPIF_WritePage() %d BYTES WITERN DONE AFTER %ld ms\r\n", (uint16_t)Size, HAL_GetTick() - dbgTime);
			retVal = true;
		}

	} while (0);

	SPIF_WriteDisable(Handle);
	return retVal;
}

/***********************************************************************************************************/

bool SPIF_ReadFn(SPIF_HandleTypeDef *Handle, uint32_t Address, uint8_t *Data, uint32_t Size)
{
	bool retVal = false;
	uint8_t tx[5];
	do
	{
#if SPIF_DEBUG != SPIF_DEBUG_DISABLE
		uint32_t dbgTime = HAL_GetTick();
#endif
		dprintf("SPIF_ReadAddress() START ADDRESS 0x%8lx\r\n", Address);
		SPIF_CsPin(Handle, 0);
		if (Handle->BlockCnt >= 512)
		{
			tx[0] = SPIF_CMD_READDATA4ADD;
			tx[1] = (Address & 0xFF000000) >> 24;
			tx[2] = (Address & 0x00FF0000) >> 16;
			tx[3] = (Address & 0x0000FF00) >> 8;
			tx[4] = (Address & 0x000000FF);
			if (SPIF_Transmit(Handle, tx, 5, 100) == false)
			{
				SPIF_CsPin(Handle, 1);
				break;
			}
		}
		else
		{
			tx[0] = SPIF_CMD_READDATA3ADD;
			tx[1] = (Address & 0x00FF0000) >> 16;
			tx[2] = (Address & 0x0000FF00) >> 8;
			tx[3] = (Address & 0x000000FF);
			if (SPIF_Transmit(Handle, tx, 4, 100) == false)
			{
				SPIF_CsPin(Handle, 1);
				break;
			}
		}
		if (SPIF_Receive(Handle, Data, Size, 2000) == false)
		{
			SPIF_CsPin(Handle, 1);
			break;
		}
		SPIF_CsPin(Handle, 1);
#if SPIF_DEBUG == SPIF_DEBUG_FULL
		uint32_t address = Address;
		dprintf("SPIF_ReadAddress() %d BYTES READ DONE AFTER %ld ms\r\n", (uint16_t)Size, HAL_GetTick() - dbgTime);
		HexDump(address, Data, Size);
		dprintf(LINE_TERM LINE_TERM);
#endif
		retVal = true;

	} while (0);

	return retVal;
}

/***********************************************************************************************************/
/***********************************************************************************************************
typedef struct
{
	SPI_HandleTypeDef      *HSpi;
	GPIO_TypeDef           *Gpio;
	SPIF_ManufactorTypeDef Manufactor;
	SPIF_SizeTypeDef       Size;
	uint8_t                Inited;
	uint8_t                MemType;
	uint8_t                Lock;
	uint8_t                Reserved;
	uint32_t               Pin;
	uint32_t               PageCnt;
	uint32_t               SectorCnt;
	uint32_t               BlockCnt;

} SPIF_HandleTypeDef;
 ***********************************************************************************************************/
bool eflash_ifc_init(void *arg)
{
	assert (SPIF_Init(&spif_handle, &hspi1, SPI1_CS_GPIO_Port, SPI1_CS_Pin) == true);
	return true;
}

bool SPIF_Init(SPIF_HandleTypeDef *Handle, SPI_HandleTypeDef *HSpi, GPIO_TypeDef *Gpio, uint16_t Pin)
{
	bool retVal = false;
	do
	{
		if ((Handle == NULL) || (HSpi == NULL) || (Gpio == NULL) || (Handle->Inited == 1))
		{
			dprintf("SPIF_Init() Error, Wrong Parameter\r\n");
			break;
		}
		memset(Handle, 0, sizeof(SPIF_HandleTypeDef));
		Handle->HSpi = HSpi;
		Handle->Gpio = Gpio;
		Handle->Pin = Pin;
		SPIF_CsPin(Handle, 1);
		/* wait for stable VCC */
		while (HAL_GetTick() < 20)
		{
			SPIF_Delay(1);
		}
		if (SPIF_WriteDisable(Handle) == false)
		{
			break;
		}
		retVal = SPIF_FindChip(Handle);
		if (retVal)
		{
			Handle->Inited = 1;
		}

	} while (0);

	return retVal;
}

/***********************************************************************************************************/

bool SPIF_EraseChip(SPIF_HandleTypeDef *Handle)
{
	SPIF_Lock(Handle);
	bool retVal = false;
	uint8_t tx[1] = {SPIF_CMD_CHIPERASE1};
	do
	{
#if SPIF_DEBUG != SPIF_DEBUG_DISABLE
		uint32_t dbgTime = HAL_GetTick();
#endif
		dprintf("SPIF_EraseChip() START\r\n");
		if (SPIF_WriteEnable(Handle) == false)
		{
			break;
		}
		SPIF_CsPin(Handle, 0);
		if (SPIF_Transmit(Handle, tx, 1, 100) == false)
		{
			SPIF_CsPin(Handle, 1);
			break;
		}
		SPIF_CsPin(Handle, 1);
		if (SPIF_WaitForWriting(Handle, Handle->BlockCnt * 1000))
		{
			dprintf("SPIF_EraseChip() DONE AFTER %ld ms\r\n", HAL_GetTick() - dbgTime);
			retVal = true;
		}

	} while (0);

	SPIF_WriteDisable(Handle);
	SPIF_UnLock(Handle);
	return retVal;
}

/***********************************************************************************************************/
bool SPIF_EraseSector(SPIF_HandleTypeDef *Handle, uint32_t Sector)
{
	SPIF_Lock(Handle);
	bool retVal = false;
	uint32_t address = Sector * SPIF_SECTOR_SIZE;
	uint8_t tx[5];
	do {
#if SPIF_DEBUG != SPIF_DEBUG_DISABLE
		uint32_t dbgTime = HAL_GetTick();
#endif
		dprintf("SPIF_EraseSector() START SECTOR %ld\r\n", Sector);
		if (Sector >= Handle->SectorCnt) {
			dprintf("SPIF_EraseSector() ERROR Sector NUMBER\r\n");
			break;
		}
		if (SPIF_WriteEnable(Handle) == false) {
			break;
		}
		SPIF_CsPin(Handle, 0);
		if (Handle->BlockCnt >= 512) {
			tx[0] = SPIF_CMD_SECTORERASE4ADD;
			tx[1] = (address & 0xFF000000) >> 24;
			tx[2] = (address & 0x00FF0000) >> 16;
			tx[3] = (address & 0x0000FF00) >> 8;
			tx[4] = (address & 0x000000FF);
			if (SPIF_Transmit(Handle, tx, 5, 100) == false)	{
				SPIF_CsPin(Handle, 1);
				break;
			}
		}
		else {
			tx[0] = SPIF_CMD_SECTORERASE3ADD;
			tx[1] = (address & 0x00FF0000) >> 16;
			tx[2] = (address & 0x0000FF00) >> 8;
			tx[3] = (address & 0x000000FF);
			if (SPIF_Transmit(Handle, tx, 4, 100) == false)	{
				SPIF_CsPin(Handle, 1);
				break;
			}
		}
		SPIF_CsPin(Handle, 1);
		if (SPIF_WaitForWriting(Handle, 1000)) {
			dprintf("SPIF_EraseSector() DONE AFTER %ld ms\r\n", HAL_GetTick() - dbgTime);
			retVal = true;
		}
	} while (0);

	SPIF_WriteDisable(Handle);
	SPIF_UnLock(Handle);
	return retVal;
}

/***********************************************************************************************************/

bool SPIF_EraseBlock(SPIF_HandleTypeDef *Handle, uint32_t Block)
{
	SPIF_Lock(Handle);
	bool retVal = false;
	uint32_t address = Block * SPIF_BLOCK_SIZE;
	uint8_t tx[5];
	do {
#if SPIF_DEBUG != SPIF_DEBUG_DISABLE
		uint32_t dbgTime = HAL_GetTick();
#endif
		dprintf("SPIF_EraseBlock() START PAGE %ld\r\n", Block);
		if (Block >= Handle->BlockCnt) {
			dprintf("SPIF_EraseBlock() ERROR Block NUMBER\r\n");
			break;
		}

		if (SPIF_WriteEnable(Handle) == false) {
			break;
		}

		SPIF_CsPin(Handle, 0);
		if (Handle->BlockCnt >= 512) {
			tx[0] = SPIF_CMD_BLOCKERASE4ADD;
			tx[1] = (address & 0xFF000000) >> 24;
			tx[2] = (address & 0x00FF0000) >> 16;
			tx[3] = (address & 0x0000FF00) >> 8;
			tx[4] = (address & 0x000000FF);
			if (SPIF_Transmit(Handle, tx, 5, 100) == false)	{
				SPIF_CsPin(Handle, 1);
				break;
			}
		}
		else {
			tx[0] = SPIF_CMD_BLOCKERASE3ADD;
			tx[1] = (address & 0x00FF0000) >> 16;
			tx[2] = (address & 0x0000FF00) >> 8;
			tx[3] = (address & 0x000000FF);
			if (SPIF_Transmit(Handle, tx, 4, 100) == false)	{
				SPIF_CsPin(Handle, 1);
				break;
			}
		}
		SPIF_CsPin(Handle, 1);
		if (SPIF_WaitForWriting(Handle, 3000)) {
			dprintf("SPIF_EraseBlock() DONE AFTER %ld ms\r\n", HAL_GetTick() - dbgTime);
			retVal = true;
		}

	} while (0);

	SPIF_WriteDisable(Handle);
	SPIF_UnLock(Handle);
	return retVal;
}

/***********************************************************************************************************/

bool SPIF_WriteAddress(SPIF_HandleTypeDef *Handle, uint32_t Address, uint8_t *Data, uint32_t Size)
{
	SPIF_Lock(Handle);
	bool retVal = false;
	uint32_t page, add, offset, remaining, length, index = 0;
	add = Address;
	remaining = Size;
	do {
		page = SPIF_AddressToPage(add);
		offset = add % SPIF_PAGE_SIZE;
		if (remaining <= SPIF_PAGE_SIZE) {
			length = remaining;
		}
		else {
			length = SPIF_PAGE_SIZE - offset;
		}
		if (SPIF_WriteFn(Handle, page, &Data[index], length, offset) == false) {
			break;
		}
		add += length;
		index += length;
		remaining -= length;
		if (remaining == 0) {
			retVal = true;
			break;
		}

	} while (remaining > 0);

	SPIF_UnLock(Handle);
	return retVal;
}

/***********************************************************************************************************/

bool SPIF_WritePage(SPIF_HandleTypeDef *Handle, uint32_t PageNumber, uint8_t *Data, uint32_t Size, uint32_t Offset)
{
	SPIF_Lock(Handle);
	bool retVal = false;
	retVal = SPIF_WriteFn(Handle, PageNumber, Data, Size, Offset);
	SPIF_UnLock(Handle);
	return retVal;
}

/***********************************************************************************************************/

bool SPIF_WriteSector(SPIF_HandleTypeDef *Handle, uint32_t SectorNumber, uint8_t *Data, uint32_t Size, uint32_t Offset)
{
	SPIF_Lock(Handle);
	bool retVal = true;
	do
	{
		if (Offset >= SPIF_SECTOR_SIZE)
		{
			retVal = false;
			break;
		}
		if (Size > (SPIF_SECTOR_SIZE - Offset))
		{
			Size = SPIF_SECTOR_SIZE - Offset;
		}
		uint32_t bytesWritten = 0;
		uint32_t pageNumber = SectorNumber * (SPIF_SECTOR_SIZE / SPIF_PAGE_SIZE);
		pageNumber += Offset / SPIF_PAGE_SIZE;
		uint32_t remainingBytes = Size;
		uint32_t pageOffset = Offset % SPIF_PAGE_SIZE;
		while (remainingBytes > 0 && pageNumber < ((SectorNumber + 1) * (SPIF_SECTOR_SIZE / SPIF_PAGE_SIZE)))
		{
			uint32_t bytesToWrite = (remainingBytes > SPIF_PAGE_SIZE) ? SPIF_PAGE_SIZE : remainingBytes;
			if (SPIF_WriteFn(Handle, pageNumber, Data + bytesWritten, bytesToWrite, pageOffset) == false)
			{
				retVal = false;
				break;
			}
			bytesWritten += bytesToWrite;
			remainingBytes -= bytesToWrite;
			pageNumber++;
			pageOffset = 0;
		}
	} while (0);
	SPIF_UnLock(Handle);
	return retVal;
}

/***********************************************************************************************************/

bool SPIF_WriteBlock(SPIF_HandleTypeDef *Handle, uint32_t BlockNumber, uint8_t *Data, uint32_t Size, uint32_t Offset)
{
	SPIF_Lock(Handle);
	bool retVal = true;
	do
	{
		if (Offset >= SPIF_BLOCK_SIZE)
		{
			retVal = false;
			break;
		}
		if (Size > (SPIF_BLOCK_SIZE - Offset))
		{
			Size = SPIF_BLOCK_SIZE - Offset;
		}
		uint32_t bytesWritten = 0;
		uint32_t pageNumber = BlockNumber * (SPIF_BLOCK_SIZE / SPIF_PAGE_SIZE);
		pageNumber += Offset / SPIF_PAGE_SIZE;
		uint32_t remainingBytes = Size;
		uint32_t pageOffset = Offset % SPIF_PAGE_SIZE;
		while (remainingBytes > 0 && pageNumber < ((BlockNumber + 1) * (SPIF_BLOCK_SIZE / SPIF_PAGE_SIZE)))
		{
			uint32_t bytesToWrite = (remainingBytes > SPIF_PAGE_SIZE) ? SPIF_PAGE_SIZE : remainingBytes;
			if (SPIF_WriteFn(Handle, pageNumber, Data + bytesWritten, bytesToWrite, pageOffset) == false)
			{
				retVal = false;
				break;
			}
			bytesWritten += bytesToWrite;
			remainingBytes -= bytesToWrite;
			pageNumber++;
			pageOffset = 0;
		}

	} while (0);

	SPIF_UnLock(Handle);
	return retVal;
}

/***********************************************************************************************************/

bool SPIF_ReadAddress(SPIF_HandleTypeDef *Handle, uint32_t Address, uint8_t *Data, uint32_t Size)
{
	SPIF_Lock(Handle);
	bool retVal = false;
	retVal = SPIF_ReadFn(Handle, Address, Data, Size);
	SPIF_UnLock(Handle);
	return retVal;
}

/***********************************************************************************************************/

bool SPIF_ReadPage(SPIF_HandleTypeDef *Handle, uint32_t PageNumber, uint8_t *Data, uint32_t Size, uint32_t Offset)
{
	SPIF_Lock(Handle);
	bool retVal = false;
	uint32_t address = SPIF_PageToAddress(PageNumber);
	uint32_t maximum = SPIF_PAGE_SIZE - Offset;
	if (Size > maximum)
	{
		Size = maximum;
	}
	retVal = SPIF_ReadFn(Handle, address, Data, Size);
	SPIF_UnLock(Handle);
	return retVal;
}

/***********************************************************************************************************/

bool SPIF_ReadSector(SPIF_HandleTypeDef *Handle, uint32_t SectorNumber, uint8_t *Data, uint32_t Size, uint32_t Offset)
{
	SPIF_Lock(Handle);
	bool retVal = false;
	uint32_t address = SPIF_SectorToAddress(SectorNumber);
	uint32_t maximum = SPIF_SECTOR_SIZE - Offset;
	if (Size > maximum)
	{
		Size = maximum;
	}
	retVal = SPIF_ReadFn(Handle, address, Data, Size);
	SPIF_UnLock(Handle);
	return retVal;
}

