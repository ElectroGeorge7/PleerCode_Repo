#include "Initialization.h"


// ИНИЦИАЛИЗАЦИЯ TIM1
void TIM1_Initialization(void)
{
  // включаем подачу сигнала тактирования на TIM1
  CLK_PeripheralClockConfig( CLK_Peripheral_TIM1, ENABLE);
  // включаем режим энкодера
  TIM1_EncoderInterfaceConfig(TIM1_EncoderMode_TI12, TIM1_ICPolarity_Rising, TIM1_ICPolarity_Rising);
  // включаем TIM2
  // TIM1_Cmd(ENABLE);
}

// ИНИЦИАЛИЗАЦИЯ TIM2
void TIM2_Initialization(void)
{
  // включаем подачу сигнала тактирования на TIM2 
  CLK_PeripheralClockConfig( CLK_Peripheral_TIM2, ENABLE);
  // будем считать, что общая системная частота равна 16 МГц 
  // при изменении системной частоты изменить значение предделителя и периода
  TIM2_TimeBaseInit(TIM2_Prescaler_128, TIM2_CounterMode_Up, TIM2_50Hz_FREQ);
  // разрешаем прерывания
  TIM2_ITConfig(TIM2_IT_Update, ENABLE);
  // включаем TIM2
  // TIM2_Cmd(ENABLE);
}

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
 // Значение регистра ЦАП отправляется во время переполнения TIM4 
 DAC_Init(DAC_Channel_1, DAC_Trigger_T4_TRGO, DAC_OutputBuffer_Enable);
 // Включение ЦАП
 DAC_Cmd(DAC_Channel_1, ENABLE);
}


// Инициализация кнопки
void Button_Initialization(void)
{
	GPIO_Init( Button_Port, Button_Pin, GPIO_Mode_In_PU_No_IT );
}