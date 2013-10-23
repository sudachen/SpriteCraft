# Microsoft Developer Studio Project File - Name="spritecraft" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=spritecraft - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "spritecraft.src.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "spritecraft.src.mak" CFG="spritecraft - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "spritecraft - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "spritecraft - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/spritecraft.4/spritecraft.SRC", MXAAAAAA"
# PROP Scc_LocalPath "."
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "spritecraft - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../temp/Dll.Scraft3.Release"
# PROP Intermediate_Dir "../../temp/Dll.Scraft3.Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /Gz /MD /W3 /Zi /Ox /Og /Oi /Os /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"StdAfx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /out:"../scraft.dll"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "spritecraft - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../temp/Dll.Scraft3.Debug"
# PROP Intermediate_Dir "../../temp/Dll.Scraft3.Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /Gz /MTd /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"StdAfx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../scraft_d.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "spritecraft - Win32 Release"
# Name "spritecraft - Win32 Debug"
# Begin Group "Level Hi"

# PROP Default_Filter ""
# Begin Group "Engine"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\console.cpp
# End Source File
# Begin Source File

SOURCE=.\console.h
# End Source File
# Begin Source File

SOURCE=.\Engine.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine.h
# End Source File
# Begin Source File

SOURCE=.\Engine_DInput.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine_DInput.h
# End Source File
# Begin Source File

SOURCE=.\Engine_globals.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine_input.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine_timer.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine_timer.h
# End Source File
# Begin Source File

SOURCE=.\GenericExecutor.cpp
# End Source File
# End Group
# Begin Group "Sprites"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SpriteHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\SpriteHelper.h
# End Source File
# Begin Source File

SOURCE=.\SpriteHierarchy.cpp
# End Source File
# Begin Source File

SOURCE=.\SpriteLwsArray.cpp
# End Source File
# Begin Source File

SOURCE=.\SpritePrimitive.cpp
# End Source File
# Begin Source File

SOURCE=.\SpritesEnumerator.cpp
# End Source File
# Begin Source File

SOURCE=.\SpritesEnumerator.h
# End Source File
# Begin Source File

SOURCE=.\SpriteTextView.cpp
# End Source File
# Begin Source File

SOURCE=.\TileMap.cpp
# End Source File
# Begin Source File

SOURCE=.\TileMap.h
# End Source File
# Begin Source File

SOURCE=.\WeakSprite.cpp
# End Source File
# End Group
# Begin Group "Managers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AnimationMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimationMgr.h
# End Source File
# Begin Source File

SOURCE=.\CollideMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\CollideMgr.h
# End Source File
# Begin Source File

SOURCE=.\MListenMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\MListenMgr.h
# End Source File
# Begin Source File

SOURCE=.\SoundMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\SoundMgr.h
# End Source File
# End Group
# Begin Group "Dispatchers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\dispsubimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\dispsubimpl.h
# End Source File
# End Group
# Begin Group "Lists"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ObjList.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjList.inl
# End Source File
# Begin Source File

SOURCE=.\SpritesList.cpp
# End Source File
# Begin Source File

SOURCE=.\SpritesList.h
# End Source File
# End Group
# Begin Group "Particles"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Particles.cpp
# End Source File
# Begin Source File

SOURCE=.\Particles.h
# End Source File
# Begin Source File

SOURCE=.\ParticlesEnumerator.cpp
# End Source File
# Begin Source File

SOURCE=.\ParticlesEnumerator.h
# End Source File
# Begin Source File

SOURCE=.\ps_default.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\ps_default.ps
# End Source File
# End Group
# Begin Group "Util Classes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Randomizer.cpp
# End Source File
# Begin Source File

SOURCE=.\Randomizer.h
# End Source File
# Begin Source File

SOURCE=.\RBGFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\RBGFilter.h
# End Source File
# End Group
# Begin Group "Hi Level Xdata"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\scXMLite.cpp
# End Source File
# Begin Source File

SOURCE=.\scXMLite.h
# End Source File
# End Group
# End Group
# Begin Group "Level Low"

# PROP Default_Filter ""
# Begin Group "Xdata"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\xdata.cpp
# End Source File
# Begin Source File

SOURCE=.\xdata.h
# End Source File
# Begin Source File

SOURCE=.\xdata_paser.cpp
# End Source File
# End Group
# Begin Group "Sound System"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SND_dxs_factory.cpp
# End Source File
# Begin Source File

SOURCE=.\SND_internal.h
# End Source File
# Begin Source File

SOURCE=.\SND_sndsubsys.cpp
# End Source File
# Begin Source File

SOURCE=.\SND_sound.h
# End Source File
# Begin Source File

SOURCE=.\SND_stream_ogg.cpp
# End Source File
# Begin Source File

SOURCE=.\SND_stream_wav.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\SST.cpp
# End Source File
# Begin Source File

SOURCE=.\SST.h
# End Source File
# End Group
# Begin Group "Python"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\epython.cpp
# End Source File
# Begin Source File

SOURCE=.\PyDispatcher.cpp
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\loginfo.cpp
# End Source File
# Begin Source File

SOURCE=.\logout.h
# End Source File
# Begin Source File

SOURCE=.\scerror.cpp
# End Source File
# Begin Source File

SOURCE=.\scerror.h
# End Source File
# End Group
# Begin Group "DLL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\sharedcode.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\spritecraft.cpp
# End Source File
# Begin Source File

SOURCE=.\spritecraft.def
# End Source File
# Begin Source File

SOURCE=.\spritecraft.h
# End Source File
# Begin Source File

SOURCE=.\spritecraft.idl
# ADD MTL /tlb "../spritecraft.tlb" /h "../ScraftAPI/spritecraft.h"
# SUBTRACT MTL /mktyplib203
# End Source File
# Begin Source File

SOURCE=.\spritecraft.SRC.rc
# End Source File
# Begin Source File

SOURCE=.\spritecraft_interfaces.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\favicon.ico
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"StdAfx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Target
# End Project
