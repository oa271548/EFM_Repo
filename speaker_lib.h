//////////////////////////////////////////////////////////////////////
// File - speaker_lib.h
//
// Library for accessing the speaker directly on the motherboard.
// Code was generated by WinDriver Wizard.
// Application uses WinDriver to access the hardware.
// 
//////////////////////////////////////////////////////////////////////

#ifndef _SPEAKER_LIB_H_
#define _SPEAKER_LIB_H_

// 'windrvr.h' is located in the WinDriver include directory,
#include "windrvr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    SPEAKER_IO_42 = 0,
    SPEAKER_IO_43 = 1,
    SPEAKER_IO_61 = 2,
    SPEAKER_ITEMS = 3
} SPEAKER_ADDR;

// IO ranges definitions
enum { SPEAKER_IO_ADDR42 = 0x42 };
enum { SPEAKER_IO_ADDR43 = 0x43 };
enum { SPEAKER_IO_ADDR61 = 0x61 };

typedef struct SPEAKER_STRUCT *SPEAKER_HANDLE;

typedef struct SPEAKER_STRUCT
{
    HANDLE hWD;
    WD_CARD_REGISTER cardReg;
} SPEAKER_STRUCT;

BOOL SPEAKER_Open (SPEAKER_HANDLE *phSPEAKER);
void SPEAKER_Close(SPEAKER_HANDLE hSPEAKER);

void SPEAKER_Tone (SPEAKER_HANDLE hSPEAKER, DWORD dwHz, DWORD dwMilli);
void SPEAKER_ToneOn(SPEAKER_HANDLE hSPEAKER, DWORD dwHz);
void SPEAKER_ToneOff(SPEAKER_HANDLE hSPEAKER);

// this string is set to an error message, if one occurs
extern CHAR SPEAKER_ErrorString[];


#ifdef __cplusplus
}
#endif

#endif
