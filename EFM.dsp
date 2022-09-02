# Microsoft Developer Studio Project File - Name="EFM" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=EFM - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "EFM.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "EFM.mak" CFG="EFM - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "EFM - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "EFM - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "EFM - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zd /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x1009 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib ftd2xx.lib /nologo /subsystem:windows /map /machine:I386 /MAPINFO:LINES /MAPINFO:EXPORTS
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "EFM - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x1009 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib ftd2xx.lib /nologo /subsystem:windows /map /debug /machine:I386 /pdbtype:sept /MAPINFO:LINES /MAPINFO:EXPORTS
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "EFM - Win32 Release"
# Name "EFM - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AddEditRecipient.cpp
# End Source File
# Begin Source File

SOURCE=.\Alerts.cpp

!IF  "$(CFG)" == "EFM - Win32 Release"

!ELSEIF  "$(CFG)" == "EFM - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\base64.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\ConfigEMailServer.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigGSMGateway.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigGSMModem.cpp
# End Source File
# Begin Source File

SOURCE=.\Configuration.cpp
# End Source File
# Begin Source File

SOURCE=.\ConnectedClient.cpp
# End Source File
# Begin Source File

SOURCE=.\CSmtp.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\DlgAddRemoveRing.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgDebugger.cpp
# End Source File
# Begin Source File

SOURCE=.\EFM.cpp
# End Source File
# Begin Source File

SOURCE=.\EFM.rc
# End Source File
# Begin Source File

SOURCE=.\EFMDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EFMOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\EMailRecipients.cpp
# End Source File
# Begin Source File

SOURCE=.\EventLog.cpp
# End Source File
# Begin Source File

SOURCE=.\EventLogViewer.cpp
# End Source File
# Begin Source File

SOURCE=.\font.cpp
# End Source File
# Begin Source File

SOURCE=.\Gsm.cpp
# End Source File
# Begin Source File

SOURCE=.\HTML.cpp
# End Source File
# Begin Source File

SOURCE=.\lightningalarm.cpp
# End Source File
# Begin Source File

SOURCE=.\Log.cpp
# End Source File
# Begin Source File

SOURCE=.\md5.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\numled.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsAlerts.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsDataLogging.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsDataSharing.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsDeltaAlarm.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsGeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsHighFieldAlarm.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsImageCapture.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsLightningAlarm.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsVeryHighFieldAlarm.cpp
# End Source File
# Begin Source File

SOURCE=.\percent.cpp
# End Source File
# Begin Source File

SOURCE=.\picture.cpp
# End Source File
# Begin Source File

SOURCE=.\PSOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\Registry.cpp
# End Source File
# Begin Source File

SOURCE=.\selector.cpp
# End Source File
# Begin Source File

SOURCE=.\Serial.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SerialEx.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SerialWnd.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SMSRecipients.cpp
# End Source File
# Begin Source File

SOURCE=.\speaker_lib.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\strip.cpp
# End Source File
# Begin Source File

SOURCE=.\TabCheckListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\toggle.cpp
# End Source File
# Begin Source File

SOURCE=.\WaitDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AddEditRecipient.h
# End Source File
# Begin Source File

SOURCE=.\Alerts.h
# End Source File
# Begin Source File

SOURCE=.\base64.h
# End Source File
# Begin Source File

SOURCE=.\BITS.H
# End Source File
# Begin Source File

SOURCE=.\ConfigEMailServer.h
# End Source File
# Begin Source File

SOURCE=.\ConfigGSMGateway.h
# End Source File
# Begin Source File

SOURCE=.\ConfigGSMModem.h
# End Source File
# Begin Source File

SOURCE=.\Configuration.h
# End Source File
# Begin Source File

SOURCE=.\ConnectedClient.h
# End Source File
# Begin Source File

SOURCE=.\CSmtp.h
# End Source File
# Begin Source File

SOURCE=.\DlgAddRemoveRing.h
# End Source File
# Begin Source File

SOURCE=.\DlgDebugger.h
# End Source File
# Begin Source File

SOURCE=.\EFM.h
# End Source File
# Begin Source File

SOURCE=.\EFMDlg.h
# End Source File
# Begin Source File

SOURCE=.\EFMOptions.h
# End Source File
# Begin Source File

SOURCE=.\EMailRecipients.h
# End Source File
# Begin Source File

SOURCE=.\EventLog.h
# End Source File
# Begin Source File

SOURCE=.\EventLogViewer.h
# End Source File
# Begin Source File

SOURCE=.\font.h
# End Source File
# Begin Source File

SOURCE=.\Gsm.h
# End Source File
# Begin Source File

SOURCE=.\HTML.h
# End Source File
# Begin Source File

SOURCE=.\lightningalarm.h
# End Source File
# Begin Source File

SOURCE=.\Log.h
# End Source File
# Begin Source File

SOURCE=.\md5.h
# End Source File
# Begin Source File

SOURCE=.\numled.h
# End Source File
# Begin Source File

SOURCE=.\OptionsAlerts.h
# End Source File
# Begin Source File

SOURCE=.\OptionsDataLogging.h
# End Source File
# Begin Source File

SOURCE=.\OptionsDataSharing.h
# End Source File
# Begin Source File

SOURCE=.\OptionsDeltaAlarm.h
# End Source File
# Begin Source File

SOURCE=.\OptionsGeneral.h
# End Source File
# Begin Source File

SOURCE=.\OptionsHighFieldAlarm.h
# End Source File
# Begin Source File

SOURCE=.\OptionsImageCapture.h
# End Source File
# Begin Source File

SOURCE=.\OptionsLightningAlarm.h
# End Source File
# Begin Source File

SOURCE=.\OptionsVeryHighFieldAlarm.h
# End Source File
# Begin Source File

SOURCE=.\percent.h
# End Source File
# Begin Source File

SOURCE=.\picture.h
# End Source File
# Begin Source File

SOURCE=.\PSOptions.h
# End Source File
# Begin Source File

SOURCE=.\Registry.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\selector.h
# End Source File
# Begin Source File

SOURCE=.\Serial.h
# End Source File
# Begin Source File

SOURCE=.\SerialEx.h
# End Source File
# Begin Source File

SOURCE=.\SerialMFC.h
# End Source File
# Begin Source File

SOURCE=.\SerialWnd.h
# End Source File
# Begin Source File

SOURCE=.\SMSRecipients.h
# End Source File
# Begin Source File

SOURCE=.\speaker_lib.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\strip.h
# End Source File
# Begin Source File

SOURCE=.\TabCheckListBox.h
# End Source File
# Begin Source File

SOURCE=.\toggle.h
# End Source File
# Begin Source File

SOURCE=.\version.h
# End Source File
# Begin Source File

SOURCE=.\VersionNo.h
# End Source File
# Begin Source File

SOURCE=.\WaitDlg.h
# End Source File
# Begin Source File

SOURCE=.\WINDRVR.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=".\res\1309906488_favicon[1].ico"
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00002.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00003.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00004.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp_config.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp_opti.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp_peop.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Boltek_ColorLoResSM.bmp
# End Source File
# Begin Source File

SOURCE=".\res\brightex-logo.bmp"
# End Source File
# Begin Source File

SOURCE=.\res\brightex.bmp
# End Source File
# Begin Source File

SOURCE=.\res\EFM.ico
# End Source File
# Begin Source File

SOURCE=.\res\EFM.rc2
# End Source File
# Begin Source File

SOURCE=.\res\soundoff.bmp
# End Source File
# Begin Source File

SOURCE=.\res\soundon.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Documentation\Changes.txt
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\openssl\lib\ssleay32.lib
# End Source File
# Begin Source File

SOURCE=.\openssl\lib\libeay32.lib
# End Source File
# End Target
# End Project
# Section EFM : {BEF6E003-A874-101A-8BBA-00AA00300CAB}
# 	2:5:Class:COleFont
# 	2:10:HeaderFile:font.h
# 	2:8:ImplFile:font.cpp
# End Section
# Section EFM : {1A0FF33C-8DC4-4255-AB0C-16CA01ED9FA3}
# 	2:5:Class:CLightningAlarm
# 	2:10:HeaderFile:lightningalarm.h
# 	2:8:ImplFile:lightningalarm.cpp
# End Section
# Section EFM : {999EA2C3-F93F-11CF-A40A-444553540000}
# 	2:21:DefaultSinkHeaderFile:toggle.h
# 	2:16:DefaultSinkClass:CToggle
# End Section
# Section EFM : {A0ED9402-6466-11CF-BC8B-207402C10627}
# 	2:5:Class:CPercent
# 	2:10:HeaderFile:percent.h
# 	2:8:ImplFile:percent.cpp
# End Section
# Section EFM : {7BF80981-BF32-101A-8BBB-00AA00300CAB}
# 	2:5:Class:CPicture
# 	2:10:HeaderFile:picture.h
# 	2:8:ImplFile:picture.cpp
# End Section
# Section EFM : {A68A1581-4572-11D0-B597-00A024A54A93}
# 	2:5:Class:CNumLED
# 	2:10:HeaderFile:numled.h
# 	2:8:ImplFile:numled.cpp
# End Section
# Section EFM : {B9617403-76BA-11CF-BC8B-207402C10627}
# 	2:21:DefaultSinkHeaderFile:selector.h
# 	2:16:DefaultSinkClass:CSelector
# End Section
# Section EFM : {0DF75593-1C7B-11D0-A89E-00A024C85C82}
# 	2:21:DefaultSinkHeaderFile:strip.h
# 	2:16:DefaultSinkClass:CStrip
# End Section
# Section EFM : {73861100-D037-4EF3-8EB5-62A83A22ECC9}
# 	2:21:DefaultSinkHeaderFile:lightningalarm.h
# 	2:16:DefaultSinkClass:CLightningAlarm
# End Section
# Section EFM : {999EA2C1-F93F-11CF-A40A-444553540000}
# 	2:5:Class:CToggle
# 	2:10:HeaderFile:toggle.h
# 	2:8:ImplFile:toggle.cpp
# End Section
# Section EFM : {5E7839C1-922F-11D0-B597-00A024A63F06}
# 	2:5:Class:CStrip
# 	2:10:HeaderFile:strip.h
# 	2:8:ImplFile:strip.cpp
# End Section
# Section EFM : {B9617401-76BA-11CF-BC8B-207402C10627}
# 	2:5:Class:CSelector
# 	2:10:HeaderFile:selector.h
# 	2:8:ImplFile:selector.cpp
# End Section
# Section EFM : {A0ED9404-6466-11CF-BC8B-207402C10627}
# 	2:21:DefaultSinkHeaderFile:percent.h
# 	2:16:DefaultSinkClass:CPercent
# End Section
# Section EFM : {A68A1583-4572-11D0-B597-00A024A54A93}
# 	2:21:DefaultSinkHeaderFile:numled.h
# 	2:16:DefaultSinkClass:CNumLED
# End Section
