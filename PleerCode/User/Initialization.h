//В исходниках Initialization будут находиться функции инициализации
// TIM1 в режиме энкодера, 
// TIM2 для сканирования кнопки,
// TIM4,
// Кнопки энкодера,
// ЦАП

#include "stm8l15x.h"
#include "stm8l15x_gpio.h"
#include "stm8l15x_clk.h"
#include "stm8l15x_dac.h"
#include "stm8l15x_tim1.h"
#include "stm8l15x_tim2.h"
#include "stm8l15x_tim4.h"


#define Button_IT_Flag  EXTI_IT_Pin3 
#define Button_Pin      GPIO_Pin_3
#define Button_Port     GPIOD

// при частоте после предделителя таймера 4 Мгц
#define TIM4_22kHz_FREQ  181//181  
#define TIM4_24kHz_FREQ  220
#define TIM4_32kHz_FREQ  126
#define TIM4_44kHz_FREQ  91 

// частота опроса кнопки 
// условимся, что минимальное время нажатия кнопки 50мс, на короткое нажатие должно приходить 3 прерывания TIM1
// для этого вполне подойдёт частота 50Гц(T=20мс), модуль счётчика подобран исходя из частоты после предделителя 125кГц
// Короткое нажатие до 1с (pulseTime<50)
// Длинное нажатие от 1с до 5с (50<pulseTime<250)
// Более 5с pulseTime не увеличивается, нажатие считается длинным
#define TIM2_50Hz_FREQ 2500
#define MAX_SHORT_TIME 50    //максимальное кол-во обноружений нажатой кнопки в прерываниях для короткого нажатия
#define MAX_LONG_TIME 250    //максимальное кол-во обноружений нажатой кнопки в прерываниях для долгого нажатия


void TIM1_Initialization(void);
void TIM2_Initialization(void);
void TIM4_Initialization(void);
void Button_Initialization(void);
void DAC_Initialization(void);