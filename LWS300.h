// (c) 2007 Boltek Corp

// Library functions for accessing LD-350


// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LWS300_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LWS300_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef LWS300_EXPORTS
#define LWS300_API __declspec(dllexport)
#else
#define LWS300_API __declspec(dllimport)
#endif


#define MAX_MESSAGE_LEN 50

struct LWS300_MessageDataType
{
	char message[MAX_MESSAGE_LEN];  // status and strike messages
};

#define BUFFERSIZE 512   // Capture Buffer goes from 0 - 511 = 512 bytes.  
#define GPSDATASIZE 164  // SanityCheck(1)+Timestamp(8)+Checksum(1)+ GPS Data(154)

struct LWS300_CaptureDataType {
   int East[BUFFERSIZE];
   int North[BUFFERSIZE];
   int E_Field[BUFFERSIZE];
   char GPS_Data[GPSDATASIZE];
};


LWS300_API int  LWS300_Open(); // connect to LWS
LWS300_API void LWS300_SetSquelch( char ); // 0-15, 0:most sensitive (preferred), 15: least sensitive
LWS300_API void LWS300_ForceTrigger(); // force LWS to give you a capture
LWS300_API void LWS300_RawDataOn(); // tell LWS to start sending raw capture data
LWS300_API void LWS300_RawDataOff(); // tell LWS to start sending raw capture data
LWS300_API bool LWS300_MessageReady(); // check if a strike is waiting to be read by GetCapture()
LWS300_API bool LWS300_CaptureReady(); // check if a strike is waiting to be read by GetCapture()
LWS300_API void LWS300_GetMessageData( LWS300_MessageDataType* ); // retrieve the waiting message
LWS300_API void LWS300_GetCaptureData( LWS300_CaptureDataType* ); // retrieve the waiting capture
LWS300_API void LWS300_Command(char*); // set real time clock "mm/dd/yy hh:mm:ss"
LWS300_API void LWS300_Close(); // clean up, all done
