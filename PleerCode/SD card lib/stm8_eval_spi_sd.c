/**
  ******************************************************************************
  * @file    stm8_eval_spi_sd.c
  * @author  MCD Application Team
  * @version V2.1.3
  * @date    28-June-2013
  * @brief   This file provides a set of functions needed to manage the SPI SD
  *          Card memory mounted on STM8xx-EVAL board (refer to stm8_eval.h
  *          to know about the boards supporting this memory).
  *          It implements a high level communication layer for read and write
  *          from/to this memory. The needed STM8 hardware resources (SPI and
  *          GPIO) are defined in stm8xx_eval.h file, and the initialization is
  *          performed in SD_LowLevel_Init() function declared in stm8xx_eval.c
  *          file.
  *          You can easily tailor this driver to any other development board,
  *          by just adapting the defines for hardware resources and
  *          SD_LowLevel_Init() function.
  *
  *          +-------------------------------------------------------+
  *          |                     Pin assignment                    |
  *          +-------------------------+---------------+-------------+
  *          |  STM8 SPI Pins         |     SD        |    Pin      |
  *          +-------------------------+---------------+-------------+
  *          | SD_SPI_CS_PIN           |   ChipSelect  |    1        |
  *          | SD_SPI_MOSI_PIN / MOSI  |   DataIn      |    2        |
  *          |                         |   GND         |    3 (0 V)  |
  *          |                         |   VDD         |    4 (3.3 V)|
  *          | SD_SPI_SCK_PIN / SCLK   |   Clock       |    5        |
  *          |                         |   GND         |    6 (0 V)  |
  *          | SD_SPI_MISO_PIN / MISO  |   DataOut     |    7        |
  *          +-------------------------+---------------+-------------+
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

/* Includes ------------------------------------------------------------------*/
#include "stm8_eval_spi_sd.h"
#include "stm8l15x_spi.h"


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
  * @brief      This file includes the SD card driver of STM8-EVAL boards.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup STM8_EVAL_SPI_SD_Private_Functions
  * @{
  */

/**
  * @brief  DeInitializes the SD/SD communication.
  * @param  None
  * @retval None
  */
void SD_DeInit(void)
{
  SD_LowLevel_DeInit();
}

/**
  * @brief  Initializes the SD/SD communication.
  * @param  None
  * @retval The SD Response:
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */

 /*
uint8_t SD_Init(void)
{
  uint32_t i = 0;

  //!< Initialize SD_SPI 
  SD_LowLevel_Init();

  //!< SD chip select high 
  SD_CS_LOW();

  //!< Send dummy byte 0xFF, 10 times with CS high 
  //!< Rise CS and MOSI for 80 clocks cycles 
  for (i = 0; i <= 9; i++)
  {
    //!< Send dummy byte 0xFF 
    SD_WriteByte(SD_DUMMY_BYTE);
  }
  //------------Put SD in SPI mode--------------
  //!< SD initialized and set to SPI mode properly 
  return (SD_GoIdleState());
}
*/

uint8_t SD_Initialize(void)
{
  uint8_t i=0;
  uint8_t resp1=1;
  uint8_t Count=3;
  SD_Version version;


  SD_SPI_Init(); //инициализация SPI модуля и установка режимов пинов, включая CS 
  //Инициализация должна проходить на частоте от 100 кГц до 400кГц

  //Idle State

  //посылаются быйты 0xFF для отправки мимнимум 74 тактовых сигналов, как я понял, нужно для того, чтобы SD карта настроилась на частоту
  for (i = 0; i < 13; i++)  // минимум 74 такта, здесь посылается 8 бит*12 =96 тактов, чтоб наверняка
  {
    SD_WriteByte(SD_DUMMY_BYTE); 
  };

  //посылаем команду CMD0 (CRC=0x95 всегда, так все значени байтов известны)
   SD_SendCmd(SD_CMD_GO_IDLE_STATE, 0, 0x95);
    SD_WriteByte(SD_DUMMY_BYTE); //NCR byte for command response time, после команды обязательно минимум один раз нужно отправить 0xFF, 
                                 //чтобы сгенерировать 8 тактовых импульсов на SCK для обработки полученной команды SD картой
         
  if (SD_GetResponse(SD_IN_IDLE_STATE) == SD_RESPONSE_FAILURE)  //если ответ не возвращает значение 0x01, то выходим из функции с ошибкой SD_RESPONSE_FAILURE
    {
      /*!< No Idle State Response: return response failue */
    return SD_RESPONSE_FAILURE;
    };

  //Card goes into SPI Operation Mode
  
  //CMD8 проверяет поддержку диапазона питания 
  SD_SendCmd(SD_CMD_SEND_IF_COND, 0x01AA, 0x87); //в аргументе: 20 битов зарезервированы, 4 бита -0b0001- означают VHS(voltage host supply), 8 бит-0b10101010 - проверочный шаблон (Check Pattern)
  SD_WriteByte(SD_DUMMY_BYTE);
  //CMD8 возвращает ответ R7, в который входит 5 байт 
  resp1= SD_GetResponse1();      
  if ( resp1 == SD_IN_IDLE_STATE )   //5ый байт
  {
   SD_ReadByte();                   //4ый и 3ий байты с Reserved Bits, просто игнорим
   SD_ReadByte();
   if ( SD_ReadByte()==0 )             //2ой байт содержит VCA, если он равен 0, то карта не поддерживает напряжение в VHS, в итоге карта не подходит
    return UNUSABLE_CARD;
   SD_ReadByte();                    //содержит CheckPattern, игнорим
  }else
    if ( resp1 == SD_ILLEGAL_COMMAND )
      version=VER1_SDSC;
   
   //Initialization 
   //на некоторых сайтах пишут, что можно использовать команду CMD1 для инициализации, но я буду делать по спецификации
   do{                                          
       //CMD55
       SD_SendCmd(SD_CMD_APP_CMD,0,0x01);         //аргумент и CRC не имеют значения, последний бит(end bit) должен быть равен 1 (7.3.1.1)
       SD_WriteByte(SD_DUMMY_BYTE);
       SD_ReadByte();                         //ответ R1, здесь он пока не интересует
                                   
      //ACMD41   
       SD_SendCmd(SD_ACMD_SD_SEND_OP_COND, ACMD41_ARG , 0x01);  //нулевой бит должен быть равен 1 по спецификации (4.2.3.1)
       SD_WriteByte(SD_DUMMY_BYTE);
       resp1=SD_GetResponse(SD_IN_READY_STATE);

       if ( resp1 == SD_RESPONSE_NO_ERROR )
          break;
       else
         if ( Count==0 && (resp1 == SD_RESPONSE_FAILURE) )       
            return  SD_RESPONSE_FAILURE;  

   } while(Count--);
   
   Count=7;

   do{
  //CMD58
   SD_SendCmd(SD_CMD_READ_OCR,0,0x01);
   SD_WriteByte(SD_DUMMY_BYTE);
   if ( SD_GetResponse(SD_IN_READY_STATE) == SD_RESPONSE_FAILURE )
      return  SD_RESPONSE_FAILURE;

   resp1=SD_ReadByte();
   if (resp1 & 0x80){    //проверка Card power up status bit (busy bit)
      Count=0;
      if (version != VER1_SDSC)    //если версия уже была присвоена, то она и отсаётся
        if (resp1 & 0x40)          //проверка Card Capacity Status bit
          version=VER2_SDHC_SDXC;
        else
          version=VER2_SDSC;
    }else
      if (Count==0 && !(resp1 & 0x80))
        return UNUSABLE_CARD;

   SD_ReadByte();     //это зарезервированные байты плюс значения поддерживаемых напряжений, просто все игнорим
   SD_ReadByte();
   SD_ReadByte();

  } while(Count--);

  SD_SPI_Init_HighFreq(); 
   
   return version;
}



/**
 * @brief  Detect if SD card is correctly plugged in the memory slot.
 * @param  None
 * @retval Return if SD is detected or not
 */
uint8_t SD_Detect(void)
{
  __IO uint8_t status = SD_PRESENT;

  /*!< Check GPIO to detect SD */
  if (GPIO_ReadInputData(SD_DETECT_GPIO_PORT) & SD_DETECT_PIN)
  {
    status = SD_NOT_PRESENT;
  }
  return status;
}

/**
  * @brief  Returns information about specific card.
  * @param  cardinfo: pointer to a SD_CardInfo structure that contains all SD
  *         card information.
  * @retval The SD Response:
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
uint8_t SD_GetCardInfo(SD_CardInfo *cardinfo)
{
  uint8_t status = SD_RESPONSE_FAILURE;

  status = SD_GetCSDRegister(&(cardinfo->SD_csd));
  status = SD_GetCIDRegister(&(cardinfo->SD_cid));
  cardinfo->CardCapacity = (cardinfo->SD_csd.DeviceSize + 1) ;
  cardinfo->CardCapacity *= (1 << (cardinfo->SD_csd.DeviceSizeMul + 2));
  cardinfo->CardBlockSize = 1 << (cardinfo->SD_csd.RdBlockLen);
  cardinfo->CardCapacity *= cardinfo->CardBlockSize;

  /*!< Returns the reponse */
  return status;
}

/**
  * @brief  Reads a block of data from the SD.
  * @param  pBuffer: pointer to the buffer that receives the data read from the
  *                  SD.
  * @param  ReadAddr: SD's internal address to read from.
  * @param  BlockSize: the SD card Data block size.
  * @retval The SD Response:
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
//на высоких частотах функция не работала из-за того, что ответ карты SD_CMD_READ_SINGLE_BLOCK нужно ждать некоторое время
//при ограниченном числе проверок ответа в функции SD_GetResponse() (25 проверок) на высоких частотах карте не хватало времени, чтобы выдать ответ
//поэтому я увеличил количество проверок до 70 (для макс частоты 8МГц этого хватает карте чтобы выдать ответ)
uint8_t SD_ReadBlock(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t BlockSize)
{
  uint32_t i = 0;
  uint8_t rvalue = SD_RESPONSE_FAILURE;

  /*!< SD chip select low */
  SD_CS_LOW();

  /*!< Send CMD17 (SD_CMD_READ_SINGLE_BLOCK) to read one block */
  SD_SendCmd(SD_CMD_READ_SINGLE_BLOCK, ReadAddr, 0xFF);

  /*!< Check if the SD acknowledged the read block command: R1 response (0x00: no errors) */
  if (!SD_GetResponse(SD_RESPONSE_NO_ERROR))
  {
    /*!< Now look for the data token to signify the start of the data */
    if (!SD_GetResponse(SD_START_DATA_SINGLE_BLOCK_READ))
    {
      /*!< Read the SD block data : read NumByteToRead data */
      for (i = 0; i < BlockSize; i++)
      {
        /*!< Save the received data */
        *pBuffer = SD_ReadByte();

        /*!< Point to the next location where the byte read will be saved */
        pBuffer++;
      }
      /*!< Get CRC bytes (not really needed by us, but required by SD) */
      SD_ReadByte();
      SD_ReadByte();
      /*!< Set response value to success */
      rvalue = SD_RESPONSE_NO_ERROR;
    }
  }
  /*!< SD chip select high */
  SD_CS_HIGH();

  /*!< Send dummy byte: 8 Clock pulses of delay */
  SD_WriteByte(SD_DUMMY_BYTE);

  /*!< Returns the reponse */
  return rvalue;

}

//Данная функция немного изменена по отношению к SD_ReadBlock() для того, чтобы она нормально работала с библиотекой pff Petit FAT
//В этой функции задается отступ в секторе offset, с которого надо начать чтение, и количество байт count, которые надо считать
uint8_t SD_ReadBlock_PFF(uint8_t* pBuffer, uint32_t ReadAddr,  uint16_t offset, uint16_t count)
{
  uint16_t i = 0;
  uint8_t rvalue = SD_RESPONSE_FAILURE;

  /*!< Send CMD17 (SD_CMD_READ_SINGLE_BLOCK) to read one block */
  SD_SendCmd(SD_CMD_READ_SINGLE_BLOCK, ReadAddr, 0xFF);
  SD_WriteByte(SD_DUMMY_BYTE);

  /*!< Check if the SD acknowledged the read block command: R1 response (0x00: no errors) */
  if (!SD_GetResponse(SD_RESPONSE_NO_ERROR))
  {
    /*!< Now look for the data token to signify the start of the data */
    if (!SD_GetResponse(SD_START_DATA_SINGLE_BLOCK_READ))
    {
      //Запись в массив только count байтов с отступом в offset от начала сектора, остальные байты игнорируются
      for (i=0;i<512;i++)
            {
              if ( (i >= offset) && (i < offset+count) )
                pBuffer[i-offset]=SD_ReadByte();
              else
                SD_ReadByte();
            }
      /*!< Get CRC bytes (not really needed by us, but required by SD) */
      SD_ReadByte();
      SD_ReadByte();
      /*!< Set response value to success */
      rvalue = SD_RESPONSE_NO_ERROR;
    }
  }

  /*!< Send dummy byte: 8 Clock pulses of delay */
  SD_WriteByte(SD_DUMMY_BYTE);

  /*!< Returns the reponse */
  return rvalue;

}




/**
  * @brief  Read a buffer (many blocks) from the SD card.
  * @param  pBuffer : pointer to the buffer that receives the data read from the SD.
  * @param  ReadAddr : SD's internal address to read from.
  * @param  NumByteToRead : number of bytes to read from the SD.
  * @retval SD Response:
  *   - SD_RESPONSE_FAILURE: Sequence failed.
  *   - SD_RESPONSE_NO_ERROR: Sequence succeed.
  */
uint8_t SD_ReadBuffer(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)
{
  uint32_t i = 0, NbrOfBlock = 0, Offset = 0;
  uint8_t rvalue = SD_RESPONSE_FAILURE;

  /* Calculate number of blocks to read */
  NbrOfBlock = NumByteToRead / SD_BLOCK_SIZE;
  /* SD chip select low */
  SD_CS_LOW();

  /* Data transfer */
  while (NbrOfBlock --)
  {
    /* Send CMD17 (SD_READ_SINGLE_BLOCK) to read one block */
    SD_SendCmd (SD_CMD_READ_SINGLE_BLOCK, ReadAddr + Offset, 0xFF);
    /* Check if the SD acknowledged the read block command: R1 response (0x00: no errors) */
    if (SD_GetResponse(SD_RESPONSE_NO_ERROR))
    {
      return  SD_RESPONSE_FAILURE;
    }
    /* Now look for the data token to signify the start of the data */
    if (!SD_GetResponse(SD_START_DATA_SINGLE_BLOCK_READ))
    {
      /* Read the SD block data : read NumByteToRead data */
      for (i = 0; i < SD_BLOCK_SIZE; i++)
      {
        /* Read the pointed data */
        *pBuffer = SD_ReadByte();
        /* Point to the next location where the byte read will be saved */
        pBuffer++;
      }
      /* Set next read address*/
      Offset += 512;
      /* get CRC bytes (not really needed by us, but required by SD) */
      SD_ReadByte();
      SD_ReadByte();
      /* Set response value to success */
      rvalue = SD_RESPONSE_NO_ERROR;
    }
    else
    {
      /* Set response value to failure */
      rvalue = SD_RESPONSE_FAILURE;
    }
  }

  /* SD chip select high */
  SD_CS_HIGH();
  /* Send dummy byte: 8 Clock pulses of delay */
  SD_WriteByte(SD_DUMMY_BYTE);
  /* Returns the reponse */
  return rvalue;
}

/**
  * @brief  Writes a block on the SD
  * @param  pBuffer: pointer to the buffer containing the data to be written on
  *                  the SD.
  * @param  WriteAddr: address to write on.
  * @param  BlockSize: the SD card Data block size.
  * @retval The SD Response:
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
uint8_t SD_WriteBlock(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t BlockSize)
{
  uint32_t i = 0;
  uint8_t rvalue = SD_RESPONSE_FAILURE;

  /*!< SD chip select low */
  SD_CS_LOW();

  /*!< Send CMD24 (SD_CMD_WRITE_SINGLE_BLOCK) to write multiple block */
  SD_SendCmd(SD_CMD_WRITE_SINGLE_BLOCK, WriteAddr, 0xFF);

  /*!< Check if the SD acknowledged the write block command: R1 response (0x00: no errors) */
  if (!SD_GetResponse(SD_RESPONSE_NO_ERROR))
  {
    /*!< Send a dummy byte */
    SD_WriteByte(SD_DUMMY_BYTE);

    /*!< Send the data token to signify the start of the data */
    SD_WriteByte(0xFE);

    /*!< Write the block data to SD : write count data by block */
    for (i = 0; i < BlockSize; i++)
    {
      /*!< Send the pointed byte */
      SD_WriteByte(*pBuffer);
      /*!< Point to the next location where the byte read will be saved */
      pBuffer++;
    }

    /* Send DUMMY bytes when the number of data to be written are lower
       than the SD card BLOCK size (512 Byte) */
    for (; i != SD_BLOCK_SIZE; i++)
    {
      /* Send the pointed byte */
      SD_WriteByte(SD_DUMMY_BYTE);
    }

    /*!< Put CRC bytes (not really needed by us, but required by SD) */
    SD_ReadByte();
    SD_ReadByte();

    /*!< Read data response */
    if (SD_GetDataResponse() == SD_DATA_OK)
    {
      rvalue = SD_RESPONSE_NO_ERROR;
    }
  }
  /*!< SD chip select high */
  SD_CS_HIGH();
  /*!< Send dummy byte: 8 Clock pulses of delay */
  SD_WriteByte(SD_DUMMY_BYTE);

  /*!< Returns the reponse */
  return rvalue;

}


/**
  * @brief  Write a buffer (many blocks) in the SD card.
  * @note   The amount of data to write should be a multiple of SD card BLOCK
  *         size (512 Byte).
  * @param  pBuffer : pointer to the buffer containing the data to be written on the SD.
  * @param  WriteAddr : address to write on.
  * @param  NumByteToWrite : number of data to write.
  * @retval SD Response:
  *   - SD_RESPONSE_FAILURE: Sequence failed.
  *   - SD_RESPONSE_NO_ERROR: Sequence succeed.
  */
uint8_t SD_WriteBuffer(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite)
{
  uint32_t i = 0, NbrOfBlock = 0, Offset = 0;
  uint8_t rvalue = SD_RESPONSE_FAILURE;

  /* Calculate number of blocks to write */
  NbrOfBlock = NumByteToWrite / SD_BLOCK_SIZE;
  /* SD chip select low */
  SD_CS_LOW();

  /* Data transfer */
  while (NbrOfBlock--)
  {
    /* Send CMD24 (SD_WRITE_BLOCK) to write blocks */
    SD_SendCmd(SD_CMD_WRITE_SINGLE_BLOCK, WriteAddr + Offset, 0xFF);

    /* Check if the SD acknowledged the write block command: R1 response (0x00: no errors) */
    if (SD_GetResponse(SD_RESPONSE_NO_ERROR))
    {
      return SD_RESPONSE_FAILURE;
    }
    /* Send dummy byte */
    SD_WriteByte(SD_DUMMY_BYTE);
    /* Send the data token to signify the start of the data */
    SD_WriteByte(SD_START_DATA_SINGLE_BLOCK_WRITE);
    /* Write the block data to SD : write count data by block */
    for (i = 0; i < SD_BLOCK_SIZE; i++)
    {
      /* Send the pointed byte */
      SD_WriteByte(*pBuffer);
      /* Point to the next location where the byte read will be saved */
      pBuffer++;
    }
    /* Set next write address */
    Offset += 512;
    /* Put CRC bytes (not really needed by us, but required by SD) */
    SD_ReadByte();
    SD_ReadByte();
    /* Read data response */
    if (SD_GetDataResponse() == SD_DATA_OK)
    {
      /* Set response value to success */
      rvalue = SD_RESPONSE_NO_ERROR;
    }
    else
    {
      /* Set response value to failure */
      rvalue = SD_RESPONSE_FAILURE;
    }
  }

  /* SD chip select high */
  SD_CS_HIGH();
  /* Send dummy byte: 8 Clock pulses of delay */
  SD_WriteByte(SD_DUMMY_BYTE);
  /* Returns the reponse */
  return rvalue;
}

/**
  * @brief  Read the CSD card register.
  *         Reading the contents of the CSD register in SPI mode is a simple
  *         read-block transaction.
  * @param  SD_csd: pointer on an SCD register structure
  * @retval The SD Response:
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
uint8_t SD_GetCSDRegister(SD_CSD* SD_csd)
{
  uint32_t i = 0;
  uint8_t rvalue = SD_RESPONSE_FAILURE;
  uint8_t CSD_Tab[16];

  /*!< SD chip select low */
  SD_CS_LOW();
  /*!< Send CMD9 (CSD register) or CMD10(CSD register) */
  SD_SendCmd(SD_CMD_SEND_CSD, 0, 0xFF);
  /*!< Wait for response in the R1 format (0x00 is no errors) */
  if (!SD_GetResponse(SD_RESPONSE_NO_ERROR))
  {
    if (!SD_GetResponse(SD_START_DATA_SINGLE_BLOCK_READ))
    {
      for (i = 0; i < 16; i++)
      {
        /*!< Store CSD register value on CSD_Tab */
        CSD_Tab[i] = SD_ReadByte();
      }
    }
    /*!< Get CRC bytes (not really needed by us, but required by SD) */
    SD_WriteByte(SD_DUMMY_BYTE);
    SD_WriteByte(SD_DUMMY_BYTE);
    /*!< Set response value to success */
    rvalue = SD_RESPONSE_NO_ERROR;
  }
  /*!< SD chip select high */
  SD_CS_HIGH();
  /*!< Send dummy byte: 8 Clock pulses of delay */
  SD_WriteByte(SD_DUMMY_BYTE);

  /*!< Byte 0 */
  SD_csd->CSDStruct = (uint8_t)((CSD_Tab[0] & 0xC0) >> 6);
  SD_csd->SysSpecVersion = (uint8_t)((CSD_Tab[0] & 0x3C) >> 2);
  SD_csd->Reserved1 = (uint8_t)(CSD_Tab[0] & 0x03);

  /*!< Byte 1 */
  SD_csd->TAAC = CSD_Tab[1];

  /*!< Byte 2 */
  SD_csd->NSAC = CSD_Tab[2];

  /*!< Byte 3 */
  SD_csd->MaxBusClkFrec = CSD_Tab[3];

  /*!< Byte 4 */
  SD_csd->CardComdClasses = CSD_Tab[4] << 4;

  /*!< Byte 5 */
  SD_csd->CardComdClasses |= (CSD_Tab[5] & 0xF0) >> 4;
  SD_csd->RdBlockLen = (uint8_t)(CSD_Tab[5] & 0x0F);

  /*!< Byte 6 */
  SD_csd->PartBlockRead = (uint8_t)((CSD_Tab[6] & 0x80) >> 7);
  SD_csd->WrBlockMisalign = (uint8_t)((CSD_Tab[6] & 0x40) >> 6);
  SD_csd->RdBlockMisalign = (uint8_t)((CSD_Tab[6] & 0x20) >> 5);
  SD_csd->DSRImpl = (uint8_t)((CSD_Tab[6] & 0x10) >> 4);
  SD_csd->Reserved2 = 0; /*!< Reserved */

  SD_csd->DeviceSize = (CSD_Tab[6] & 0x03) << 10;

  /*!< Byte 7 */
  SD_csd->DeviceSize |= (CSD_Tab[7]) << 2;

  /*!< Byte 8 */
  SD_csd->DeviceSize |= (CSD_Tab[8] & 0xC0) >> 6;

  SD_csd->MaxRdCurrentVDDMin = (uint8_t)((CSD_Tab[8] & 0x38) >> 3);
  SD_csd->MaxRdCurrentVDDMax = (uint8_t)((CSD_Tab[8] & 0x07));

  /*!< Byte 9 */
  SD_csd->MaxWrCurrentVDDMin = (uint8_t)((CSD_Tab[9] & 0xE0) >> 5);
  SD_csd->MaxWrCurrentVDDMax = (uint8_t)((CSD_Tab[9] & 0x1C) >> 2);
  SD_csd->DeviceSizeMul = (uint8_t)((CSD_Tab[9] & 0x03) << 1);
  /*!< Byte 10 */
  SD_csd->DeviceSizeMul |= (uint8_t)((CSD_Tab[10] & 0x80) >> 7);

  SD_csd->EraseGrSize = (uint8_t)((CSD_Tab[10] & 0x40) >> 6);
  SD_csd->EraseGrMul = (uint8_t)((CSD_Tab[10] & 0x3F) << 1);

  /*!< Byte 11 */
  SD_csd->EraseGrMul |= (uint8_t)((CSD_Tab[11] & 0x80) >> 7);
  SD_csd->WrProtectGrSize = (uint8_t)((CSD_Tab[11] & 0x7F));

  /*!< Byte 12 */
  SD_csd->WrProtectGrEnable = (uint8_t)((CSD_Tab[12] & 0x80) >> 7);
  SD_csd->ManDeflECC = (uint8_t)((CSD_Tab[12] & 0x60) >> 5);
  SD_csd->WrSpeedFact = (uint8_t)((CSD_Tab[12] & 0x1C) >> 2);
  SD_csd->MaxWrBlockLen = (uint8_t)((CSD_Tab[12] & 0x03) << 2);

  /*!< Byte 13 */
  SD_csd->MaxWrBlockLen |= (uint8_t)((CSD_Tab[13] & 0xC0) >> 6);
  SD_csd->WriteBlockPaPartial = (uint8_t)((CSD_Tab[13] & 0x20) >> 5);
  SD_csd->Reserved3 = 0;
  SD_csd->ContentProtectAppli = (uint8_t)(CSD_Tab[13] & 0x01);

  /*!< Byte 14 */
  SD_csd->FileFormatGrouop = (uint8_t)((CSD_Tab[14] & 0x80) >> 7);
  SD_csd->CopyFlag = (uint8_t)((CSD_Tab[14] & 0x40) >> 6);
  SD_csd->PermWrProtect = (uint8_t)((CSD_Tab[14] & 0x20) >> 5);
  SD_csd->TempWrProtect = (uint8_t)((CSD_Tab[14] & 0x10) >> 4);
  SD_csd->FileFormat = (uint8_t)((CSD_Tab[14] & 0x0C) >> 2);
  SD_csd->ECC = (uint8_t)(CSD_Tab[14] & 0x03);

  /*!< Byte 15 */
  SD_csd->CSD_CRC = (uint8_t)((CSD_Tab[15] & 0xFE) >> 1);
  SD_csd->Reserved4 = 1;

  /*!< Return the reponse */
  return rvalue;
}

/**
  * @brief  Read the CID card register.
  *         Reading the contents of the CID register in SPI mode is a simple
  *         read-block transaction.
  * @param  SD_cid: pointer on an CID register structure
  * @retval The SD Response:
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
uint8_t SD_GetCIDRegister(SD_CID* SD_cid)
{
  uint32_t i = 0;
  uint8_t rvalue = SD_RESPONSE_FAILURE;
  uint8_t CID_Tab[16];

  /*!< SD chip select low */
  SD_CS_LOW();

  /*!< Send CMD10 (CID register) */
  SD_SendCmd(SD_CMD_SEND_CID, 0, 0xFF);

  /*!< Wait for response in the R1 format (0x00 is no errors) */
  if (!SD_GetResponse(SD_RESPONSE_NO_ERROR))
  {
    if (!SD_GetResponse(SD_START_DATA_SINGLE_BLOCK_READ))
    {
      /*!< Store CID register value on CID_Tab */
      for (i = 0; i < 16; i++)
      {
        CID_Tab[i] = SD_ReadByte();
      }
    }
    /*!< Get CRC bytes (not really needed by us, but required by SD) */
    SD_WriteByte(SD_DUMMY_BYTE);
    SD_WriteByte(SD_DUMMY_BYTE);
    /*!< Set response value to success */
    rvalue = SD_RESPONSE_NO_ERROR;
  }
  /*!< SD chip select high */
  SD_CS_HIGH();
  /*!< Send dummy byte: 8 Clock pulses of delay */
  SD_WriteByte(SD_DUMMY_BYTE);

  /*!< Byte 0 */
  SD_cid->ManufacturerID = CID_Tab[0];

  /*!< Byte 1 */
  SD_cid->OEM_AppliID = CID_Tab[1] << 8;

  /*!< Byte 2 */
  SD_cid->OEM_AppliID |= CID_Tab[2];

  /*!< Byte 3 */
  SD_cid->ProdName1 = ((uint32_t)CID_Tab[3] << 24);

  /*!< Byte 4 */
  SD_cid->ProdName1 |= ((uint32_t)CID_Tab[4] << 16);

  /*!< Byte 5 */
  SD_cid->ProdName1 |= CID_Tab[5] << 8;

  /*!< Byte 6 */
  SD_cid->ProdName1 |= CID_Tab[6];

  /*!< Byte 7 */
  SD_cid->ProdName2 = CID_Tab[7];

  /*!< Byte 8 */
  SD_cid->ProdRev = CID_Tab[8];

  /*!< Byte 9 */
  SD_cid->ProdSN = ((uint32_t)CID_Tab[9] << 24);

  /*!< Byte 10 */
  SD_cid->ProdSN |= ((uint32_t)CID_Tab[10] << 16);

  /*!< Byte 11 */
  SD_cid->ProdSN |= CID_Tab[11] << 8;

  /*!< Byte 12 */
  SD_cid->ProdSN |= CID_Tab[12];

  /*!< Byte 13 */
  SD_cid->Reserved1 |= (uint8_t)((CID_Tab[13] & 0xF0) >> 4);
  SD_cid->ManufactDate = (CID_Tab[13] & 0x0F) << 8;

  /*!< Byte 14 */
  SD_cid->ManufactDate |= CID_Tab[14];

  /*!< Byte 15 */
  SD_cid->CID_CRC = (uint8_t)((CID_Tab[15] & 0xFE) >> 1);
  SD_cid->Reserved2 = 1;

  /*!< Return the reponse */
  return rvalue;
}

/**
  * @brief  Send 5 bytes command to the SD card.
  * @param  Cmd: The user expected command to send to SD card.
  * @param  Arg: The command argument.
  * @param  Crc: The CRC.
  * @retval None
  */


void SD_SendCmd(uint8_t Cmd, uint32_t Arg, uint8_t Crc)
{
  uint32_t i = 0;

  uint8_t Frame[6];

  Frame[0] = (uint8_t)(Cmd | 0x40); /*!< Construct byte 1 */

  Frame[1] = (uint8_t)(Arg >> 24); /*!< Construct byte 2 */

  Frame[2] = (uint8_t)(Arg >> 16); /*!< Construct byte 3 */

  Frame[3] = (uint8_t)(Arg >> 8); /*!< Construct byte 4 */

  Frame[4] = (uint8_t)(Arg); /*!< Construct byte 5 */

  Frame[5] = (Crc); /*!< Construct CRC: byte 6 */

  for (i = 0; i < 6; i++)
  {
    SD_WriteByte(Frame[i]); /*!< Send the Cmd bytes */
  }
}


/**
  * @brief  Get SD card data response.
  * @param  None
  * @retval The SD status: Read data response xxx0<status>1
  *         - status 010: Data accecpted
  *         - status 101: Data rejected due to a crc error
  *         - status 110: Data rejected due to a Write error.
  *         - status 111: Data rejected due to other error.
  */
uint8_t SD_GetDataResponse(void)
{
  uint32_t i = 0;
  uint8_t response = 0, rvalue = 0;

  while (i <= 64)
  {
    /*!< Read response */
    response = SD_ReadByte();
    /*!< Mask unused bits */
    response &= 0x1F;
    switch (response)
    {
      case SD_DATA_OK:
      {
        rvalue = SD_DATA_OK;
        break;
      }
      case SD_DATA_CRC_ERROR:
        return SD_DATA_CRC_ERROR;
      case SD_DATA_WRITE_ERROR:
        return SD_DATA_WRITE_ERROR;
      default:
      {
        rvalue = SD_DATA_OTHER_ERROR;
        break;
      }
    }
    /*!< Exit loop in case of data ok */
    if (rvalue == SD_DATA_OK)
      break;
    /*!< Increment loop counter */
    i++;
  }

  /*!< Wait null data */
  while (SD_ReadByte() == 0);

  /*!< Return response */
  return response;
}

/**
  * @brief  Returns the SD response.
  * @param  None
  * @retval The SD Response:
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
uint8_t SD_GetResponse(uint8_t Response)
{
  uint8_t Count = SD_RESPONSE_CHECKS_COUNT; 
                      //было 25, но при этом функция некорректно работала с принятием Data token start byte, Start Single Block Read
                      //потому что необходимо подождать некоторое время пока карта обработает запрос и выдаст ответ
                      //на низких частотах хватало 25 проверок (за счёт бОльшего времени передачи данных), 
                      //на высоких необходимо повысить количесвто, на макс частоте 8МГц не менее 67(определил опытным путём)
  /*!< Check if response is got or a timeout is happen */
  while ((SD_ReadByte() != Response) && Count)
  {
    Count--;  //проверяем ответ несколько раз, при первом же совпадении с нужным значением ответа покидаем цикл
  }
  if (Count == 0)
  {
    /*!< After time out */
    return SD_RESPONSE_FAILURE;   //если после count раз мы так и не получили правильный ответ, тогда возвращаем значение ошибки 0xFF
  }
  else
  {
    /*!< Right response got */
    return SD_RESPONSE_NO_ERROR;  //если мы получили правильный ответ до окончания цикла, то возвращаем отсутствие ошибок 
  }
}

uint8_t SD_GetResponse1(void)
{
  uint8_t Count = 25;

  do{
      switch(SD_ReadByte()) {
    case SD_IN_READY_STATE:
        return SD_IN_READY_STATE;
    case SD_IN_IDLE_STATE:
        return SD_IN_IDLE_STATE;
    case SD_ERASE_RESET:
        return SD_ERASE_RESET;
    case SD_ILLEGAL_COMMAND:
        return SD_ILLEGAL_COMMAND;
    case SD_COM_CRC_ERROR:
        return SD_COM_CRC_ERROR;
    case SD_ERASE_SEQUENCE_ERROR:
        return SD_ERASE_SEQUENCE_ERROR;
    case SD_ADDRESS_ERROR:
        return SD_ADDRESS_ERROR;
    case SD_PARAMETER_ERROR:
        return SD_PARAMETER_ERROR;
    default:
        Count--;
        break;
    };
  } while (Count);

    return SD_RESPONSE_FAILURE;
};


/**
  * @brief  Returns the SD status.
  * @param  None
  * @retval The SD status.
  */
uint16_t SD_GetStatus(void)
{
  uint16_t Status = 0;

  /*!< SD chip select low */
  SD_CS_LOW();

  /*!< Send CMD13 (SD_SEND_STATUS) to get SD status */
  SD_SendCmd(SD_CMD_SEND_STATUS, 0, 0xFF);

  Status = SD_ReadByte();
  Status |= (uint16_t)(SD_ReadByte() << 8);

  /*!< SD chip select high */
  SD_CS_HIGH();

  /*!< Send dummy byte 0xFF */
  SD_WriteByte(SD_DUMMY_BYTE);

  return Status;
}

/**
  * @brief  Put SD in Idle state.
  * @param  None
  * @retval The SD Response:
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
uint8_t SD_GoIdleState(void)
{
  /*!< SD chip select low */
  SD_CS_LOW();

  /*!< Send CMD0 (SD_CMD_GO_IDLE_STATE) to put SD in SPI mode */
  SD_SendCmd(SD_CMD_GO_IDLE_STATE, 0, 0x95);

  /*!< Wait for In Idle State Response (R1 Format) equal to 0x01 */
  if (SD_GetResponse(SD_IN_IDLE_STATE))
  {
    /*!< No Idle State Response: return response failue */
    return SD_RESPONSE_FAILURE;
  }
  /*----------Activates the card initialization process-----------*/
  do
  {
    /*!< SD chip select high */
    SD_CS_HIGH();

    /*!< Send Dummy byte 0xFF */
    SD_WriteByte(SD_DUMMY_BYTE);

    /*!< SD chip select low */
    SD_CS_LOW();

    /*!< Send CMD1 (Activates the card process) until response equal to 0x0 */
    SD_SendCmd(SD_CMD_SEND_OP_COND, 0, 0xFF);
    /*!< Wait for no error Response (R1 Format) equal to 0x00 */
  }
  while (SD_GetResponse(SD_RESPONSE_NO_ERROR));

  /*!< SD chip select high */
  SD_CS_HIGH();

  /*!< Send dummy byte 0xFF */
  SD_WriteByte(SD_DUMMY_BYTE);

  return SD_RESPONSE_NO_ERROR;
}

/**
  * @brief  Write a byte on the SD.
  * @param  Data: byte to send.
  * @retval None
  */
uint8_t SD_WriteByte(uint8_t Data)
{
  /*!< Wait until the transmit buffer is empty */
  while (SPI_GetFlagStatus(SD_SPI, SPI_FLAG_TXE) == RESET)
  {}

  /*!< Send the byte */
  SPI_SendData(SD_SPI, Data);

  /*!< Wait to receive a byte*/
  while (SPI_GetFlagStatus(SD_SPI, SPI_FLAG_RXNE) == RESET)
  {}

  /*!< Return the byte read from the SPI bus */
  return SPI_ReceiveData(SD_SPI);
}

/**
  * @brief  Read a byte from the SD.
  * @param  None
  * @retval The received byte.
  */
uint8_t SD_ReadByte(void)
{
  uint8_t Data = 0;

  /*!< Wait until the transmit buffer is empty */
  while (SPI_GetFlagStatus(SD_SPI, SPI_FLAG_TXE) == RESET)
  {}
  /*!< Send the byte */
  SPI_SendData(SD_SPI, SD_DUMMY_BYTE);

  /*!< Wait until a data is received */
  while (SPI_GetFlagStatus(SD_SPI, SPI_FLAG_RXNE) == RESET)
  {}
  /*!< Get the received data */
  Data = SPI_ReceiveData(SD_SPI);

  /*!< Return the shifted data */
  return Data;
}

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
