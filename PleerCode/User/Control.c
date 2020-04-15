#include "Control.h"
#include "string.h"

uint16_t MaxNumber=0;
uint16_t FileNumber=1;
uint32_t FileOffset=0;
REWIND_DIRECT  OffsetDirect=REWIND_FORWARD;

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

FRESULT SelectFile(DIR* dir, FILINFO* fno, char* filepath, uint16_t FileNum)
{
  FRESULT res;

  uint16_t i;
  uint8_t Count=7;

  while( (res!=FR_OK) && Count-- )
  {
  res=pf_opendir(dir, DIR_NAME);
//   if( res != FR_OK )
// 	 return res;	 

  for(i=0; i<FileNum; i++)
	 res=pf_readdir(dir, fno);
  };
  
  if( res != FR_OK )
          return res;

  for (i=0; i<25; i++)
    filepath[i]=NULL;

  strcat(filepath, DIR_NAME);
  strcat(filepath,"/");
  strcat(filepath,fno->fname);

  return res;

};


FRESULT RewindFile(FATFS* fs, REWIND_DIRECT direct, DWORD offset )
{
  FRESULT res;

  if ( offset == 0)
	return FR_OK;

  if ( direct == REWIND_FORWARD )
    if ( (fs->fptr+offset) < fs->fsize )
      res = pf_lseek(fs->fptr+offset);
    else
      res = pf_lseek(fs->fsize);

  if ( direct == REWIND_BACK )
    if ( offset < fs->fptr )
      res = pf_lseek(fs->fptr-offset);
    else 
      res = pf_lseek(0);
   
   return res;
}

