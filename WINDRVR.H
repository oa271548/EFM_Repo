/* 
 * W i n D r i v e r    V 3 . 0 4
 * ==============================
 *
 * Header file for Windows-95/NT.
 * This file may not be distributed, It is only for evaluation & development purposes.
 *
 * Web site: http://www.krftech.com/windrv
 * Email:    support@krftech.com
 *
 * (C) KRF-Tech 1997
 */

#ifndef _WINDRVR_H_
#define _WINDRVR_H_

#define WD_VER      304
#define WD_VER_STR  "WinDriver V3.04 KRF-Tech (c) 1997"
static CHAR *WD_VER_MODULE = WD_VER_STR;

typedef enum 
{                   
    CMD_NONE = 0,       // No command
    CMD_END = 1,        // End command

    RP_BYTE = 10,       // Read port byte
    RP_WORD = 11,       // Read port word  
    RP_DWORD = 12,      // Read port dword
    WP_BYTE = 13,       // Write port byte
    WP_WORD = 14,       // Write port word 
    WP_DWORD = 15,      // Write port dword 

    RP_SBYTE = 20,      // Read port string byte
    RP_SWORD = 21,      // Read port string word  
    RP_SDWORD = 22,     // Read port string dword
    WP_SBYTE = 23,      // Write port string byte
    WP_SWORD = 24,      // Write port string word 
    WP_SDWORD = 25,     // Write port string dword 

    RM_BYTE = 30,       // Read memory byte
    RM_WORD = 31,       // Read memory word  
    RM_DWORD = 32,      // Read memory dword
    WM_BYTE = 33,       // Write memory byte
    WM_WORD = 34,       // Write memory word 
    WM_DWORD = 35,      // Write memory dword 

    RM_SBYTE = 40,      // Read memory string byte
    RM_SWORD = 41,      // Read memory string word  
    RM_SDWORD = 42,     // Read memory string dword
    WM_SBYTE = 43,      // Write memory string byte
    WM_SWORD = 44,      // Write memory string word 
    WM_SDWORD = 45      // Write memory string dword 
} WD_TRANSFER_CMD;                                         

enum {WD_DMA_PAGES=256};

enum {DMA_KERNEL_BUFFER_ALLOC = 1}; // the system allocates a contiguous buffer
								// the user doesnt need to supply linear_address
enum {DMA_KBUF_BELOW_16M = 2};  // if DMA_KERNEL_BUFFER_ALLOC if used,
                                // this will make sure it is under 16M
enum {DMA_LARGE_BUFFER   = 4};  // if DMA_LARGE_BUFFER if used,
                                // the maximum number of pages are dwPages, and not
                                // WD_DMA_PAGES. if you lock a user buffer (not a kernel
                                // allocated buffer) that is larger than 1MB, then use this
                                // option, and allocate memory for pages.
typedef struct
{
    PVOID pPhysicalAddr;    // physical address of page
    DWORD dwBytes;          // size of page
} WD_DMA_PAGE;

typedef struct 
{
    DWORD     hDma;             // handle of dma buffer
    PVOID     pUserAddr;        // beginning of buffer
    DWORD     dwBytes;          // size of buffer
    DWORD     dwOptions;        // allocation options:
                                // DMA_KERNEL_BUFFER_ALLOC, DMA_KBUF_BELOW_16M, DMA_LARGE_BUFFER
    DWORD     dwPages;          // number of pages in buffer

    WD_DMA_PAGE Page[WD_DMA_PAGES];
} WD_DMA;

typedef struct 
{
    DWORD            cmdTrans;  // Transfer command WD_TRANSFER_CMD
    DWORD            dwPort;    // io port for transfer or user memory address

    // parameters used for string transfers:
    DWORD            dwBytes;   // for string transfer
    DWORD            fAutoinc;  // transfer from one port/address 
                                // or use incremental range of addresses
    DWORD            dwOptions; // must be 0

    union
    {
        UCHAR            Byte;  // use for byte transfer
        USHORT           Word;  // use for word transfer
        DWORD            Dword; // use for dword transfer

        PVOID            pBuffer;   // use for string transfer
    } Data;       
} WD_TRANSFER;


enum {INTERRUPT_LEVEL_SENSITIVE=1};
enum {INTERRUPT_CMD_COPY=2};

typedef struct 
{                                                        
    DWORD        hInterrupt;    // handle of interrupt
    DWORD        dwInterruptNum;// number of interrupt to install 
    DWORD        fNotSharable;  // is interrupt unshareable
    DWORD        dwOptions;     // interrupt options: INTERRUPT_LEVEL_SENSITIVE, INTERRUPT_CMD_COPY
    WD_TRANSFER  *Cmd;          // commands to do on interrupt
    DWORD        dwCmds;        // number of commands
    DWORD        dwCounter;     // number of interrupts received
    DWORD        dwLost;        // number of interrupts not yet dealt with
    DWORD        fStopped;      // was interrupt disabled during wait
} WD_INTERRUPT;

typedef struct 
{                                                        
    DWORD   dwVer;    
    CHAR    cVer[100];
} WD_VERSION;

enum { LICENSE_DEMO=0x1,   LICENSE_LITE=0x2,  LICENSE_FULL=0x4, 
       LICENSE_IO  =0x8,   LICENSE_MEM =0x10, LICENSE_INT =0x20,
       LICENSE_PCI =0x40,  LICENSE_DMA =0x80, LICENSE_NT  =0x100,
	   LICENSE_95  =0x200, LICENSE_PCI_DUMP=0x1000,
       LICENSE_MSG_GEN =0x2000, LICENSE_MSG_EDU=0x4000, LICENSE_MSG_INT=0x8000 };
typedef struct {
	CHAR        cLicense[100]; // buffer with license string to put
                               // if empty string then get current license setting 
                               // into dwLicense
	DWORD       dwLicense;     // returns license settings: LICENSE_DEMO, LICENSE_LITE etc...
	                           // if put license was unsuccessful (i.e. invalid license)
	                           // then dwLicense will return 0.
} WD_LICENSE;

typedef struct {
    DWORD dwBusType;        // Bus Type: ISA, EISA, PCI, PCMCIA
    DWORD dwBusNum;         // Bus number
    DWORD dwSlotFunc;       // Slot number on Bus
} WD_BUS;

typedef enum { ITEM_NONE=0, ITEM_INTERRUPT=1, ITEM_MEMORY=2, ITEM_IO=3, ITEM_BUS=5} ITEM_TYPE;

typedef struct {

    DWORD item; // ITEM_TYPE
    
    DWORD fNotSharable;

    union 
    {
        struct { // ITEM_MEMORY
            DWORD dwPhysicalAddr;     // physical address on card
            DWORD dwBytes;            // address range
            DWORD dwTransAddr;        // returns the address to pass on to transfer commands
            DWORD dwUserDirectAddr;   // returns the address for direct user read/write
            DWORD dwCpuPhysicalAddr;  // returns the CPU physical address of card
        } Mem;
        struct { // ITEM_IO
            DWORD dwAddr;         // beginning of io address
            DWORD dwBytes;        // io range
        } IO;
        struct { // ITEM_INTERRUPT
            DWORD dwInterrupt;    // number of interrupt to install 
            DWORD dwOptions;      // interrupt options: INTERRUPT_LEVEL_SENSITIVE
            DWORD hInterrupt;     // returns the handle of the interrupt installed
        } Int;
        WD_BUS Bus; // ITEM_BUS
        struct {
            DWORD dw1;
            DWORD dw2;
            DWORD dw3;
			DWORD dw4;
			DWORD dw5;
        } Val;
    } I;
} WD_ITEMS;

enum {WD_CARD_ITEMS=20};
typedef struct {
    DWORD    dwItems;
    WD_ITEMS Item[WD_CARD_ITEMS];
} WD_CARD;

typedef struct {
    WD_CARD Card;           // card to register
    DWORD   fCheckLockOnly; // only check if card is lockable, return hCard=1 if OK
    DWORD   hCard;          // handle of card
} WD_CARD_REGISTER;

enum {WD_PCI_CARDS=30};
typedef struct {
    DWORD dwBus;
    DWORD dwSlot;
	DWORD dwFunction;
} WD_PCI_SLOT;
typedef struct {
    DWORD dwVendorId;
    DWORD dwDeviceId;
} WD_PCI_ID;

typedef struct {
	WD_PCI_ID   searchId;   // if dwVendorId==0 - scan all vendor IDs
							// if dwDeviceId==0 - scan all device IDs
    DWORD dwCards;          // number of cards found
	WD_PCI_ID   cardId[WD_PCI_CARDS];   // VendorID & DeviceID of cards found
    WD_PCI_SLOT cardSlot[WD_PCI_CARDS]; // pci slot info of cards found
} WD_PCI_SCAN_CARDS;

typedef struct {
    WD_PCI_SLOT pciSlot;    // pci slot
    WD_CARD     Card;       // get card parameters for pci slot
} WD_PCI_CARD_INFO;

typedef enum { PCI_ACCESS_OK = 0, PCI_ACCESS_ERROR = 1, PCI_BAD_BUS = 2, PCI_BAD_SLOT = 3} PCI_ACCESS_RESULT;
typedef struct {
	WD_PCI_SLOT pciSlot;    // pci bus, slot and function number
	PVOID       pBuffer;    // buffer for read/write
	DWORD       dwOffset;   // offset in pci configuration space to read/write from
	DWORD       dwBytes;    // bytes to read/write from/to buffer
                            // returns the number of bytes read/wrote
	DWORD       fIsRead;    // if 1 then read pci config, 0 write pci config
	DWORD       dwResult;   // PCI_ACCESS_RESULT
} WD_PCI_CONFIG_DUMP;

static DWORD WinDriverGlobalDW;

#define BZERO(buf) memset(&(buf),0,sizeof(buf))

// Device type 
#define WD_TYPE 38200

#define WD_CTL_CODE(wFuncNum) CTL_CODE( WD_TYPE, wFuncNum, METHOD_NEITHER, FILE_ANY_ACCESS)

#define IOCTL_WD_DMA_LOCK             WD_CTL_CODE(0x901)
#define IOCTL_WD_DMA_UNLOCK           WD_CTL_CODE(0x902)
#define IOCTL_WD_TRANSFER             WD_CTL_CODE(0x903)
#define IOCTL_WD_MULTI_TRANSFER       WD_CTL_CODE(0x904)
#define IOCTL_WD_INT_ENABLE           WD_CTL_CODE(0x907)
#define IOCTL_WD_INT_DISABLE          WD_CTL_CODE(0x908)
#define IOCTL_WD_INT_COUNT            WD_CTL_CODE(0x909)
#define IOCTL_WD_INT_WAIT             WD_CTL_CODE(0x90a)
#define IOCTL_WD_CARD_REGISTER        WD_CTL_CODE(0x90c)
#define IOCTL_WD_CARD_UNREGISTER      WD_CTL_CODE(0x90d)
#define IOCTL_WD_PCI_SCAN_CARDS       WD_CTL_CODE(0x90e)
#define IOCTL_WD_PCI_GET_CARD_INFO    WD_CTL_CODE(0x90f)
#define IOCTL_WD_VERSION              WD_CTL_CODE(0x910)
#define IOCTL_WD_DEBUG                WD_CTL_CODE(0x911)
#define IOCTL_WD_LICENSE              WD_CTL_CODE(0x912)
#define IOCTL_WD_PCI_CONFIG_DUMP      WD_CTL_CODE(0x91a)

// Returns - HANDLE to WinDriver or INVALID_HANDLE_VALUE if error.
#define WD_Open()\
    CreateFile(\
        "\\\\.\\WINDRVR",\
        GENERIC_READ,\
        FILE_SHARE_READ | FILE_SHARE_WRITE,\
        NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL)  

#define WD_Close(h)\
    CloseHandle(h)

#define WD_FUNCTION(wFuncNum, h, pParam, dwSize) \
    DeviceIoControl( h, (DWORD) wFuncNum, \
        (PVOID) pParam, (DWORD) dwSize, NULL, 0, \
        &WinDriverGlobalDW, NULL)

#define WD_Debug(h,Debug)\
    WD_FUNCTION(IOCTL_WD_DEBUG, h, NULL, Debug)
#define WD_Transfer(h,pTransfer)\
    WD_FUNCTION(IOCTL_WD_TRANSFER, h, pTransfer, sizeof (WD_TRANSFER))
#define WD_MultiTransfer(h,pTransferArray,dwNumTransfers)\
    WD_FUNCTION(IOCTL_WD_MULTI_TRANSFER, h, pTransferArray, sizeof (WD_TRANSFER) * dwNumTransfers)
#define WD_DMALock(h,pDma)\
    WD_FUNCTION(IOCTL_WD_DMA_LOCK, h, pDma, sizeof (WD_DMA))
#define WD_DMAUnlock(h,pDma)\
    WD_FUNCTION(IOCTL_WD_DMA_UNLOCK, h, pDma, sizeof (WD_DMA))
#define WD_IntEnable(h,pInterrupt)\
    WD_FUNCTION(IOCTL_WD_INT_ENABLE, h, pInterrupt, sizeof (WD_INTERRUPT))
#define WD_IntDisable(h,pInterrupt)\
    WD_FUNCTION(IOCTL_WD_INT_DISABLE, h, pInterrupt, sizeof (WD_INTERRUPT))
#define WD_IntCount(h,pInterrupt)\
    WD_FUNCTION(IOCTL_WD_INT_COUNT, h, pInterrupt, sizeof (WD_INTERRUPT))
#define WD_IntWait(h,pInterrupt)\
    WD_FUNCTION(IOCTL_WD_INT_WAIT, h, pInterrupt, sizeof (WD_INTERRUPT))
#define WD_CardRegister(h,pCard)\
    WD_FUNCTION(IOCTL_WD_CARD_REGISTER, h, pCard, sizeof (WD_CARD_REGISTER))
#define WD_CardUnregister(h,pCard)\
    WD_FUNCTION(IOCTL_WD_CARD_UNREGISTER, h, pCard, sizeof (WD_CARD_REGISTER))
#define WD_PciScanCards(h,pPciScan)\
    WD_FUNCTION(IOCTL_WD_PCI_SCAN_CARDS, h, pPciScan, sizeof (WD_PCI_SCAN_CARDS))
#define WD_PciGetCardInfo(h,pPciCard)\
    WD_FUNCTION(IOCTL_WD_PCI_GET_CARD_INFO, h, pPciCard, sizeof (WD_PCI_CARD_INFO))
#define WD_PciConfigDump(h,pPciConfigDump)\
    WD_FUNCTION(IOCTL_WD_PCI_CONFIG_DUMP, h, pPciConfigDump, sizeof (WD_PCI_CONFIG_DUMP))
#define WD_Version(h,pVerInfo)\
    WD_FUNCTION(IOCTL_WD_VERSION, h, pVerInfo, sizeof (WD_VERSION))
#define WD_License(h,pLicense)\
    WD_FUNCTION(IOCTL_WD_LICENSE, h, pLicense, sizeof (WD_LICENSE))

#endif
