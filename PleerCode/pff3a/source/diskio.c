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

	// Put your code here

	uint8_t version;

	version=SD_Initialize();

	if ( (version == VER1_SDSC) || (version == VER2_SDSC) || (version == VER2_SDHC_SDXC) )
		stat=STA_INIT_SUCCESS;
	else
		stat=STA_NOINIT;		

	return stat;             //если stat=STA_NOINIT=0x01,то произошёл сбой инициализации и функция  pf_mount() выдаст return FR_NOT_READY, 
							 //при любом другом значении инициализация для функции pf_mount() считается успешной
							 //сделаем след. образом: если инициализация действительно прошла как надо, то stat=STA_INIT_SUCCESS=0
							 //при возникновении какой-либо ошибки с инициализацией (т.е. если версия карты не определилась)
							 //stat=STA_NOINIT
}







/*-----------------------------------------------------------------------*/
/* Read Partial Sector                                                   */
/*-----------------------------------------------------------------------*/

DRESULT disk_readp (
	BYTE* buff,		/* Pointer to the destination object */
	DWORD sector,	/* Sector number (LBA) */ //в библиотеке также поддерживается обрадотка физического адреса, 
											  //именно поэтому я не использую смещение до LBA
											  //библиотека сама определяет по MBR(0 сектор) расположение boot record(BRB) 
											  //и в последствии добавляет это смещение
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

	/*buff[count];
  	BYTE ReadStatus=1;

	uint16_t i=0;


	if ((offset+count)>512)
		{
			return RES_PARERR;
		}

	//ReadStatus=SD_ReadBlock(buffer,sector+8192, 512);
            uint8_t startDataToken1=0;
        
            SD_SendCmd(17,sector,512);   // команду CMD17 в качестве аргумента передаётся физический адрес сектора
            								  // причём сама библиотека оперирует логическим адресом 
            								  //начало логического блока (logical block address LBA) у карт отличается
            SD_WriteByte(SD_DUMMY_BYTE);
            ReadStatus=SD_ReadByte();
            do
              startDataToken1=SD_ReadByte();  //start Data Token=11111110
            while(startDataToken1!=0xFE);

            for (i=0;i<512;i++)
            {
            	if ( (i >= offset) && (i < offset+count) )
            		buff[i-offset]=SD_ReadByte();
            	else
            		SD_ReadByte();
            }

            //for(i=0;i<512;i++)   //сначала передаются старшие байты
            //buffer[i]=SD_ReadByte();

            SD_ReadByte();
            SD_ReadByte();
        
        
	if (ReadStatus==SD_RESPONSE_NO_ERROR)
		res=RES_OK;
	else
		return RES_NOTRDY;
		

	// Put your code here

	return res;*/
}



/*-----------------------------------------------------------------------*/
/* Write Partial Sector                                                  */
/*-----------------------------------------------------------------------*/
/*
DRESULT disk_writep (
	BYTE* buff,		 //Pointer to the data to be written, NULL:Initiate/Finalize write operation 
	DWORD sc		 //Sector number (LBA) or Number of bytes to send 
)
{
	DRESULT res;


	if (!buff) {
		if (sc) {

			// Initiate write process

		} else {

			// Finalize write process

		}
	} else {

		// Send data to the disk

	}

	return res;
}
*/
