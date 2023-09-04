# Microsoft Developer Studio Project File - Name="Bomber" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Bomber - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Bomber.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Bomber.mak" CFG="Bomber - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Bomber - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Bomber - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /Gr /W3 /GX /Ox /Ot /Oa /Og /Oi /Ob2 /Gf /Gy /I "lzo\include" /I "ucl\include" /I "bzip2" /I "SeeR\include" /D "BZ_NO_STDIO" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "LANG_FR" /FD /QIfist /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG" /d "LANG_FR"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ws2_32.lib shell32.lib dinput.lib version.lib winmm.lib comctl32.lib dxguid.lib advapi32.lib ole32.lib kernel32.lib user32.lib gdi32.lib /nologo /subsystem:windows /pdb:none /machine:I386 /opt:ref
# SUBTRACT LINK32 /profile
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Desc=BuildCount...
PreLink_Cmds=BCnt.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /Gi /GR /GX /ZI /Od /I "lzo\include" /I "ucl\include" /I "bzip2" /I "SeeR\include" /D "BZ_NO_STDIO" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "LZO_DEBUG" /D "UCL_DEBUG" /D "LANG_FR" /Fr /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG" /d "LANG_FR"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ws2_32.lib shell32.lib dinput.lib version.lib winmm.lib comctl32.lib dxguid.lib advapi32.lib ole32.lib kernel32.lib user32.lib gdi32.lib /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /profile /pdb:none /incremental:no /nodefaultlib

!ENDIF 

# Begin Target

# Name "Bomber - Win32 Release"
# Name "Bomber - Win32 Debug"
# Begin Group "Bomber"

# PROP Default_Filter ""
# Begin Group "Sources"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\Ace.cpp
# End Source File
# Begin Source File

SOURCE=.\Aleat.cpp
# End Source File
# Begin Source File

SOURCE=.\Bomber.cpp
# End Source File
# Begin Source File

SOURCE=.\Configs.cpp
# End Source File
# Begin Source File

SOURCE=.\Crypte.cpp
# End Source File
# Begin Source File

SOURCE=.\Ddutil.cpp
# End Source File
# Begin Source File

SOURCE=.\Debug.cpp
# End Source File
# Begin Source File

SOURCE=.\Font.cpp
# End Source File
# Begin Source File

SOURCE=.\Getdxver.cpp
# End Source File
# Begin Source File

SOURCE=.\Ia.cpp
# End Source File
# Begin Source File

SOURCE=.\LoadFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Maps.cpp
# End Source File
# Begin Source File

SOURCE=.\Multi.cpp
# End Source File
# Begin Source File

SOURCE=.\Pak.cpp
# End Source File
# Begin Source File

SOURCE=.\pBZ.cpp
# End Source File
# Begin Source File

SOURCE=.\pLZO.cpp
# End Source File
# Begin Source File

SOURCE=.\pUCL.cpp
# End Source File
# Begin Source File

SOURCE=.\Rar.cpp
# End Source File
# Begin Source File

SOURCE=.\Skins.cpp
# End Source File
# Begin Source File

SOURCE=.\Son.cpp
# End Source File
# Begin Source File

SOURCE=.\Voice.cpp
# End Source File
# Begin Source File

SOURCE=.\VoiceStack.cpp
# End Source File
# Begin Source File

SOURCE=.\WSDPlay.cpp
# End Source File
# End Group
# Begin Group "Headers"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\ACE.h
# End Source File
# Begin Source File

SOURCE=.\Aleat.h
# End Source File
# Begin Source File

SOURCE=.\Bcnt.h
# End Source File
# Begin Source File

SOURCE=.\Bomber.h
# End Source File
# Begin Source File

SOURCE=.\Configs.h
# End Source File
# Begin Source File

SOURCE=.\Crypte.h
# End Source File
# Begin Source File

SOURCE=.\Ddutil.h
# End Source File
# Begin Source File

SOURCE=.\Debug.h
# End Source File
# Begin Source File

SOURCE=.\Font.h
# End Source File
# Begin Source File

SOURCE=.\General.h
# End Source File
# Begin Source File

SOURCE=.\GetDXVer.h
# End Source File
# Begin Source File

SOURCE=.\IA.h
# End Source File
# Begin Source File

SOURCE=.\iBomber.h
# End Source File
# Begin Source File

SOURCE=.\iIA.h
# End Source File
# Begin Source File

SOURCE=.\iSon.h
# End Source File
# Begin Source File

SOURCE=.\iVoiceStack.h
# End Source File
# Begin Source File

SOURCE=.\LoadFile.h
# End Source File
# Begin Source File

SOURCE=.\lstrings.h
# End Source File
# Begin Source File

SOURCE=.\Maps.h
# End Source File
# Begin Source File

SOURCE=.\Multi.h
# End Source File
# Begin Source File

SOURCE=.\PAK.h
# End Source File
# Begin Source File

SOURCE=.\pBZ.h
# End Source File
# Begin Source File

SOURCE=.\pLZO.h
# End Source File
# Begin Source File

SOURCE=.\pUCL.h
# End Source File
# Begin Source File

SOURCE=.\RAR.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\Skins.h
# End Source File
# Begin Source File

SOURCE=.\Son.h
# End Source File
# Begin Source File

SOURCE=.\Voice.h
# End Source File
# Begin Source File

SOURCE=.\VoiceStack.h
# End Source File
# Begin Source File

SOURCE=.\WSDPlay.h
# End Source File
# End Group
# Begin Group "Ressources"

# PROP Default_Filter "*.rc;*.ico"
# Begin Source File

SOURCE=.\Bomber.ico
# End Source File
# Begin Source File

SOURCE=.\Bomber.rc
# End Source File
# Begin Source File

SOURCE=.\logo.bmp
# End Source File
# Begin Source File

SOURCE=.\Marble.ico
# End Source File
# End Group
# End Group
# Begin Group "lzo"

# PROP Default_Filter ""
# Begin Group "Sources lzo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lzo\src\alloc.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo1x_9x.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo1x_d1.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Lzo\src\lzo1x_d2.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo_crc.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo_init.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo_ptr.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Lzo\src\lzo_str.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo_util.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Headers lzo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lzo\src\compr1b.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\compr1c.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\config1.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\config1a.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\config1b.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\config1c.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\config1f.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\config1x.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\config1y.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\config1z.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\config2a.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\fake16.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo1a_de.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo1b_cc.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo1b_de.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo1c_cc.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo_conf.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo_dict.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo_ptr.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo_util.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\stats1a.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\stats1b.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\stats1c.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "CHeadres lzo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lzo\src\lzo1_cm.ch

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo1_d.ch

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo1a_cm.ch

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo1a_cr.ch

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo1b_c.ch

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo1b_cm.ch

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo1b_cr.ch

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo1b_d.ch

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo1b_r.ch

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo1b_sm.ch

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo1b_tm.ch

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo1f_d.ch

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo1x_c.ch

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo1x_d.ch

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo1x_oo.ch

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo2a_d.ch

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo_mchw.ch

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\src\lzo_swd.ch

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Export Headers lzo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lzo\include\lzo1.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\include\lzo16bit.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\include\lzo1a.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\include\lzo1b.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\include\lzo1c.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\include\lzo1f.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\include\lzo1x.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\include\lzo1y.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\include\lzo1z.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\include\lzo2a.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\include\lzoconf.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lzo\include\lzoutil.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\lzo"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\lzo"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# End Group
# Begin Group "ucl"

# PROP Default_Filter ""
# Begin Group "Sources ucl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Ucl\src\alloc.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\ucl"

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\ucl"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Ucl\src\n2d_99.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\ucl"

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\ucl"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Ucl\src\n2d_d.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\ucl"

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\ucl"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Ucl\src\n2d_ds.c
# End Source File
# Begin Source File

SOURCE=.\Ucl\src\ucl_init.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\ucl"

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\ucl"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Ucl\src\ucl_ptr.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\ucl"

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\ucl"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Ucl\src\ucl_str.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\ucl"

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\ucl"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Ucl\src\ucl_util.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\ucl"

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\ucl"

!ENDIF 

# End Source File
# End Group
# Begin Group "Headers ucl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Ucl\src\fake16.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\ucl"

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\ucl"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Ucl\src\getbit.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\ucl"

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\ucl"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Ucl\src\internal.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\ucl"

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\ucl"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Ucl\src\ucl_conf.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\ucl"

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\ucl"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Ucl\src\ucl_ptr.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\ucl"

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\ucl"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Ucl\src\ucl_util.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\ucl"

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\ucl"

!ENDIF 

# End Source File
# End Group
# Begin Group "CHeaders ucl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Ucl\src\ucl_mchw.ch

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\ucl"

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\ucl"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Ucl\src\ucl_swd.ch

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\ucl"

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\ucl"

!ENDIF 

# End Source File
# End Group
# Begin Group "Export Headers ucl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Ucl\Include\Ucl\ucl.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\ucl"

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\ucl"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Ucl\Include\Ucl\uclconf.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\ucl"

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\ucl"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Ucl\Include\Ucl\uclutil.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\ucl"

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\ucl"

!ENDIF 

# End Source File
# End Group
# End Group
# Begin Group "bzip2"

# PROP Default_Filter ""
# Begin Group "Sources bzip2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\bzip2\blocksort.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\bzip2"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\bzip2"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\bzip2\bzlib.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\bzip2"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\bzip2"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\bzip2\compress.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\bzip2"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\bzip2"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\bzip2\crctable.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\bzip2"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\bzip2"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\bzip2\decompress.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\bzip2"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\bzip2"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\bzip2\huffman.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\bzip2"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\bzip2"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\bzip2\randtable.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\bzip2"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\bzip2"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Headers bzip2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\bzip2\bzlib.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\bzip2"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\bzip2"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\bzip2\bzlib_private.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\bzip2"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\bzip2"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# End Group
# Begin Group "SeeR"

# PROP Default_Filter ""
# Begin Group "Sources SeeR"

# PROP Default_Filter "c;cpp"
# Begin Source File

SOURCE=.\Seer\Src\Callasm.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\SeeR"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\SeeR"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Seer\Src\Compiler.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\SeeR"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\SeeR"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Seer\Src\Consts.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\SeeR"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\SeeR"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Seer\Src\Declare.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\SeeR"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\SeeR"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Seer\Src\Dict.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\SeeR"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\SeeR"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Seer\Src\Expr.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\SeeR"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\SeeR"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Seer\Src\Headers.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\SeeR"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\SeeR"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Seer\Src\Initial.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\SeeR"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\SeeR"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Seer\Src\Instance.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\SeeR"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\SeeR"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Seer\Src\Parser.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\SeeR"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\SeeR"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Seer\Src\Pmem.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\SeeR"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\SeeR"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Seer\Src\Preproc.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\SeeR"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\SeeR"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Seer\Src\Protect.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\SeeR"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\SeeR"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Seer\Src\Script.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\SeeR"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\SeeR"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Seer\Src\Vars.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\SeeR"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\SeeR"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Seer\Src\Vcpucode.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\SeeR"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\SeeR"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Seer\Src\Vcpudeco.c

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\SeeR"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\SeeR"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Headers SeeR"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\Seer\Src\Code.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\SeeR"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\SeeR"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Seer\Src\Fixed.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\SeeR"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\SeeR"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Seer\Src\Internal.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\SeeR"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\SeeR"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Export Headers SeeR"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\Seer\Include\Seer.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\SeeR"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\SeeR"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# End Group
# Begin Group "CSon"

# PROP Default_Filter ""
# Begin Group "Sources CSon"

# PROP Default_Filter "c;cpp"
# Begin Source File

SOURCE=.\CSon\CSon.cpp

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\CSon"

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\CSon"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CSon\Fft4asm.cpp

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\CSon"

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\CSon"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CSon\FourierFloat.cpp

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\CSon"

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\CSon"

!ENDIF 

# End Source File
# End Group
# Begin Group "Headers CSon"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\CSon\CSon.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\CSon"

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\CSon"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CSon\Fft4asm.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\CSon"

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\CSon"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CSon\FourierFloat.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\CSon"

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\CSon"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CSon\WhatFFT.h

!IF  "$(CFG)" == "Bomber - Win32 Release"

# PROP Intermediate_Dir "Release\CSon"

!ELSEIF  "$(CFG)" == "Bomber - Win32 Debug"

# PROP Intermediate_Dir "Debug\CSon"

!ENDIF 

# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=.\Debug.txt
# End Source File
# End Target
# End Project
