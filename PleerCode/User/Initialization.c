#include "Initialization.h"


// ИНИЦИАЛИЗАЦИЯ TIM4
void TIM4_Initialization(void)
{
  // включаем подачу сигнала тактирования на TIM4 
  CLK_PeripheralClockConfig( CLK_Peripheral_TIM4, ENABLE);
  // будем считать, что общая системная частота равна 16 МГц 
  TIM4_TimeBaseInit(TIM4_Prescaler_4, TIM4_22kHz_FREQ);    //при изменении системной частоты изменить значение предделителя и периода
  // разрешаем прерывания
  TIM4_ITConfig(TIM4_IT_Update, ENABLE); 
  // в качестве сигнала TIM4_TRGO будет Update Event
  TIM4_SelectOutputTrigger(TIM4_TRGOSource_Update);
  // включаем TIM4
  // TIM4_Cmd(ENABLE);
}

//ИНИЦИАЛИЗАЦИЯ ЦАП
void DAC_Initialization(void)
{
 // Для работы ЦАП необходимо включить выход в аналоговом режиме (RM0031 стр.121)
 GPIO_Init( GPIOF , GPIO_Pin_0, GPIO_Mode_In_FL_No_IT); 
 // Подаём сигнал тактирования на ЦАП
 CLK_PeripheralClockConfig(CLK_Peripheral_DAC, ENABLE);
 // Настраивается конфигурация ЦАП, важно, что в качестве триггера используется TIM4_TRGO, настроенный на Update Event
 DAC_Init(DAC_Channel_1, DAC_Trigger_T4_TRGO, DAC_OutputBuffer_Enable);
 // Включение ЦАП
 DAC_Cmd(DAC_Channel_1, ENABLE);
}


// Инициализация кнопки
void Button_Initialization(void)
{
	GPIO_Init( Button_Port, Button_Pin, GPIO_Mode_In_PU_IT );
  EXTI_SetPinSensitivity(Button_IT_Flag, EXTI_Trigger_Falling);
}