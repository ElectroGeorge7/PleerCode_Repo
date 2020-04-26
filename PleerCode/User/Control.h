#include "stm8l15x.h"
#include "pffconf.h"
#include "pff.h"
#include "diskio.h"

typedef enum
{
  SelectMode,
  PlayMode,
  RewindMode
} PLEER_MODE;

typedef enum
{
  NOT_PRESS,
  SHORT_PRESS,
  LONG_PRESS
} PRESS_STATUS;

typedef enum
{
  BUF_WAS_READ = ((uint8_t)0x00), 
  BUF_WAS_WRITTEN  = ((uint8_t)0x01), 
  BUF_ERROR = ((uint8_t)0xFF)
} BUFFER_STATUS;


#define DIR_NAME "Music"
#define REWIND_FACTOR 15


void Delay(uint16_t nCount);
FRESULT CountFiles (DIR* dir, uint16_t* MaxNum);
void SelectFile(uint16_t cntrVal, uint16_t* FileNum, uint16_t MaxNum);
FRESULT ChangeFile (DIR* dir, FILINFO* fno, char* filepath, uint16_t FileNum);
FRESULT RewindFile(FATFS* fs,  DWORD filePointer, uint16_t cntrVal);
