/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for Petit FatFs (C)ChaN, 2014      */
/*-----------------------------------------------------------------------*/

#include "diskio.h"


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (void)
{
	DSTATUS stat;

	uint8_t version;

	// СИСТЕМНОЕ ТАКТИРОВАНИЕ
	// при изменении системной частоты необходимо изменить коэффициенты тактирования TIM4, TIM1, SPI
    CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1); // основное тактирование без делителя, т.е. на 16 Мгц, 
	
    Delay(1000); 
	version=SD_Initialize();

	if ( (version == VER1_SDSC) || (version == VER2_SDSC) || (version == VER2_SDHC_SDXC) )
		stat=STA_INIT_SUCCESS;
	else
		stat=STA_NOINIT;	
	//если stat=STA_NOINIT=0x01,то произошёл сбой инициализации и функция  pf_mount() выдаст return FR_NOT_READY, 
	//при любом другом значении инициализация для функции pf_mount() считается успешной
	//сделаем след. образом: если инициализация действительно прошла как надо, то stat=STA_INIT_SUCCESS=0
	//при возникновении какой-либо ошибки с инициализацией (т.е. если версия карты не определилась)
	//stat=STA_NOINIT
	return stat;             
}



/*-----------------------------------------------------------------------*/
/* Read Partial Sector                                                   */
/*-----------------------------------------------------------------------*/

DRESULT disk_readp (
	BYTE* buff,		/* Pointer to the destination object */
	DWORD sector,	/* Sector number (LBA) */
	UINT offset,	/* Offset in the sector */
	UINT count		/* Byte count (bit15:destination) */
)
{
	DRESULT res;

	if ((offset+count)>512)
		{
			return RES_PARERR;
		};

	if (SD_ReadBlock_PFF(buff, sector, offset, count ))
		return RES_ERROR;
	else
		return RES_OK;
}


