/**
  ******************************************************************************
  * @file    stm8l1526_eval.h
  * @author  MCD Application Team
  * @version V2.1.3
  * @date    28-June-2013
  * @brief   This file contains definitions for STM8L1526_EVAL's Leds, push-buttons
  *          and COM ports hardware resources.
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
#ifndef __STM8L1526_EVAL_H
#define __STM8L1526_EVAL_H

/* Includes ------------------------------------------------------------------*/
#include "stm8_eval.h"

/** @addtogroup STM8L1526_EVAL_SD_SPI
  * @{
  */
/**
  * @brief  SD SPI Interface pins
  */
#define SD_SPI                           SPI1
#define SD_SPI_CLK                       CLK_Peripheral_SPI1
#define SD_SPI_SCK_PIN                   GPIO_Pin_5                 /* PB.05 */
#define SD_SPI_SCK_GPIO_PORT             GPIOB                       /* GPIOB */
#define SD_SPI_MISO_PIN                  GPIO_Pin_7                  /* PB.07 */
#define SD_SPI_MISO_GPIO_PORT            GPIOB                       /* GPIOB */
#define SD_SPI_MOSI_PIN                  GPIO_Pin_6                  /* PB.06 */
#define SD_SPI_MOSI_GPIO_PORT            GPIOB                       /* GPIOB */
#define SD_CS_PIN                        GPIO_Pin_4                  /* PB.04 */
#define SD_CS_GPIO_PORT                  GPIOB                       /* GPIOB */
#define SD_DETECT_PIN                    GPIO_Pin_6                  /* PD.06 */   
#define SD_DETECT_GPIO_PORT              GPIOD                       /* GPIOD */

/**
  * @}
  */


/* Exported Macros ------------------------------------------------------------*/

/** @defgroup STM8L1526_EVAL_LOW_LEVEL_Exported_Functions
  * @{
  */

void SD_LowLevel_DeInit(void);
void SD_LowLevel_Init(void);

//эти функции написаны мною, немного отличаюся от тех, что выше
void SD_SPI_Init(void);
void SD_SPI_Init_HighFreq(void);
void SD_SPI_DeInit(void);

/**
  * @}
  */

#endif /* __STM8L1526_EVAL_H */

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
