/**
  ******************************************************************************
  * @file    stm8l_eval.c
  * @author  MCD Application Team
  * @version V2.1.3
  * @date    28-June-2013
  * @brief   This file provides firmware functions to manage Leds, push-buttons
  *          and COM ports available on STM8L Evaluation Boards from STMicroelectronics.
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
#include "stm8l1526_eval.h"
#include "stm8l15x_spi.h"


/** @addtogroup Utilities
  * @{
  */

/** @addtogroup STM8_EVAL
  * @{
  */

/** @addtogroup STM8L1526_EVAL
  * @{
  */


/**
  * @brief  DeInitializes the SD/SD communication.
  * @param  None
  * @retval None
  */
void SD_LowLevel_DeInit(void)
{
  SPI_Cmd(SD_SPI, DISABLE); /*!< SD_SPI disable */

  /*!< SD_SPI Periph clock disable */
  CLK_PeripheralClockConfig(SD_SPI_CLK, DISABLE);

  /*!< Configure SD_SPI pins: SCK */
  GPIO_Init(SD_SPI_SCK_GPIO_PORT, SD_SPI_SCK_PIN, GPIO_Mode_In_FL_No_IT);

  /*!< Configure SD_SPI pins: MISO */
  GPIO_Init(SD_SPI_MISO_GPIO_PORT, SD_SPI_MISO_PIN, GPIO_Mode_In_FL_No_IT);

  /*!< Configure SD_SPI pins: MOSI */
  GPIO_Init(SD_SPI_MOSI_GPIO_PORT, SD_SPI_MOSI_PIN, GPIO_Mode_In_FL_No_IT);

  /*!< Configure SD_SPI_CS_PIN pin: SD Card CS pin */
  GPIO_Init(SD_CS_GPIO_PORT, SD_CS_PIN, GPIO_Mode_In_FL_No_IT);

  /*!< Configure SD_SPI_DETECT_PIN pin: SD Card detect pin */
  GPIO_Init(SD_DETECT_GPIO_PORT, SD_DETECT_PIN, GPIO_Mode_In_FL_No_IT);
}

/**
  * @brief  Initializes the SD_SPI and CS pins.
  * @param  None
  * @retval None
  */
void SD_LowLevel_Init(void)
{
  /* Enable SPI clock */
  CLK_PeripheralClockConfig(SD_SPI_CLK, ENABLE);

  /* Set the MOSI,MISO and SCK at high level */
  GPIO_ExternalPullUpConfig(SD_SPI_SCK_GPIO_PORT, SD_SPI_MISO_PIN | SD_SPI_MOSI_PIN | \
                            SD_SPI_SCK_PIN, ENABLE);

  /* SD_SPI Config */
  SPI_Init(SD_SPI, SPI_FirstBit_MSB, SPI_BaudRatePrescaler_4, SPI_Mode_Master,
           SPI_CPOL_High, SPI_CPHA_2Edge, SPI_Direction_2Lines_FullDuplex,
           SPI_NSS_Soft, 0x07);


  /* SD_SPI enable */
  SPI_Cmd(SD_SPI, ENABLE);

  /* Set MSD ChipSelect pin in Output push-pull high level */
  GPIO_Init(SD_CS_GPIO_PORT, SD_CS_PIN, GPIO_Mode_Out_PP_High_Slow);   //это место напрягает, так как в качестве SD_CS_PIN передаётся макрос GPIO_Pin_3,
                                                                       // но он не определён. Такое значение есть в перечислении GPIO_Pin_TypeDef, 
                                                                       //однако в функцию передаётся почему то uint8_t
}


void SD_SPI_Init(void)
{
  
  //ТАКТИРОВАНИЕ
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1); // основное тактирование без делителя, т.е. на 16 Мгц,  
                                        //ВНИМАНИЕ, в дальнейшем при изменении частоты, минимальное значение может быть не менее 8 МГц, либо нужно изменить коэф. деления 
                                        //на переферийном делителе, чтобы частота SPI была в диапазоне 100кГц-400кГц

  CLK_PeripheralClockConfig(SD_SPI_CLK, ENABLE); // включаем тактирование SPI модуля
  
  //УСТАНОВКА ПИНОВ
  SD_SPI_SCK_GPIO_PORT->DDR|=SD_SPI_MOSI_PIN |SD_SPI_SCK_PIN|SD_CS_PIN; //SCK, MOSI, CS устанавливаем как выходы, MISO - вход - начальное значение 

  GPIO_ExternalPullUpConfig(SD_SPI_SCK_GPIO_PORT, SD_SPI_MISO_PIN | SD_SPI_MOSI_PIN |  SD_SPI_SCK_PIN | SD_CS_PIN, ENABLE);    //переводим  MISO во вход с подтяжкой, а MOSI, CS и SCK в режим push-pull 
                           
  SD_SPI_SCK_GPIO_PORT->ODR|=SD_CS_PIN;  //сначала поставим линии CS в высокий уровень
  
  //РЕЖИМ SPI 
  //делитель ставим на 64, в итоге получается частота на SCK равная 16МГц/64=250кГц, ВНИМАНИЕ!!! при инициализации частота на SCK должна быть в диапазоне 100кГц-400кГЦ
  SPI_Init (SD_SPI,SPI_FirstBit_MSB,SPI_BaudRatePrescaler_64,SPI_Mode_Master,SPI_CPOL_Low,SPI_CPHA_1Edge, \
   SPI_Direction_2Lines_FullDuplex,SPI_NSS_Soft, 0x07); //устанваливаем режим работы модуля
  
  SPI_Cmd(SD_SPI, ENABLE); //включаем модуль SPI
  
  SD_SPI_SCK_GPIO_PORT->ODR&=~SD_CS_PIN;  //линию CS притягиваем к земле

}


void SD_SPI_Init_HighFreq(void)  
{
  //РЕЖИМ SPI 

  SPI_Cmd(SD_SPI, DISABLE);
  //После завершения инициализации скорость передачи данных по SPI можно увеличить
  //Повышать скорость можно в любой момент после инициализации, но не в момент передачи байта
  SPI_Init (SD_SPI,SPI_FirstBit_MSB,SPI_High_Freq,SPI_Mode_Master,SPI_CPOL_Low,SPI_CPHA_1Edge, \
   SPI_Direction_2Lines_FullDuplex,SPI_NSS_Soft, 0x07); //устанваливаем режим работы модуля
  
  SPI_Cmd(SD_SPI, ENABLE); //включаем модуль SPI

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


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
