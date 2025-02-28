/*-----------------------------------------------------------------------
/  PFF - Low level disk interface modlue include file    (C)ChaN, 2014
/-----------------------------------------------------------------------*/

#ifndef _DISKIO_DEFINED
#define _DISKIO_DEFINED

#ifdef __cplusplus
extern "C" {
#endif

#include "pff.h"
#include "stm8l15x.h"
#include "stm8l15x_syscfg.h"
#include "stm8_eval_spi_sd.h"

/* Status of Disk Functions */
typedef BYTE	DSTATUS;


/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,		/* 0: Function succeeded */
	RES_ERROR,		/* 1: Disk error */
	RES_NOTRDY,		/* 2: Not ready */
	RES_PARERR		/* 3: Invalid parameter */
} DRESULT;


/*---------------------------------------*/
/* Prototypes for disk control functions */

DSTATUS disk_initialize (void);
DRESULT disk_readp (BYTE* buff, DWORD sector, UINT offser, UINT count);
DRESULT disk_writep (const BYTE* buff, DWORD sc);

#define STA_INIT_SUCCESS 	0x00    // инициализация прошла успешно
#define STA_NOINIT			0x01	/* Drive not initialized */
#define STA_NODISK			0x02	/* No medium in the drive */ //означает, что в разьёме нет карты, данный флаг не используется в Petit FAT 

#ifdef __cplusplus
}
#endif

#endif	/* _DISKIO_DEFINED */
