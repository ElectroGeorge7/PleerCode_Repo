/**
  ******************************************************************************
  * @file    stm8_eval_spi_sd.h
  * @author  MCD Application Team
  * @version V2.1.3
  * @date    28-June-2013
  * @brief   This file contains all the functions prototypes for the stm8_eval_spi_sd
  *          firmware driver.
  ******************************************************************************
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM8_EVAL_SPI_SD_H
#define __STM8_EVAL_SPI_SD_H

/* Includes ------------------------------------------------------------------*/
#include "stm8_eval.h"

/** @addtogroup Utilities
  * @{
  */
  
/** @addtogroup STM8_EVAL
  * @{
  */ 

/** @addtogroup Common
  * @{
  */
  
/** @addtogroup STM8_EVAL_SPI_SD
  * @{
  */  

/** @defgroup STM8_EVAL_SPI_SD_Exported_Types
  * @{
  */ 

/** 
  * @brief  Card Specific Data: CSD Register   
  */ 
typedef struct
{
  __IO uint8_t  CSDStruct;            /*!< CSD structure */
  __IO uint8_t  SysSpecVersion;       /*!< System specification version */
  __IO uint8_t  Reserved1;            /*!< Reserved */
  __IO uint8_t  TAAC;                 /*!< Data read access-time 1 */
  __IO uint8_t  NSAC;                 /*!< Data read access-time 2 in CLK cycles */
  __IO uint8_t  MaxBusClkFrec;        /*!< Max. bus clock frequency */
  __IO uint16_t CardComdClasses;      /*!< Card command classes */
  __IO uint8_t  RdBlockLen;           /*!< Max. read data block length */
  __IO uint8_t  PartBlockRead;        /*!< Partial blocks for read allowed */
  __IO uint8_t  WrBlockMisalign;      /*!< Write block misalignment */
  __IO uint8_t  RdBlockMisalign;      /*!< Read block misalignment */
  __IO uint8_t  DSRImpl;              /*!< DSR implemented */
  __IO uint8_t  Reserved2;            /*!< Reserved */
  __IO uint32_t DeviceSize;           /*!< Device Size */
  __IO uint8_t  MaxRdCurrentVDDMin;   /*!< Max. read current @ VDD min */
  __IO uint8_t  MaxRdCurrentVDDMax;   /*!< Max. read current @ VDD max */
  __IO uint8_t  MaxWrCurrentVDDMin;   /*!< Max. write current @ VDD min */
  __IO uint8_t  MaxWrCurrentVDDMax;   /*!< Max. write current @ VDD max */
  __IO uint8_t  DeviceSizeMul;        /*!< Device size multiplier */
  __IO uint8_t  EraseGrSize;          /*!< Erase group size */
  __IO uint8_t  EraseGrMul;           /*!< Erase group size multiplier */
  __IO uint8_t  WrProtectGrSize;      /*!< Write protect group size */
  __IO uint8_t  WrProtectGrEnable;    /*!< Write protect group enable */
  __IO uint8_t  ManDeflECC;           /*!< Manufacturer default ECC */
  __IO uint8_t  WrSpeedFact;          /*!< Write speed factor */
  __IO uint8_t  MaxWrBlockLen;        /*!< Max. write data block length */
  __IO uint8_t  WriteBlockPaPartial;  /*!< Partial blocks for write allowed */
  __IO uint8_t  Reserved3;            /*!< Reserded */
  __IO uint8_t  ContentProtectAppli;  /*!< Content protection application */
  __IO uint8_t  FileFormatGrouop;     /*!< File format group */
  __IO uint8_t  CopyFlag;             /*!< Copy flag (OTP) */
  __IO uint8_t  PermWrProtect;        /*!< Permanent write protection */
  __IO uint8_t  TempWrProtect;        /*!< Temporary write protection */
  __IO uint8_t  FileFormat;           /*!< File Format */
  __IO uint8_t  ECC;                  /*!< ECC code */
  __IO uint8_t  CSD_CRC;              /*!< CSD CRC */
  __IO uint8_t  Reserved4;            /*!< always 1*/
} SD_CSD;

/** 
  * @brief  Card Identification Data: CID Register   
  */
typedef struct
{
  __IO uint8_t  ManufacturerID;       /*!< ManufacturerID */
  __IO uint16_t OEM_AppliID;          /*!< OEM/Application ID */
  __IO uint32_t ProdName1;            /*!< Product Name part1 */
  __IO uint8_t  ProdName2;            /*!< Product Name part2*/
  __IO uint8_t  ProdRev;              /*!< Product Revision */
  __IO uint32_t ProdSN;               /*!< Product Serial Number */
  __IO uint8_t  Reserved1;            /*!< Reserved1 */
  __IO uint16_t ManufactDate;         /*!< Manufacturing Date */
  __IO uint8_t  CID_CRC;              /*!< CID CRC */
  __IO uint8_t  Reserved2;            /*!< always 1 */
} SD_CID;

/** 
  * @brief SD Card information 
  */
typedef struct
{
  SD_CSD SD_csd;
  SD_CID SD_cid;
  uint32_t CardCapacity;  /*!< Card Capacity */
  uint32_t CardBlockSize; /*!< Card Block Size */
} SD_CardInfo;

typedef enum
{
  UNDEFINED_SD = ((uint8_t)0x00),
  VER1_SDSC = ((uint8_t)0x01),
  VER2_SDSC = ((uint8_t)0x02),
  VER2_SDHC_SDXC = ((uint8_t)0x03),
  UNUSABLE_CARD = ((uint8_t)0x04)
} SD_Version;


/**
  * @}
  */
  
/** @defgroup STM8_EVAL_SPI_SD_Exported_Constants
  * @{
  */ 

/**
  * @brief  SD reponses and error flags
  */

#define  SD_RESPONSE_NO_ERROR         0x00
#define  SD_IN_READY_STATE            0x00
#define  SD_IN_IDLE_STATE             0x01
#define  SD_ERASE_RESET               0x02
#define  SD_ILLEGAL_COMMAND           0x04
#define  SD_COM_CRC_ERROR             0x08
#define  SD_ERASE_SEQUENCE_ERROR      0x10
#define  SD_ADDRESS_ERROR             0x20
#define  SD_PARAMETER_ERROR           0x40
#define  SD_RESPONSE_FAILURE          0xFF

/**
  * @brief  Data response error
  */
#define  SD_DATA_OK                   0x05
#define  SD_DATA_CRC_ERROR            0x0B
#define  SD_DATA_WRITE_ERROR          0x0D
#define  SD_DATA_OTHER_ERROR          0xFF
    
/**
  * @brief  Block Size
  */
#define SD_BLOCK_SIZE    0x200

/**
  * @brief  Dummy byte
  */
#define SD_DUMMY_BYTE   0xFF

#define SD_RESPONSE_CHECKS_COUNT 70 //количество проверок ответа SDMC на команду
                                    //было 25, но при этом функция некорректно работала с принятием Data token start byte, Start Single Block Read
                                    //потому что необходимо подождать некоторое время пока карта обработает запрос и выдаст ответ
                                    //на низких частотах хватало 25 проверок (за счёт бОльшего времени передачи данных), 
                                    //на высоких необходимо повысить количесвто, на макс частоте 8МГц не менее 67(определил опытным путём)

/**
  * @brief  Start Data tokens:
  *         Tokens (necessary because at nop/idle (and CS active) only 0xff is 
  *         on the data/command line)  
  */
#define SD_START_DATA_SINGLE_BLOCK_READ    0xFE  /*!< Data token start byte, Start Single Block Read */
#define SD_START_DATA_MULTIPLE_BLOCK_READ  0xFE  /*!< Data token start byte, Start Multiple Block Read */
#define SD_START_DATA_SINGLE_BLOCK_WRITE   0xFE  /*!< Data token start byte, Start Single Block Write */
#define SD_START_DATA_MULTIPLE_BLOCK_WRITE 0xFD  /*!< Data token start byte, Start Multiple Block Write */
#define SD_STOP_DATA_MULTIPLE_BLOCK_WRITE  0xFD  /*!< Data toke stop byte, Stop Multiple Block Write */

/**
  * @brief  SD detection on its memory slot
  */
#define SD_PRESENT        ((uint8_t)0x01)
#define SD_NOT_PRESENT    ((uint8_t)0x00)


/**
  * @brief  Commands: CMDxx = CMD-number | 0x40
  */
#define SD_CMD_GO_IDLE_STATE          0   /*!< CMD0 = 0x40 */
#define SD_CMD_SEND_OP_COND           1   /*!< CMD1 = 0x41 */
#define SD_CMD_SEND_IF_COND           8   /*!< CMD8 = 0x48 */
#define SD_CMD_SEND_CSD               9   /*!< CMD9 = 0x49 */
#define SD_CMD_SEND_CID               10  /*!< CMD10 = 0x4A */
#define SD_CMD_STOP_TRANSMISSION      12  /*!< CMD12 = 0x4C */
#define SD_CMD_SEND_STATUS            13  /*!< CMD13 = 0x4D */
#define SD_CMD_SET_BLOCKLEN           16  /*!< CMD16 = 0x50 */
#define SD_CMD_READ_SINGLE_BLOCK      17  /*!< CMD17 = 0x51 */
#define SD_CMD_READ_MULT_BLOCK        18  /*!< CMD18 = 0x52 */
#define SD_CMD_SET_BLOCK_COUNT        23  /*!< CMD23 = 0x57 */
#define SD_CMD_WRITE_SINGLE_BLOCK     24  /*!< CMD24 = 0x58 */
#define SD_CMD_WRITE_MULT_BLOCK       25  /*!< CMD25 = 0x59 */
#define SD_CMD_PROG_CSD               27  /*!< CMD27 = 0x5B */
#define SD_CMD_SET_WRITE_PROT         28  /*!< CMD28 = 0x5C */
#define SD_CMD_CLR_WRITE_PROT         29  /*!< CMD29 = 0x5D */
#define SD_CMD_SEND_WRITE_PROT        30  /*!< CMD30 = 0x5E */
#define SD_CMD_SD_ERASE_GRP_START     32  /*!< CMD32 = 0x60 */
#define SD_CMD_SD_ERASE_GRP_END       33  /*!< CMD33 = 0x61 */
#define SD_CMD_UNTAG_SECTOR           34  /*!< CMD34 = 0x62 */
#define SD_CMD_ERASE_GRP_START        35  /*!< CMD35 = 0x63 */
#define SD_CMD_ERASE_GRP_END          36  /*!< CMD36 = 0x64 */
#define SD_CMD_UNTAG_ERASE_GROUP      37  /*!< CMD37 = 0x65 */
#define SD_CMD_ERASE                  38  /*!< CMD38 = 0x66 */
#define SD_CMD_APP_CMD                55  /*!< CMD55 = 0x77 */
#define SD_CMD_READ_OCR               58  /*!< CMD58 = 0x7A */
#define SD_ACMD_SD_SEND_OP_COND       41  /*!< ACMD41 */



//аргумент команды ACMD41
#define ACMD41_ARG        ((uint32_t)( 0x40FF8000 ))    //(1<<30) | (0x1FF<<15) Host Capacity Support(HCS) | voltage window field (bit 23-0) of OCR раздел (4.2.3.1)


/**
  * @}
  */ 
  
/** @defgroup STM8_EVAL_SPI_SD_Exported_Macros
  * @{
  */
/** 
  * @brief  Select SD Card: ChipSelect pin low   
  */  
#define SD_CS_LOW()     GPIO_ResetBits(SD_CS_GPIO_PORT, SD_CS_PIN)
/** 
  * @brief  Deselect SD Card: ChipSelect pin high   
  */ 
#define SD_CS_HIGH()    GPIO_SetBits(SD_CS_GPIO_PORT, SD_CS_PIN)
/**
  * @}
  */ 

/** @defgroup STM8_EVAL_SPI_SD_Exported_Functions
  * @{
  */ 
void SD_DeInit(void);                                                                       //не проверял

//uint8_t SD_Init(void);                                                                    //функция не работает, причём инициализация не соответствует стандарту,  
                                                                                            //здесь для инициализации используется CMD1, а в стандарте указана команда 
                                                                                            // ACMD1. CMD1 отдельно тоже работает, но не со всеми картами

uint8_t SD_Initialize(void);                                                                //моя функция инициализации    

uint8_t SD_Detect(void);
uint8_t SD_GetCardInfo(SD_CardInfo *cardinfo);
uint8_t SD_ReadBlock(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t BlockSize);              //эта функция работает, почему то при высоких частотах функция передачи данных функция не работала

uint8_t SD_ReadBlock_PFF(uint8_t* pBuffer, uint32_t ReadAddr,  uint16_t offset, uint16_t count); //эта функция написана мной для работы с Petit FAT

uint8_t SD_ReadBuffer(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead);         //насчёт работоспособности не знаю, но в Petit FAT она не нужна
uint8_t SD_WriteBlock(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t BlockSize);            //эта функция работает
uint8_t SD_WriteBuffer(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite);      //насчёт работоспособности не знаю, но в Petit FAT она не нужна
uint8_t SD_GetCSDRegister(SD_CSD* SD_csd);                                                  //надо  проверить
uint8_t SD_GetCIDRegister(SD_CID* SD_cid);                                                  //надо проверить

void SD_SendCmd(uint8_t Cmd, uint32_t Arg, uint8_t Crc);                                    //эта функция работает

uint8_t SD_GetResponse(uint8_t Response);                                                   //не уверен, ну по идее должна, кроме чтения байта по SPI 25 раз, в ней ничего нет
uint8_t SD_GetResponse1(void);                                                              //мной написанная функция, которая выводит ошибку R1
uint8_t SD_GetDataResponse(void);                                                           //не уверен

uint8_t SD_GoIdleState(void);                                                               //нет
uint16_t SD_GetStatus(void);                                                                //не знаю

uint8_t SD_WriteByte(uint8_t byte);                                                         //работает, надо учесть, что после прочтения функция также читает байт, 
                                                                                            //это нужно для сбрасывания флага SPI_FLAG_RXNE
uint8_t SD_ReadByte(void);                                                                  //работает, надо учесть, что вначале посылатся байт 0xFF(только высокий уровень)
                                                                                            //остальные значения могут не работать на некоторых SD  картах) и вместе с байтом
                                                                                            //отправляются тактовые импульсы, затем происходит чтение байта из буфера


#endif /* __STM8_EVAL_SPI_SD_H */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */ 

/**
  * @}
  */    

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/