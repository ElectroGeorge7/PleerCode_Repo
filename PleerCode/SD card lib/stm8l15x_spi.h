/**
  ******************************************************************************
  * @file    stm8l15x_spi.h
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    07/14/2010
  * @brief   This file contains all the functions prototypes for the SPI firmware
  *          library.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM8L15x_SPI_H
#define __STM8L15x_SPI_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */

/** @addtogroup SPI_Exported_Types
  * @{
  */

/**
  * @brief SPI data direction mode
  * Warning: element values correspond to BDM, BDOE, RXONLY bits position
  */
typedef enum {
  SPI_Direction_2Lines_FullDuplex = (uint8_t)0x00, /*!< 2-line uni-directional data mode enable */
  SPI_Direction_2Lines_RxOnly     = (uint8_t)0x04, /*!< Receiver only in 2 line uni-directional data mode */
  SPI_Direction_1Line_Rx          = (uint8_t)0x80, /*!< Receiver only in 1 line bi-directional data mode */
  SPI_Direction_1Line_Tx          = (uint8_t)0xC0  /*!< Transmit only in 1 line bi-directional data mode */
} SPI_DirectionMode_TypeDef;

/**
  * @brief SPI Slave Select management
  * Warning: element values correspond to LSBFIRST bit position
  */
typedef enum
{
  SPI_NSS_Soft  = (uint8_t)0x02, /*!< Software slave management disabled */
  SPI_NSS_Hard  = (uint8_t)0x00  /*!< Software slave management enabled */
} SPI_NSS_TypeDef;


/**
  * @brief SPI direction transmit/receive
  */

typedef enum {
  SPI_Direction_Rx = (uint8_t)0x00, /*!< Select Rx receive direction in bi-directional mode */
  SPI_Direction_Tx = (uint8_t)0x01  /*!< Select Tx transmission direction in bi-directional mode */
} SPI_Direction_TypeDef;

/**
  * @brief SPI master/slave mode
  * Warning: element values correspond to MSTR bit position
  */
typedef enum {
  SPI_Mode_Master = (uint8_t)0x04, /*!< SPI Master configuration */
  SPI_Mode_Slave  = (uint8_t)0x00  /*!< SPI Slave configuration */
} SPI_Mode_TypeDef;

/**
  * @brief SPI BaudRate Prescaler
  * Warning: element values correspond to BR bits position
  */
typedef enum {
  SPI_BaudRatePrescaler_2   = (uint8_t)0x00, /*!< SPI frequency = frequency(CPU)/2 */
  SPI_BaudRatePrescaler_4   = (uint8_t)0x08, /*!< SPI frequency = frequency(CPU)/4 */
  SPI_BaudRatePrescaler_8   = (uint8_t)0x10, /*!< SPI frequency = frequency(CPU)/8 */
  SPI_BaudRatePrescaler_16  = (uint8_t)0x18, /*!< SPI frequency = frequency(CPU)/16 */
  SPI_BaudRatePrescaler_32  = (uint8_t)0x20, /*!< SPI frequency = frequency(CPU)/32 */
  SPI_BaudRatePrescaler_64  = (uint8_t)0x28, /*!< SPI frequency = frequency(CPU)/64 */
  SPI_BaudRatePrescaler_128 = (uint8_t)0x30, /*!< SPI frequency = frequency(CPU)/128 */
  SPI_BaudRatePrescaler_256 = (uint8_t)0x38  /*!< SPI frequency = frequency(CPU)/256 */
} SPI_BaudRatePrescaler_TypeDef;

/**
  * @brief SPI Clock Polarity
  * Warning: element values correspond to CPOL bit position
  */
typedef enum {
  SPI_CPOL_Low  = (uint8_t)0x00, /*!< Clock to 0 when idle */
  SPI_CPOL_High = (uint8_t)0x02  /*!< Clock to 1 when idle */
} SPI_CPOL_TypeDef;

/**
  * @brief SPI Clock Phase
  * Warning: element values correspond to CPHA bit position
  */
typedef enum {
  SPI_CPHA_1Edge = (uint8_t)0x00, /*!< The first clock transition is the first data capture edge */
  SPI_CPHA_2Edge = (uint8_t)0x01  /*!< The second clock transition is the first data capture edge */
} SPI_CPHA_TypeDef;

/**
  * @brief SPI Frame Format: MSB or LSB transmitted first
  * Warning: element values correspond to LSBFIRST bit position
  */
typedef enum {
  SPI_FirstBit_MSB = (uint8_t)0x00, /*!< MSB bit will be transmitted first */
  SPI_FirstBit_LSB = (uint8_t)0x80  /*!< LSB bit will be transmitted first */
} SPI_FirstBit_TypeDef;


/**
  * @brief SPI DMA transfer requests
  */
typedef enum {
  SPI_DMAReq_RX = (uint8_t)0x01,    /*!< SPI DMA Rx transfer requests */
  SPI_DMAReq_TX = (uint8_t)0x02     /*!< SPI DMA Tx transfer requests */
} SPI_DMAReq_TypeDef;

/**
  * @brief SPI CRC Transmit/Receive
  */
typedef enum {
  SPI_CRC_RX = (uint8_t)0x00, /*!< Select Tx CRC register */
  SPI_CRC_TX = (uint8_t)0x01  /*!< Select Rx CRC register */
} SPI_CRC_TypeDef;

/**
  * @brief SPI flags definition - Warning : FLAG value = mapping position register
  */
typedef enum {
  SPI_FLAG_BSY    = (uint8_t)0x80, /*!< Busy flag */
  SPI_FLAG_OVR    = (uint8_t)0x40, /*!< Overrun flag */
  SPI_FLAG_MODF   = (uint8_t)0x20, /*!< Mode fault */
  SPI_FLAG_CRCERR = (uint8_t)0x10, /*!< CRC error flag */
  SPI_FLAG_WKUP   = (uint8_t)0x08, /*!< Wake-up flag */
  SPI_FLAG_TXE    = (uint8_t)0x02, /*!< Transmit buffer empty */
  SPI_FLAG_RXNE   = (uint8_t)0x01  /*!< Receive buffer empty */
} SPI_FLAG_TypeDef;

/**
  * @brief SPI_IT possible values
  * Elements values convention: 0xYX
  *   X: Position of the corresponding Interrupt
  *   Y: ITPENDINGBIT position
  */
typedef enum
{
  SPI_IT_WKUP   = (uint8_t)0x34, /*!< Wake-up interrupt*/
  SPI_IT_OVR    = (uint8_t)0x65,  /*!< Overrun interrupt*/
  SPI_IT_MODF   = (uint8_t)0x55, /*!< Mode fault interrupt*/
  SPI_IT_CRCERR = (uint8_t)0x45, /*!< CRC error interrupt*/
  SPI_IT_TXE    = (uint8_t)0x17, /*!< Transmit buffer empty interrupt*/
  SPI_IT_RXNE   = (uint8_t)0x06, /*!< Receive buffer not empty interrupt*/
  SPI_IT_ERR    = (uint8_t)0x05  /*!< Error interrupt*/
} SPI_IT_TypeDef;

/**
  * @}
  */

/* Private define ------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/** @addtogroup SPI_Private_Macros
  * @brief Macros used by the assert_param function to check the different functions parameters.
  * @{
  */

/**
  * @brief Macro used by the assert_param function in order to check the data direction mode values
  */
#define IS_SPI_DATA_DIRECTION(Mode) (((Mode) == SPI_Direction_2Lines_FullDuplex) || \
                                     ((Mode) == SPI_Direction_2Lines_RxOnly) || \
                                     ((Mode) == SPI_Direction_1Line_Rx) || \
                                     ((Mode) == SPI_Direction_1Line_Tx))

/**
  * @brief Macro used by the assert_param function in order to check the mode half duplex data direction values
  */
#define IS_SPI_DIRECTION(Direction) (((Direction) == SPI_Direction_Rx) || \
                                     ((Direction) == SPI_Direction_Tx))

/**
  * @brief Macro used by the assert_param function in order to check the NSS management values
  */
#define IS_SPI_SLAVEMANAGEMENT(NSS) (((NSS) == SPI_NSS_Soft) || \
                                     ((NSS) == SPI_NSS_Hard))


/**
  * @brief Macro used by the assert_param function in order to check the different
  *  sensitivity values for the CRC polynomial
  */
#define IS_SPI_CRC_POLYNOMIAL(Polynomial) ((Polynomial) > (uint8_t)0x00)

/**
  * @brief Macro used by the assert_param function in order to check the SPI Mode values
  */
#define IS_SPI_MODE(Mode) (((Mode) == SPI_Mode_Master) || \
                           ((Mode) == SPI_Mode_Slave))

/**
  * @brief Macro used by the assert_param function in order to check the baudrate values
  */
#define IS_SPI_BAUDRATE_PRESCALER(Prescaler) (((Prescaler) == SPI_BaudRatePrescaler_2) || \
    ((Prescaler) == SPI_BaudRatePrescaler_4) || \
    ((Prescaler) == SPI_BaudRatePrescaler_8) || \
    ((Prescaler) == SPI_BaudRatePrescaler_16) || \
    ((Prescaler) == SPI_BaudRatePrescaler_32) || \
    ((Prescaler) == SPI_BaudRatePrescaler_64) || \
    ((Prescaler) == SPI_BaudRatePrescaler_128) || \
    ((Prescaler) == SPI_BaudRatePrescaler_256))

/**
  * @brief Macro used by the assert_param function in order to check the polarity values
  */
#define IS_SPI_POLARITY(ClkPol) (((ClkPol) == SPI_CPOL_Low) || \
                                 ((ClkPol) == SPI_CPOL_High))

/**
  * @brief Macro used by the assert_param function in order to check the phase values
  */
#define IS_SPI_PHASE(ClkPha) (((ClkPha) == SPI_CPHA_1Edge) || \
                              ((ClkPha) == SPI_CPHA_2Edge))

/**
  * @brief Macro used by the assert_param function in order to check the first bit
  *   to be transmited values
  */
#define IS_SPI_FIRSTBIT(Bit) (((Bit) == SPI_FirstBit_MSB) || \
                              ((Bit) == SPI_FirstBit_LSB))

/**
  * @brief Macro used by the assert_param function in order to check the CRC Transmit/Receive
  */
#define IS_SPI_CRC(CRC) (((CRC) == SPI_CRC_TX) || \
                         ((CRC) == SPI_CRC_RX))

/**
  * @brief Macro used by the assert_param function in order to check the DMA transfer requests
  */
#define IS_SPI_DMAREQ(DMAREQ) ((((DMAREQ) & (uint16_t)0xFC) == 0x00) && ((DMAREQ) != 0x00))


/**
  * @brief Macro used by the assert_param function in order to check the different flags values
  */
#define IS_SPI_FLAG(Flag) (((Flag) == SPI_FLAG_OVR) || \
                           ((Flag) == SPI_FLAG_MODF) || \
                           ((Flag) == SPI_FLAG_CRCERR) || \
                           ((Flag) == SPI_FLAG_WKUP) || \
                           ((Flag) == SPI_FLAG_TXE) || \
                           ((Flag) == SPI_FLAG_RXNE) || \
                           ((Flag) == SPI_FLAG_BSY))

/**
  * @brief Macro used by the assert_param function in order to check the different
  *   sensitivity values for the flag that can be cleared by writing 0
  */
#define IS_SPI_CLEAR_FLAG(Flag) (((Flag) == SPI_FLAG_CRCERR) || \
                                 ((Flag) == SPI_FLAG_WKUP))



/**
  * @brief Macro used by the assert_param function in order to check the different
  *   sensitivity values for the Interrupts
  */
#define IS_SPI_CONFIG_IT(Interrupt) (((Interrupt) == SPI_IT_TXE)  || \
                                     ((Interrupt) == SPI_IT_RXNE)  || \
                                     ((Interrupt) == SPI_IT_ERR) || \
                                     ((Interrupt) == SPI_IT_WKUP))



/**
  * @brief Macro used by the assert_param function in order to check the different
  *   sensitivity values for the pending bit
  */
#define IS_SPI_GET_IT(ITPendingBit) (((ITPendingBit) == SPI_IT_OVR)  || \
                                     ((ITPendingBit) == SPI_IT_MODF) || \
                                     ((ITPendingBit) == SPI_IT_CRCERR) || \
                                     ((ITPendingBit) == SPI_IT_WKUP) || \
                                     ((ITPendingBit) == SPI_IT_TXE)  || \
                                     ((ITPendingBit) == SPI_IT_RXNE))



/**
  * @brief Macro used by the assert_param function in order to check the different
  *   sensitivity values for the pending bit that can be cleared by writing 0
  */
#define IS_SPI_CLEAR_IT(ITPendingBit) (((ITPendingBit) == SPI_IT_CRCERR) || \
                                       ((ITPendingBit) == SPI_IT_WKUP))

/**
  * @}
  */

/* Exported functions ------------------------------------------------------- */

/** @addtogroup SPI_Exported_Functions
  * @{
  */
void SPI_DeInit(SPI_TypeDef* SPIx);
void SPI_Init(SPI_TypeDef* SPIx, SPI_FirstBit_TypeDef SPI_FirstBit,
              SPI_BaudRatePrescaler_TypeDef SPI_BaudRatePrescaler,
              SPI_Mode_TypeDef SPI_Mode, SPI_CPOL_TypeDef SPI_CPOL,
              SPI_CPHA_TypeDef SPI_CPHA, SPI_DirectionMode_TypeDef SPI_Data_Direction,
              SPI_NSS_TypeDef SPI_Slave_Management, uint8_t CRCPolynomial);
void SPI_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState);
void SPI_ITConfig(SPI_TypeDef* SPIx, SPI_IT_TypeDef SPI_IT, FunctionalState NewState);
void SPI_SendData(SPI_TypeDef* SPIx, uint8_t Data);
uint8_t SPI_ReceiveData(SPI_TypeDef* SPIx);
void SPI_NSSInternalSoftwareCmd(SPI_TypeDef* SPIx, FunctionalState NewState);
void SPI_TransmitCRC(SPI_TypeDef* SPIx);
void SPI_CalculateCRCCmd(SPI_TypeDef* SPIx, FunctionalState NewState);
uint8_t SPI_GetCRC(SPI_TypeDef* SPIx, SPI_CRC_TypeDef SPI_CRC);
void SPI_ResetCRC(SPI_TypeDef* SPIx);
uint8_t SPI_GetCRCPolynomial(SPI_TypeDef* SPIx);
void SPI_BiDirectionalLineConfig(SPI_TypeDef* SPIx, SPI_Direction_TypeDef SPI_Direction);
void SPI_DMACmd(SPI_TypeDef* SPIx, SPI_DMAReq_TypeDef SPI_DMAReq, FunctionalState NewState);
FlagStatus SPI_GetFlagStatus(SPI_TypeDef* SPIx, SPI_FLAG_TypeDef SPI_FLAG);
void SPI_ClearFlag(SPI_TypeDef* SPIx, SPI_FLAG_TypeDef SPI_FLAG);
ITStatus SPI_GetITStatus(SPI_TypeDef* SPIx, SPI_IT_TypeDef SPI_IT);
void SPI_ClearITPendingBit(SPI_TypeDef* SPIx, SPI_IT_TypeDef SPI_IT);

/**
  * @}
  */

#endif /* __STM8L15x_SPI_H */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
