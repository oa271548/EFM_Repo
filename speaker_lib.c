//////////////////////////////////////////////////////////////////////
// File - speaker_lib.c
//
// Library for accessing the speaker directly on the motherboard.
// Code was generated by WinDriver Wizard.
// Application uses WinDriver to access the hardware.
// 
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include <winioctl.h>
#include "windrvr.h"
#include "speaker_lib.h"
#include "bits.h"

// this string is set to an error message, if one occurs
CHAR SPEAKER_ErrorString[1024];

// internal function used by SPEAKER_Open()
void SPEAKER_SetCardElements(SPEAKER_HANDLE hSPEAKER);

BOOL SPEAKER_Open (SPEAKER_HANDLE *phSPEAKER)
{
    SPEAKER_HANDLE hSPEAKER = (SPEAKER_HANDLE) malloc (sizeof (SPEAKER_STRUCT));

    WD_VERSION ver;

    *phSPEAKER = NULL;
    SPEAKER_ErrorString[0] = '\0';
    BZERO(*hSPEAKER);

    hSPEAKER->cardReg.hCard = 0;
    hSPEAKER->hWD = WD_Open();

    // check if handle valid & version OK
    if (hSPEAKER->hWD==INVALID_HANDLE_VALUE)
    {
        sprintf_s(SPEAKER_ErrorString, 1024, "Failed opening WinDriver device\n");
        goto Exit;
    }

    BZERO(ver);
    WD_Version(hSPEAKER->hWD,&ver);
    if (ver.dwVer<WD_VER)
    {
        sprintf_s(SPEAKER_ErrorString, 1024, "Incorrect WinDriver version\n");
        goto Exit;
    }

    SPEAKER_SetCardElements(hSPEAKER);
    hSPEAKER->cardReg.fCheckLockOnly = FALSE;
    WD_CardRegister (hSPEAKER->hWD, &hSPEAKER->cardReg);
    if (hSPEAKER->cardReg.hCard==0)
    {
        sprintf_s(SPEAKER_ErrorString, 1024, "Failed locking device\n");
        goto Exit;
    }

    // Open finished OK
    *phSPEAKER = hSPEAKER;
    return TRUE;

Exit:
    // Error during Open
    if (hSPEAKER->cardReg.hCard) 
        WD_CardUnregister(hSPEAKER->hWD, &hSPEAKER->cardReg);
    if (hSPEAKER->hWD!=INVALID_HANDLE_VALUE)
        WD_Close(hSPEAKER->hWD);
    free (hSPEAKER);
    return FALSE;
}

void SPEAKER_Close(SPEAKER_HANDLE hSPEAKER)
{
    // unregister card
    if (hSPEAKER->cardReg.hCard) 
        WD_CardUnregister(hSPEAKER->hWD, &hSPEAKER->cardReg);

    // close WinDriver
    WD_Close(hSPEAKER->hWD);

    free (hSPEAKER);
}

void SPEAKER_SetCardElements(SPEAKER_HANDLE hSPEAKER)
{
    int i=0;
    WD_ITEMS* pItem;
    hSPEAKER->cardReg.Card.dwItems = SPEAKER_ITEMS;
    pItem = &hSPEAKER->cardReg.Card.Item[0];

    // SPEAKER IO range
    pItem[SPEAKER_IO_42].item = ITEM_IO;
    pItem[SPEAKER_IO_42].fNotSharable = FALSE;
    pItem[SPEAKER_IO_42].I.IO.dwAddr = SPEAKER_IO_ADDR42;
    pItem[SPEAKER_IO_42].I.IO.dwBytes = 1;
    pItem[SPEAKER_IO_43].item = ITEM_IO;
    pItem[SPEAKER_IO_43].fNotSharable = FALSE;
    pItem[SPEAKER_IO_43].I.IO.dwAddr = SPEAKER_IO_ADDR43;
    pItem[SPEAKER_IO_43].I.IO.dwBytes = 1;
    pItem[SPEAKER_IO_61].item = ITEM_IO;
    pItem[SPEAKER_IO_61].fNotSharable = FALSE;
    pItem[SPEAKER_IO_61].I.IO.dwAddr = SPEAKER_IO_ADDR61;
    pItem[SPEAKER_IO_61].I.IO.dwBytes = 1;
}

void SPEAKER_WriteCtrl (SPEAKER_HANDLE hSPEAKER, BYTE data)
{
    WD_TRANSFER trans;

    BZERO(trans);
    trans.cmdTrans = WP_BYTE;
    trans.dwPort = SPEAKER_IO_ADDR61;
    trans.Data.Byte = data;
    WD_Transfer(hSPEAKER->hWD, &trans);
}

BYTE SPEAKER_ReadCtrl (SPEAKER_HANDLE hSPEAKER)
{
    WD_TRANSFER trans;

    BZERO(trans);
    trans.cmdTrans = RP_BYTE;
    trans.dwPort = SPEAKER_IO_ADDR61;
    WD_Transfer(hSPEAKER->hWD, &trans);
    return trans.Data.Byte;
}

void SPEAKER_WriteTimerData (SPEAKER_HANDLE hSPEAKER, BYTE data)
{
    WD_TRANSFER trans;

    BZERO(trans);
    trans.cmdTrans = WP_BYTE;
    trans.dwPort = SPEAKER_IO_ADDR42;
    trans.Data.Byte = data;
    WD_Transfer(hSPEAKER->hWD, &trans);
}

void SPEAKER_WriteTimerCtrl (SPEAKER_HANDLE hSPEAKER, BYTE data)
{
    WD_TRANSFER trans;

    BZERO(trans);
    trans.cmdTrans = WP_BYTE;
    trans.dwPort = SPEAKER_IO_ADDR43;
    trans.Data.Byte = data;
    WD_Transfer(hSPEAKER->hWD, &trans);
}

/*void SPEAKER_Tone (SPEAKER_HANDLE hSPEAKER, DWORD dwHz, DWORD dwMilli)
{
    DWORD dwDevisor = 1190000 / dwHz;
    BYTE bCtrl;
    WD_SLEEP sleep;
    
    SPEAKER_WriteTimerCtrl(hSPEAKER, 0xb6);
    SPEAKER_WriteTimerData(hSPEAKER, (BYTE) (dwDevisor & 0xff));
    SPEAKER_WriteTimerData(hSPEAKER, (BYTE) ((dwDevisor >> 8) & 0xff));

    bCtrl = SPEAKER_ReadCtrl(hSPEAKER);
    SPEAKER_WriteCtrl(hSPEAKER, (BYTE) (bCtrl | (BIT0 | BIT1)));
    BZERO(sleep);
    sleep.dwMicroSeconds = dwMilli * 1000 ; // 2 secounds
    WD_Sleep(hSPEAKER->hWD, &sleep);
    SPEAKER_WriteCtrl(hSPEAKER, (BYTE) (bCtrl & ~(BIT0 | BIT1)));
}*/

void SPEAKER_ToneOn(SPEAKER_HANDLE hSPEAKER, DWORD dwHz)
{
    DWORD dwDevisor;
	BYTE bCtrl;
    
	if (dwHz==0) dwHz = 1;
	dwDevisor = 1190000 / dwHz;
    
    SPEAKER_WriteTimerCtrl(hSPEAKER, 0xb6);
    SPEAKER_WriteTimerData(hSPEAKER, (BYTE) (dwDevisor & 0xff));
    SPEAKER_WriteTimerData(hSPEAKER, (BYTE) ((dwDevisor >> 8) & 0xff));

    bCtrl = SPEAKER_ReadCtrl(hSPEAKER);
    SPEAKER_WriteCtrl(hSPEAKER, (BYTE) (bCtrl | (BIT0 | BIT1)));
}

void SPEAKER_ToneOff(SPEAKER_HANDLE hSPEAKER)
{
    BYTE bCtrl = SPEAKER_ReadCtrl(hSPEAKER);
    SPEAKER_WriteCtrl(hSPEAKER, (BYTE) (bCtrl & ~(BIT0 | BIT1)));
}

