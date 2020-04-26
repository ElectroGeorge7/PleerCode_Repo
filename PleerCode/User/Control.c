#include "Control.h"
#include "string.h"


void Delay(__IO uint16_t nCount)    //пока что оставлю программную задержку, если останется время сделаю нормально, в инете куча примеров
{
  /* Decrement nCount value */
  while (nCount != 0)
  {
    nCount--;
  }
}

FRESULT CountFiles (DIR* dir, uint16_t* MaxNum)
{
    FRESULT res;
    FILINFO fno;
	// если директория не открыта, то откроем её
	if ( dir->fn == NULL)    
	{
		res=pf_opendir(dir, DIR_NAME);
		if( res != FR_OK )
		return res;
	};

        for (;;) {
            res = pf_readdir(dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0) 
              break;
            else 
              (*MaxNum)++;
        }
	// после прочтения всех файлов, указатель стоит на конечной позиции, чтобы его сбросить, нужно заново открыть директорию
	res=pf_opendir(dir, DIR_NAME);
    return res;
}

void SelectFile(uint16_t cntrVal, uint16_t* FileNum, uint16_t MaxNum)
{
  if ( cntrVal == 0)
	  return;

  // Начальное значение равно 0, смещение в обратом направлении определяется значениями менее 0xFFFF(до 0x8000)
  // Смещение в прямом направлеии - значением от 0 до 0х8000 
  if ( cntrVal < 0x8000 )
    {
      // Смещение в прямом направлении
      cntrVal=(uint16_t)(cntrVal >> 2); //сдвиг на 2 разряда влево аналогичен делению на 4 (один щелчок-4 импульса)
      if ( ((*FileNum)+cntrVal) > MaxNum )
        *FileNum=MaxNum;
      else
        *FileNum+=cntrVal;
    }
  else 
    {
      // Смещение в обратном направлении
      // дополнительно прибавляю 1, потому что из 4 импульсов при щелчке, один импульс переводит счётчик из 0 в 0xFFFF
      cntrVal=(uint16_t)((0xFFFF-cntrVal)+1);  
      cntrVal=(uint16_t)(cntrVal >> 2);
      if ( cntrVal >= (*FileNum) )
        *FileNum=1; // минимальное значение 1, что соответствует первому файлу
      else
        *FileNum-=cntrVal;
    }
}

FRESULT ChangeFile(DIR* dir, FILINFO* fno, char* filepath, uint16_t FileNum)
{
  FRESULT res;

  uint16_t i;
  uint8_t Count=7;
  
  do
  {
    res=pf_opendir(dir, DIR_NAME);
 //   if( res != FR_OK )
 // 	 return res;	 

   for(i=0; i<FileNum; i++)
	    res=pf_readdir(dir, fno);
  } while( (res!=FR_OK) && Count-- );
  
  if( res != FR_OK )
          return res;
  
  for (i=0; i<25; i++)
    filepath[i]=NULL;

  strcat(filepath, DIR_NAME);
  strcat(filepath,"/");
  strcat(filepath,fno->fname);

  return res;
};


FRESULT RewindFile(FATFS* fs, DWORD filePointer, uint16_t cntrVal)
{
  FRESULT res;
  DWORD offset=0;
  
  // Начальное значение равно 0, смещение в обратом направлении определяется значениями менее 0xFFFF(до 0x8000)
  // Смещение в прямом направлеии - значением от 0 до 0х8000 
  if ( cntrVal < 0x8000 )
    {
      // Долго мучался, тип cntrVal uint16_t и поэтому cntrVal нельзя умножать на большие числа (макс её значение 65536)
      // Чтобы можно было работать с бОльшими числами нужно перенести значение cntrVal в тип uint32_t(DWORD)
      offset=cntrVal;
      // Смещение в прямом направлении
      offset=(uint32_t)(offset << REWIND_FACTOR); //сдвиг на 2 разряда вправо аналогичен умножению на 2^(REWIND_FACTOR)
      if ( offset < (fs->fsize-filePointer) )
          res = pf_lseek(offset+filePointer);
      else
        res = pf_lseek(fs->fsize); 
    }
  else 
    {
      // Смещение в обратном направлении
      // дополнительно прибавляю 1, потому что из 4 импульсов при щелчке, один импульс переводит счётчик из 0 в 0xFFFF
      cntrVal=(uint16_t)((0xFFFF-cntrVal)+1); 
      offset=cntrVal;
      offset=(uint32_t)(offset << REWIND_FACTOR); //сдвиг на 2 разряда вправо аналогичен умножению на 2^(REWIND_FACTOR)
      if ( offset < filePointer )
        res = pf_lseek(filePointer-offset);
      else 
        res = pf_lseek(0);
    }

   return res;
}

