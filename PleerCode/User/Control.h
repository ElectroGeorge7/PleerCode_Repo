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
  BUF_WAS_READ = ((uint8_t)0x00), 
  BUF_WAS_WRITTEN  = ((uint8_t)0x01), 
  BUF_ERROR = ((uint8_t)0xFF)
} BUFFER_STATUS;

typedef enum
{
  REWIND_FORWARD = ((uint8_t)0x00), 
  REWIND_BACK  = ((uint8_t)0x01)
}REWIND_DIRECT;

#define DIR_NAME "Music"

// Глобальные перменные
extern uint16_t       MaxNumber;
extern uint16_t       FileNumber;
extern uint32_t       FileOffset;
extern REWIND_DIRECT  OffsetDirect;

void Delay(uint16_t nCount);
FRESULT CountFiles (DIR* dir, uint16_t* MaxNum);
FRESULT SelectFile (DIR* dir, FILINFO* fno, char* filepath, uint16_t FileNum);
FRESULT RewindFile (FATFS* fs, REWIND_DIRECT direct, DWORD offset);
