//В исходниках Initialization будут находиться функции инициализации (без включения)
// TIM1 в режиме энкодера, 
// TIM4,
// Кнопки энкодера,
// ЦАП

#include "stm8l15x.h"
#include "stm8l15x_gpio.h"
#include "stm8l15x_clk.h"
#include "stm8l15x_dac.h"
#include "stm8l15x_tim4.h"

#define Button_IT_Flag  EXTI_IT_Pin3 
#define Button_Pin      GPIO_Pin_3
#define Button_Port     GPIOD

//при частоте после предделителя таймера 4 Мгц
#define TIM4_22kHz_FREQ  181//181  
#define TIM4_24kHz_FREQ  220
#define TIM4_32kHz_FREQ  126
#define TIM4_44kHz_FREQ  91 


void TIM1_Initialization(void);
void TIM4_Initialization(void);
void Button_Initialization(void);
void DAC_Initialization(void);