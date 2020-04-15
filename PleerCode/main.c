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

#include "stm8l15x_gpio.h"

#include "stm8l15x_dac.h"
#include "stm8l15x_tim4.h"

#include "pffconf.h"
#include "pff.h"
#include "diskio.h"

#include "Control.h"
#include "Initialization.h"

#include "string.h"


PLEER_MODE mode;

#define BUFFER_SIZE 500

uint8_t Buffer1[BUFFER_SIZE];
uint8_t Buffer2[BUFFER_SIZE];

BUFFER_STATUS Buf1_Status;
BUFFER_STATUS Buf2_Status;

uint16_t ByteinBuffer=0;


#define EXTI3_vector                         13 /* IRQ No. in STM8 manual: 11, взято из iostm8l152c6.h */
#pragma vector=EXTI3_vector
__interrupt void ButtonPress(void)
{
  if ( GPIO_ReadInputDataBit(Button_Port, Button_Pin) == RESET )
  {
    // Проверка на дребезг
    Delay(2000);
    if ( GPIO_ReadInputDataBit(Button_Port, Button_Pin) == RESET )
    {
      // Определение долгого или короткого нажатия
      Delay(5000);
      if ( GPIO_ReadInputDataBit(Button_Port, Button_Pin) == RESET )
      {
        // Долгое нажатие
        if ( mode != SelectMode )
          mode=SelectMode;
      }
      else
      {
        // Короткое нажатие
        if ( mode != PlayMode ) 
          mode=PlayMode;
        else 
          mode=RewindMode;  
      }
    }  
  }
  EXTI_ClearITPendingBit(Button_IT_Flag);
}


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
      if ( ByteinBuffer < BUFFER_SIZE ){
          DAC_SetChannel1Data(DAC_Align_12b_R, Buffer1[ByteinBuffer++]);
        }else{
          ByteinBuffer=0;
          Buf1_Status=BUF_WAS_READ;}
    }else
    if ( Buf2_Status == BUF_WAS_WRITTEN )
    {
      if ( ByteinBuffer < BUFFER_SIZE ){
          DAC_SetChannel1Data(DAC_Align_12b_R, Buffer2[ByteinBuffer++]);
        }else{
          ByteinBuffer=0;
          Buf2_Status=BUF_WAS_READ;}
    };

  TIM4_ClearFlag(TIM4_FLAG_Update);
};


  
FATFS fs; //объявление объекта FATFS
FILINFO fno;
DIR dir;
FRESULT mountRes; //переменная для возвращаемых значений
FRESULT odirRes; //переменная для возвращаемых значений
FRESULT sfileRes; //переменная для возвращаемых значений
FRESULT ofileRes; //переменная для возвращаемых значений
FRESULT rfileRes; //переменная для возвращаемых значений
FRESULT res4; //переменная для возвращаемых значений
UINT readedBytes;


uint8_t resp1=0;
uint8_t resp11=0;
uint8_t resp12=0;


char filepath[25];


int main( void )
{
  /* Порядок инициализации лучше не менять, т.к. ЦАП использует сигнал с TIM4*/
  // Инициализация TIM4 без включения модуля
  TIM4_Initialization();
  // Инициализация ЦАП с включением модуля
  DAC_Initialization();
  // Инициализация кнопки с прерыванием
  Button_Initialization();
  // Инициализация ТIM1 в режиме энкодера без включения модуля
  //TIM1_Initialization();

  asm("rim"); //глобально разрешаем прерывание

  // Монтирование диска
  mountRes = pf_mount(&fs);
  if ( mountRes != FR_OK )  //проверим результат и повторим ещё один раз перед завершением программы
      {
        pf_mount(NULL);
        mountRes=pf_mount(&fs);
        if ( mountRes != FR_OK )
        return mountRes;
      }
 
  // Отркываем директорию Music/ и считаем количество файлов в ней
  odirRes=pf_opendir(&dir, DIR_NAME);
  CountFiles (&dir, &MaxNumber);
  // Открываем первый файл
  // res2 = pf_readdir(&dir, &fno);
  // FileNumber++;

  // mode=PlayMode;
  // FileNumber=5;
  // OffsetDirect=REWIND_FORWARD;
  // FileOffset=3000000;

   mode=SelectMode;
  while(1)
  {
    while( mode == SelectMode )
    {
      // Включение TIM1 и проверить включение, если можно
      ////////////////////////////////////////////////////////////
    };
    
    if ( mode == PlayMode )
    {
      asm("sim"); //глобально запрещаем прерывание
      // Отключение TIM1
      /////////////////////////////////////////////////////////////
      // Переносим указатель на нужный файл и открываем его
      sfileRes=SelectFile(&dir, &fno, filepath,FileNumber);
      ofileRes=pf_open(filepath);
      if ( ofileRes != FR_OK )
        return ofileRes;
      //Перематываем файл
      RewindFile(&fs, OffsetDirect, FileOffset);  
      // Предварительно заполняем Buffer1
      rfileRes=pf_read(Buffer1, BUFFER_SIZE, &readedBytes);
      if ( rfileRes != FR_OK )
        return rfileRes;
      Buf1_Status=BUF_WAS_WRITTEN;
      Buf2_Status=BUF_WAS_READ;
      // Включение TIM4
      asm("rim"); //глобально разрешаем прерывание
      TIM4_Cmd(ENABLE);
      while( (mode == PlayMode) && (readedBytes == BUFFER_SIZE) && (rfileRes == FR_OK) )
      {
        if ( Buf1_Status == BUF_WAS_WRITTEN && Buf2_Status == BUF_WAS_READ )
        {
          rfileRes=pf_read(Buffer2, BUFFER_SIZE, &readedBytes);
          Buf2_Status=BUF_WAS_WRITTEN;
        }else
          if ( Buf2_Status == BUF_WAS_WRITTEN && Buf1_Status == BUF_WAS_READ )
        {
          rfileRes=pf_read(Buffer1, BUFFER_SIZE, &readedBytes);
          Buf1_Status=BUF_WAS_WRITTEN;
        };
      };
      // Отключение TIM4
      TIM4_Cmd(DISABLE);

      if ( rfileRes != FR_OK )
      return rfileRes;
      // Если количество прочтённых байтов меньше BUFFER_SIZE, след-но файл закончился и переходим в режим SelectMode
      if ( readedBytes != BUFFER_SIZE )
        mode=SelectMode;
    };

    if ( mode == RewindMode )
    {
      // Включение TIM1 и проверить включение, если можно
      ////////////////////////////////////////////////////////////
    }
  }

  
   
  
  // res2=ChangeFile( &dir, &fno, filepath, PREVIOUS_FILE );
  // res2=ChangeFile( &dir, &fno, filepath, NEXT_FILE );
  //  res2=ChangeFile( &dir, &fno, filepath, NEXT_FILE);
  //   res2=ChangeFile(  &dir, &fno, filepath, NEXT_FILE);
 
  // res3=pf_open(filepath);

  // RewindFile( &fs, REWIND_FORWARD, 2700000 );
  // rewind( &fs, REWIND_BACK, 1000000 );

  // res4=pf_read(Buffer1, BUFFER_SIZE, &br);
  // Buf1_Status=BUF_WAS_WRITTEN;
  // Buf2_Status=BUF_WAS_READ;


// // Включение TIM4
//   TIM4_Cmd(ENABLE);
 
  
//  while(1){

//    if ( Buf1_Status == BUF_WAS_WRITTEN && Buf2_Status == BUF_WAS_READ )
//    {
//      res2=pf_read(Buffer2, BUFFER_SIZE, &br);
//      Buf2_Status=BUF_WAS_WRITTEN;
//    }else
//      if ( Buf2_Status == BUF_WAS_WRITTEN && Buf1_Status == BUF_WAS_READ )
//    {
//      res2=pf_read(Buffer1, BUFFER_SIZE, &br);
//      Buf1_Status=BUF_WAS_WRITTEN;
//    };



return 0;
}