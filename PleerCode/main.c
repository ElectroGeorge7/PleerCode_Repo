#include "stm8l15x.h"      

#include "stm8l15x_spi.h" 
#include "stm8l15x_gpio.h"
#include "stm8l15x_dac.h"
#include "stm8l15x_tim1.h"
#include "stm8l15x_tim2.h"
#include "stm8l15x_tim4.h"

#include "pffconf.h"
#include "pff.h"
#include "diskio.h"

#include "Control.h"
#include "Initialization.h"

#include "string.h"

static PLEER_MODE mode;

#define BUFFER_SIZE 500

static uint8_t Buffer1[BUFFER_SIZE];
static uint8_t Buffer2[BUFFER_SIZE];

BUFFER_STATUS Buf1_Status;
BUFFER_STATUS Buf2_Status;

static uint16_t ByteinBuffer=0;

#define TIM2_OVR_UIF_vector                  21 /* IRQ No. in STM8 manual: 19 , взято из iostm8l152c6.h */
#pragma vector=TIM2_OVR_UIF_vector  
__interrupt void ButtonPress(void)
{
  static uint8_t pressTime=0;
  PRESS_STATUS pressStat=NOT_PRESS;

  if ( GPIO_ReadInputDataBit(Button_Port, Button_Pin) == RESET )
  {
     if ( pressTime < MAX_LONG_TIME ) //Если кнопка нажата очень долгое время (~5с), ничего не делаем
      pressTime++;
  }
  else if ( pressTime > 0 )
  {
    pressStat=(pressTime > MAX_SHORT_TIME) ? LONG_PRESS : SHORT_PRESS;
    pressTime=0;
  }

  // Короткое нажатие
  if ( pressStat == SHORT_PRESS )
    mode=(mode != PlayMode) ? PlayMode : RewindMode; 

  // Долгое нажатие
   if ( pressStat == LONG_PRESS )
    mode=SelectMode; 

  TIM2_ClearITPendingBit(TIM2_IT_Update);
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
FRESULT odirRes; 
FRESULT sfileRes; 
FRESULT ofileRes; 
FRESULT rfileRes; 
FRESULT rewfileRes; 
UINT readedBytes;

// символьный массив для расположения имени файла
char filepath[25];

int main( void )
{
  uint16_t MaxNumber=0;
  uint16_t FileNumber=1;
  DWORD filePointer=0;

  /* Порядок инициализации лучше не менять, т.к. ЦАП использует сигнал с TIM4*/
  // Инициализация ТIM1 в режиме энкодера без включения модуля
  TIM1_Initialization();
  // Инициализация TIM2 без включения модуля
  TIM2_Initialization();
  // Инициализация TIM4 без включения модуля
  TIM4_Initialization();
  // Инициализация ЦАП с включением модуля
  DAC_Initialization();
  // Инициализация кнопки с прерыванием
  Button_Initialization();

  asm("rim"); //глобально разрешаем прерывание

  // Монтирование диска
  mountRes = pf_mount(&fs);
  if ( mountRes != FR_OK )  //проверим результат и повторим ещё один раз перед завершением программы
      {
        Delay(1000);
        pf_mount(NULL);
        mountRes=pf_mount(&fs);
        if ( mountRes != FR_OK )
        return mountRes;
      }
 
  // Отркываем директорию Music/ и считаем количество файлов в ней
  odirRes=pf_opendir(&dir, DIR_NAME);
  CountFiles (&dir, &MaxNumber);


  mode=SelectMode;
  while(1)
  {
    if ( mode == SelectMode )
    {
      // Включаем таймер TIM2 на сканирование кнопки
      TIM2_Cmd(ENABLE);
      // Включение и сброс TIM1 
      TIM1_SetCounter(0);
      TIM1_Cmd(ENABLE);
      // Цикл, в котором происходит вращение энкодера
      while( mode == SelectMode );
      // Отключение TIM1 и TIM2
      TIM1_Cmd(DISABLE);
      TIM2_Cmd(DISABLE);
      //Глобально запрещаем прерывания
      asm("sim"); 
      // Изменяем номер файла
      SelectFile( TIM1_GetCounter(), &FileNumber, MaxNumber );
      // Переносим указатель на нужный файл по его номеру и открываем его
      sfileRes=ChangeFile(&dir, &fno, filepath, 1);   
      sfileRes=ChangeFile(&dir, &fno, filepath, FileNumber);
      ofileRes=pf_open(filepath);
      if ( ofileRes != FR_OK )
        return ofileRes;   
      //Глобально разрешаем прерывания
      asm("rim"); 
    };

    if ( mode == PlayMode )
    {
      // Предварительно заполняем Buffer1
      rfileRes=pf_read(Buffer1, BUFFER_SIZE, &readedBytes);
      if ( rfileRes != FR_OK )
        return rfileRes;
      Buf1_Status=BUF_WAS_WRITTEN;
      Buf2_Status=BUF_WAS_READ;
      // Включение TIM4 и TIM2
      TIM4_Cmd(ENABLE);
      TIM2_Cmd(ENABLE);
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
      // Отключение TIM4 и TIM2
      TIM4_Cmd(DISABLE);
      TIM2_Cmd(DISABLE);

      if ( rfileRes != FR_OK )
        return rfileRes;
      // Если количество прочтённых байтов меньше BUFFER_SIZE, след-но файл закончился и переходим в режим SelectMode
      if ( readedBytes != BUFFER_SIZE )
        mode=SelectMode;
    };

    if ( mode == RewindMode )
    { 
      // Включаем таймер TIM2 на сканирование кнопки
      TIM2_Cmd(ENABLE);
      // Включение и сброс TIM1 
      TIM1_SetCounter(0);
      TIM1_Cmd(ENABLE);
      // Цикл, в котором происходит вращение энкодера
      while( mode == RewindMode );
       // Отключение TIM1 и TIM2
      TIM1_Cmd(DISABLE);
      TIM2_Cmd(DISABLE);
      //Глобально запрещаем прерывания
      asm("sim"); 
      // Сохраняем нынешнее значение указателя
      filePointer=fs.fptr;
      sfileRes=ChangeFile(&dir, &fno, filepath, 1);  
      //Перематываем файл
      rewfileRes=RewindFile(&fs, filePointer, TIM1_GetCounter());   
       if ( rewfileRes != FR_OK )
        return rewfileRes;
      //Глобально разрешаем прерывания
       asm("rim"); 
    }

  }

return 0;
}