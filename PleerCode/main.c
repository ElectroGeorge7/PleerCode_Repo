
//#include "iostm8l152c6.h" // этот файл подключать не надо, потому что происходит 
                            //переопределение дефайнов при совместном 
                            //использовании iostm8l152c6.h и stm8l15x.h
                            //в stm8l15x.h есть все необходимые дефайны
                            //(кроме векторов прерываний, из я не нашёл)

#include "stm8l15x.h"       //в этом файле описаны основные типы данных, 
                            //указаны настройки компиляторов, 
                            //значения регистров после сброса, маски битов в регистрах,
                            //и самое главное-объявляются новые типы данных 
                            //в виде структур по каждому периферийному элементу, 
                            //которые потом ипользуются в периферийной библотеке от STM
 
#include "stm8l15x_spi.h"   //объявляются все функции для работы с SPI интерфейсом
                            //в stm8l15x_spi.с эти функции определяются, 
                            //описаны типы данных в виде перечисления(enum) 
                            //под разные режимы работы SPI

/* assert_param используется в файле stm8l15x_spi.с 
При отладке программы очень удобно для вывода сообщений об той или иной ситуации, 
иметь универсальную функцию, которая позволяла бы выводить сообщение об ошибке, 
номер строки и файл в котором эта ошибка возникла.
Для этого, в библиотеке периферии от ST, используется assert_param.
Полное описание на сайте   http://badembed.ru/assert_param-stm32/   */

//сделаем так: скопируем все необходимые файлы из примера с работой с SD картой
//для STM8_EVAL в папку нашего проекта, а дальше будем всё приводить к нужному виду
//для работы с STM8l152C6

#include "stm8_eval_spi_sd.h"
#include "stm8l15x_spi.h"
#include "stm8l15x_gpio.h"
#include "stm8l15x_clk.h"
#include "stm8l15x_syscfg.h" 
#include "stm8l15x_dac.h"
#include "stm8l15x_tim4.h"

#include "pffconf.h"
#include "pff.h"
#include "diskio.h"


void Delay(__IO uint16_t nCount)    //пока что оставлю программную задержку, если останется время сделаю нормально, в инете куча примеров
{
  /* Decrement nCount value */
  while (nCount != 0)
  {
    nCount--;
  }
}

typedef enum
{
  BUF_WAS_READ = ((uint8_t)0x00), 
  BUF_WAS_WRITTEN  = ((uint8_t)0x01), 
  BUF_ERROR = ((uint8_t)0xFF)
}BUFFER_STATUS;

#define BUFFER_SIZE 500

uint8_t Buffer1[BUFFER_SIZE];
uint8_t Buffer2[BUFFER_SIZE];

BUFFER_STATUS Buf1_Status;
BUFFER_STATUS Buf2_Status;

uint8_t* buffer;

//uint8_t temp=0xff;
uint16_t i=0;
uint16_t j=0;


//обработчик прерывания для таймера
#define TIM4_UIF_vector                      27 /* IRQ No. in STM8 manual: 25 , взято из iostm8l152c6.h*/ 



//перед объявлением обработчика прерывания необходимо указать номер вектора прерывания,
//который можно найти в конце iostm8l152c6.h, либо номер из даташита +2
#pragma vector=TIM4_UIF_vector                                                                                         
__interrupt void SetDAC_TIM4IT (void)
//во время обработки прерывания таймер продолжает считать, поэтому при выходе из обработчика  
//в счётчике уже имеется значение, которое зависит от времени обработки прерывания  
//(в данном случае примерно 0x93), при уменьшении значения регистра TIM4_ARR таймера во время обработки 
//прерывания может возникнуть новое прерывание по совпадению
{  
  if ( Buf1_Status == BUF_WAS_WRITTEN )
    { 
      if ( i < BUFFER_SIZE ){
          DAC_SoftwareTriggerCmd(DAC_Channel_1, ENABLE);
          DAC_SetChannel1Data(DAC_Align_12b_R, Buffer1[i++]);
        }else{
          i=0;
          Buf1_Status=BUF_WAS_READ;}
    }else
    if ( Buf2_Status == BUF_WAS_WRITTEN )
    {
      if ( i < BUFFER_SIZE ){
          DAC_SoftwareTriggerCmd(DAC_Channel_1, ENABLE);
          DAC_SetChannel1Data(DAC_Align_12b_R, Buffer2[i++]);
        }else{
          i=0;
          Buf2_Status=BUF_WAS_READ;}
    };

  TIM4_ClearFlag(TIM4_FLAG_Update);
};


  
FATFS fs; //объявление объекта FATFS
FRESULT res; //переменная для возвращаемых значений
FRESULT res1; //переменная для возвращаемых значений
FRESULT res2; //переменная для возвращаемых значений
WORD br;


uint8_t resp1=1;
uint8_t resp11=0;
uint8_t resp12=0;
uint8_t resp13=0;
uint8_t resp14=0;
uint8_t startDataToken1=0;
uint8_t CRC11=0;
uint8_t CRC22=0;
uint8_t ReadStatus=1;

//при частоте после предделителя таймера 4 Мгц
#define TIM4_22kHz_FREQ  181//181  
#define TIM4_44kHz_FREQ  91 

int main( void )
{
  
  Delay(1000);     
  //смонтировать диск
  res = pf_mount(&fs);
  res1=pf_open("BACKINBL.wav");
  res2 = pf_read(Buffer1, BUFFER_SIZE, &br);
  Buf1_Status=BUF_WAS_WRITTEN;
  Buf2_Status=BUF_WAS_READ;


//Установка таймера 4
  CLK_PeripheralClockConfig( CLK_Peripheral_TIM4, ENABLE);
  //будем считать, что общая системная частота равна 16 МГц
  TIM4_TimeBaseInit(TIM4_Prescaler_4, TIM4_22kHz_FREQ);    //при изменении системной частоты изменить значение предделителя
  asm("rim"); //глобально разрешаем прерывание
  TIM4_ITConfig(TIM4_IT_Update, ENABLE); //разрешаем прерывания
  TIM4_Cmd(ENABLE);


//Инициализация ЦАП
 GPIO_Init( GPIOF , GPIO_Pin_0, GPIO_Mode_In_FL_No_IT);  //аналоговый режим (RM0031 стр.121)
 CLK_PeripheralClockConfig(CLK_Peripheral_DAC, ENABLE);
 DAC_Init(DAC_Channel_1, DAC_Trigger_Software, DAC_OutputBuffer_Enable);
 DAC_Cmd(DAC_Channel_1, ENABLE);
 DAC_SoftwareTriggerCmd(DAC_Channel_1, ENABLE);
 DAC_SetChannel1Data(DAC_Align_12b_R, 0x20);
 
  
 while(1){
   if ( Buf1_Status == BUF_WAS_WRITTEN && Buf2_Status == BUF_WAS_READ )
   {
     res2=pf_read(Buffer2, BUFFER_SIZE, &br);
     Buf2_Status=BUF_WAS_WRITTEN;
   }else
     if ( Buf2_Status == BUF_WAS_WRITTEN && Buf1_Status == BUF_WAS_READ )
   {
     res2=pf_read(Buffer1, BUFFER_SIZE, &br);
     Buf1_Status=BUF_WAS_WRITTEN;
   };
   
 }


return 0;
}