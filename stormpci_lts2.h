// (c) 2007 Boltek Corp

// Library functions for accessing StormTracker PCI card

// Modified 07/02/2007 for LDN_Detector support

// LDN_Detector is Boltek software for letting a StormTracker card
// contribute data to one or more lightning detection networks. LDN_Detector
// loads on system boot and runs in the system tray. LDN_Detector reads 
// data from the StormTracker card and passes it along to the lightning detection
// network servers for triangulation with other detectors.  The local StormTracker
// card data is still accessible to local viewing software (such as 
// Lightning/2000 or NexStorm) through a socket connection with LDN_Detector. 
// This action is completetly transparent to the programmer and user. This
// library's StormPCI_OpenPciCard function will determin if LDN_Detector is 
// running. If running it will open a socket connection to LDN_Detector
// and all functions will operate through that connection.
// If it is not running then all functions will access the hardware
// directly.  This is automatic and invisible to the program
// calling these functions.


// It is now possible to access a StormTracker card installed in a remote 
// computer. The computer with the StormTracker card installed must be
// running LDN_Detector. Call StormPCI_SetLdnDetectorAddress() with the address
// and port number of the remote computer before calling StormPCI_OpenCard() 
// LDN_Detector supports multiple remote connections so multiple computers
// may access the same card.

// Of course this can only work if the StormTracker computer is visible to the
// viewing computers, that is, it is not behind a firewall.

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the STORMPCI_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// STORMPCI_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef STORMPCI_EXPORTS
#define STORMPCI_API __declspec(dllexport)
#else
#define STORMPCI_API __declspec(dllimport)
#endif

#define BUFFERSIZE 512   // Capture Buffer goes from 0 - 511 = 512 bytes.  

struct StormPCI_tPACKEDDATA
{
	unsigned short usNorth[BUFFERSIZE];
	unsigned short usWest[BUFFERSIZE];
};

// These are the functions you need to call to access the StormTracker card
// They are listed in pretty much the same order you will be calling them

// Your software can access a remote StormTracker card. if that remote computer is running 
// Boltek's LDN_Detector software (it runs in the system tray, soon to be converted to a service)
// If the StormTracker card is not installed in this computer you must call this 
// function first  with the hostname or ip and port of the StormTracker computer
// Default address for LDN_Detector socket connection:
// Address: localhost  Port: 9736
// You don't need to call this function if you are accessing a local StormTracker board (in this computer)
STORMPCI_API void StormPCI_SetLdnDetectorAddress(char hostname[80], int port); 

// Connect to local or remote LDN_Detector or the StormTracker card directly
STORMPCI_API int  StormPCI_OpenPciCard(); 

// You can use the ToneOn() and ToneOff() functions below to beep the internal speaker
// Use a timer to turn on and off the speaker beep
STORMPCI_API bool StormPCI_SpeakerOpen(); // returns true if opened ok

STORMPCI_API void StormPCI_SetSquelch(char trig_level); // 0-15, 0:most sensitive (preferred), 15: least sensitive

STORMPCI_API void StormPCI_ResetTimestamp(); // must reset the LTS-2 before using it

STORMPCI_API void StormPCI_RestartBoard(); // after reading the data, wait for the next strike 

STORMPCI_API void StormPCI_ForceTrigger(); // force StormTracker to give you a capture, to read the gps for example

STORMPCI_API bool StormPCI_StrikeReady(); // check if a strike is waiting to be read by GetBoardCapture()

STORMPCI_API void StormPCI_GetBoardCapture(StormPCI_tPACKEDDATA* board_capture); // retrieve the waiting capture data

STORMPCI_API void StormPCI_SpeakerToneOn(unsigned long freq);

STORMPCI_API void StormPCI_SpeakerToneOff();

STORMPCI_API void StormPCI_ClosePciCard(); 

STORMPCI_API void StormPCI_SpeakerClose();

// ------------------------------------------------------
//                   END OF FILE
// ------------------------------------------------------