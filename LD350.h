// (c) 2009 Boltek Corp

// Library functions for accessing LD-300


// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LD350_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LD350_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef LD350_EXPORTS
#define LD350_API __declspec(dllexport)
#else
#define LD350_API __declspec(dllimport)
#endif


#define MAX_MESSAGE_LEN 50

struct LD350_MessageDataType
{
	char message[MAX_MESSAGE_LEN];  // status and strike messages
};

#define CAPTURE_BUFFERSIZE 512   // Capture Buffer goes from 0 - 511 = 512 bytes.  
#define LTS3_BUFFERSIZE 167  // SanityCheck(1)+Timestamp(8)+Checksum(1)+ GPS Data(154)

// tSATELLITETYPE is the detailed satellite info from the gps
//
struct LD350_tSATELLITETYPE
{
  char SVID; // 0..37
  char mode; // 0..8
    // where 0 = code search   5 = message sync detect
    //       1 = code acquire  6 = satellite time available
    //       2 = AGC set       7 = ephemeris acquire
    //       3 = freq acquire  8 available for position
    //       4 = bit sync detect
  unsigned char signal_strength; // 0..255
  unsigned short channel_status; // 16 bits
    // where (msb) bit 15 = reserved
    //             bit 14 = reserved
    //             bit 13 = reserved
    //             bit 12 = narrow-band search mode
    //             bit 11 = channel used for time solution
    //             bit 10 = differential corrections available
    //             bit  9 = invalid data
    //             bit  8 = parity error
    //             bit  7 = channel used for position fix
    //             bit  6 = satellite momentum alert flag
    //             bit  5 = satellite anti-spoof flag set
    //             bit  4 = satellite reported unhealthy
    //             bit  3 = satellite accuracy (msb)
    //             bit  2 = satellite accuracy
    //             bit  1 = satellite accuracy 
    //             bit  0 = satellite accuracy (lsb)
};


// tTIMESTAMPINFO is the detailed gps and timestamp data
//
typedef struct 
{
  bool TS_valid;
  unsigned long TS_Osc; // actual frequency of timestamp's 50MHz osc
  unsigned long TS_time; // 0 to TS_Osc - How many 50 MHz clocks at trigger
  unsigned char TS_10ms; // 0..99 - low res timestamp from CPU interrupt, as sanity check

  bool gps_data_valid;
  unsigned char month; // 1..12
  unsigned char day; // 1..31
  unsigned short year; // 1998 to 2079
  unsigned char hours; // 0..23
  unsigned char minutes; // 0..59
  unsigned char seconds; // 0..60
  int latitude_mas; // 324,000,000..324,000,000 (-90°..+90°)
  int longitude_mas; // 648,000,000..648,000,000 (-180°..+176°)
  char latitude_ns; // 'N' or 'S'
  char longitude_ew; // 'E' or 'W'
  int height_cm; // -100,000..+1,800,000 (-1000..+18,000m)
  unsigned short dop; // 0..999 (0.0 to 99.9 DOP)
  unsigned char satellites_visible; // 0..12
  unsigned char satellites_tracked; // 0..12
  LD350_tSATELLITETYPE satellite[12]; // individual satellite data
  unsigned short receiver_status; // 16 bits
    // where (msb) bit 15..13    111 = 3D fix
    //                           110 = 2D fix
    //                           101 = propogate mode
    //                           100 = position hold
    //                           011 = acquiring satellites
    //                           010 = bad geometry
    //                           001 = reserved
    //                           000 = reserved
    //             bit 12..11 = reserved
    //             bit 10 = narrow band tracking mode
    //             bit  9 = fast acquisition position
    //             bit  8 = filter reset to raw gps solution
    //             bit  7 = cold start (no almanac or almanac out of date or time & position unknown)
    //             bit  6 = differential fix
    //             bit  5 = position lock
    //             bit  4 = autosurvey mode
    //             bit  3 = insufficient visible satellites
    //             bit  2..1 = antenna sense
    //               where 00 = Ok
    //                     01 = over current
    //                     10 = under current
    //                     11 = no bias voltage
    //             bit  0 = code location
  unsigned short serial_number;
} LD350_tTIMESTAMPINFO;


struct LD350_CaptureDataType {
   int East[CAPTURE_BUFFERSIZE];
   int North[CAPTURE_BUFFERSIZE];
   int E_Field[CAPTURE_BUFFERSIZE];
   unsigned char LTS3_Data[LTS3_BUFFERSIZE];
};

/*  
* Function: LD350_Open()
* Parameter: none
* Returns: 1 if successful, otherwise 0
* Open a connection to the LD-350.  
* You must call this function before calling any other LD350_API function.
*/
LD350_API int  LD350_Open();

/*
* Function: LD350_SetTriggerLevel( unsigned char )
* Parameter: trigger level 0-100, 0:most sensitive, 100: least sensitive
* Returns: 1 if successful, otherwise 0
* Sets the threshold at which the hardware will capture a strike event and analyze the signal
* Setting the level too low (below 40) can cause the hardware to constantly trigger on background noise
* Setting the level too high (above 78) can cause the hardware to ignore weaker and farther distant signals
* LD-250 Squelch  0 = LD-350 Trigger Level 40
* LD-250 Squelch  1 = LD-350 Trigger Level 43
* LD-250 Squelch  2 = LD-350 Trigger Level 47
* LD-250 Squelch  4 = LD-350 Trigger Level 53
* LD-250 Squelch  8 = LD-350 Trigger Level 58
* LD-250 Squelch 15 = LD-350 Trigger Level 78
*/
LD350_API int  LD350_SetTriggerLevel( unsigned char );

/*
* Function: LD350_KeepAlive()
* Parameter: none
* Returns: 1 if successful, otherwise 0
* Call this function at least once per second to maintain the USB connection
* Failing to call this function at least once per second will cause the LD-350 to stop transmitting data on the USB port
*/
LD350_API int  LD350_KeepAlive(); 

/*
* Function: LD350_ForceTrigger()
* Parameter: none
* Returns: 1 if successful, otherwise 0
* Triggers the LD-350 ADC hardware as if a strike was detected causing the LD-350 to process and send data
* Most likely results in a noise detection since there was no valid lightning signal found
*/
LD350_API int  LD350_ForceTrigger();

/*
* Function: LD350_RawDataOn()
* Parameter: none
* Returns: 1 if successful, otherwise 0
* Enables the delivery of raw binary waveform data from the LD-350 to the DLL
* Once the DLL receives a complete capture of waveform data LD350_CaptureReady()
* will return true and LD350_GetMessageData() can be called to retrieve the data
*/
LD350_API int  LD350_RawDataOn();

/*
* Function: LD350_RawDataOn()
* Parameter: none
* Returns: 1 if successful, otherwise 0
* Disables the delivery of raw binary waveform data from the LD-350 
*/
LD350_API int  LD350_RawDataOff();

/*
* Function: LD350_MessageReady()
* Parameter: none
* Returns: true if a message from the LD-350 is waiting, otherwise false
* If LD350_MessageReady() returns true call LD350_GetMessageData() to retrieve the message
*/
LD350_API bool LD350_MessageReady();

/*
* Function: LD350_CaptureReady()
* Parameter: none
* Returns: true if a binary waveform capture from the LD-350 is waiting, otherwise false
* If LD350_CaptureReady() returns true call LD350_GetCaptureData() to retrieve the capture
*/
LD350_API bool LD350_CaptureReady();

/*
* Function: LD350_GetMessageData( LD350_MessageDataType* )
* Parameter: pointer to LD350_MessageDataType (character string)
* Returns: void
* If LD350_MessageReady() returns true call LD350_GetMessageData() to retrieve the message
*/
LD350_API void LD350_GetMessageData( LD350_MessageDataType* );

/*
* Function: LD350_GetCaptureData( LD350_CaptureDataType* )
* Parameter: pointer to LD350_CaptureDataType (binary waveform data)
* Returns: void
* If LD350_CaptureReady() returns true call LD350_GetCaptureData() to retrieve the capture
*/
LD350_API void LD350_GetCaptureData( LD350_CaptureDataType* ); // retrieve the waiting capture

/*
* Function: LD350_ExtractLTS3Data(LD350_CaptureDataType*)
* Paramter: pointer to LD350_CaptureDataType (raw GPS data)
* Returns: LD350_tTIMESTAMPINFO (structure containing GPS data)
* Extracts the LTS-3 GPS and timestamp data from the LD350_CaptureData
* The LD-350 needs to have the LTS-3 option board installed 
*/
LD350_API LD350_tTIMESTAMPINFO LD350_ExtractLTS3Data(LD350_CaptureDataType*);

/*
* Function: LD350_Command()
* Parameter: none
* Returns: 1 if successful, otherwise 0
* Send a command to the LD-350.  Commands are text strings, for example:
* "SQ" - query trigger level
* "SQ 50" - set trigger level to 50
*/
LD350_API int  LD350_Command(char*); // send any text string command

/*
* Function: LD350_Command()
* Parameter: none
* Returns: 1 if successful, otherwise 0
* Closes the USB connection to the LD-350
*/
LD350_API int  LD350_Close();
