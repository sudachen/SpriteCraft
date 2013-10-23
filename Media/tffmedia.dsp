# Microsoft Developer Studio Project File - Name="lib.tffmedia" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=lib.tffmedia - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tffmedia.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tffmedia.mak" CFG="lib.tffmedia - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lib.tffmedia - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "lib.tffmedia - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "lib.tffmedia - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../temp/Lib.Tffmedia.Release"
# PROP Intermediate_Dir "../temp/Lib.Tffmedia.Release"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /Gz /MD /W3 /Zi /Ox /Ot /Og /Oi /Gf /Gy /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FAs /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"tffmedia.lib"

!ELSEIF  "$(CFG)" == "lib.tffmedia - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../temp/Lib.Tffmedia.Debug"
# PROP Intermediate_Dir "../temp/Lib.Tffmedia.Debug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /Gz /MTd /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"tffmedia_d.lib"

!ENDIF 

# Begin Target

# Name "lib.tffmedia - Win32 Release"
# Name "lib.tffmedia - Win32 Debug"
# Begin Group "jpeglib"

# PROP Default_Filter ""
# Begin Group "jpeg.header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lib.jpeg\jconfig.h
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jdct.h
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jdhuff.h
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jerror.h
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jinclude.h
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jmemsys.h
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jmorecfg.h
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jpegint.h
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jpeglib.h
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jversion.h
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\transupp.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\lib.jpeg\jcomapi.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jdapimin.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jdapistd.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jdatadst.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jdatasrc.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jdcoefct.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jdcolor.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jddctmgr.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jdhuff.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jdinput.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jdmainct.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jdmarker.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jdmaster.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jdmerge.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jdphuff.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jdpostct.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jdsample.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jdtrans.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jerror.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jfdctflt.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jfdctfst.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jfdctint.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jidctflt.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jidctfst.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jidctint.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jidctred.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jmemmgr.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jmemnobs.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jquant1.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jquant2.c
# End Source File
# Begin Source File

SOURCE=.\lib.jpeg\jutils.c
# End Source File
# End Group
# Begin Group "oggvorbis"

# PROP Default_Filter ""
# Begin Group "include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lib.oggvorbis\include\codec.h
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\include\ogg.h
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\include\os_types.h
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\include\vorbisfile.h
# End Source File
# End Group
# Begin Group "ogg"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lib.oggvorbis\ogg\bitwise.c
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\ogg\framing.c
# End Source File
# End Group
# Begin Group "vorbis"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\backends.h
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\bitrate.h
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\block.c
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\codebook.c
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\codebook.h
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\codec_internal.h
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\envelope.c
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\envelope.h
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\floor0.c
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\floor1.c
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\highlevel.h
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\info.c
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\lookup.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\lookup.h
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\lookup_data.h
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\lpc.c
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\lpc.h
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\lsp.c
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\lsp.h
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\mapping0.c
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\masking.h
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\mdct.c
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\mdct.h
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\misc.h
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\os.h
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\psy.c
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\psy.h
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\registry.c
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\registry.h
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\res0.c
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\scales.h
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\sharedbook.c
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\smallft.c
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\smallft.h
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\synthesis.c
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\vorbisfile.c
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\window.c
# End Source File
# Begin Source File

SOURCE=.\lib.oggvorbis\vorbis\window.h
# End Source File
# End Group
# End Group
# Begin Group "pnglib"

# PROP Default_Filter ""
# Begin Group "png.header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lib.png\png.h
# End Source File
# Begin Source File

SOURCE=.\lib.png\pngconf.h
# End Source File
# Begin Source File

SOURCE=.\lib.png\zconf.h
# End Source File
# Begin Source File

SOURCE=.\lib.png\zlib.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\lib.png\png.c
# End Source File
# Begin Source File

SOURCE=.\lib.png\pngerror.c
# End Source File
# Begin Source File

SOURCE=.\lib.png\pngget.c
# End Source File
# Begin Source File

SOURCE=.\lib.png\pngmem.c
# End Source File
# Begin Source File

SOURCE=.\lib.png\pngpread.c
# End Source File
# Begin Source File

SOURCE=.\lib.png\pngread.c
# End Source File
# Begin Source File

SOURCE=.\lib.png\pngrio.c
# End Source File
# Begin Source File

SOURCE=.\lib.png\pngrtran.c
# End Source File
# Begin Source File

SOURCE=.\lib.png\pngrutil.c
# End Source File
# Begin Source File

SOURCE=.\lib.png\pngset.c
# End Source File
# Begin Source File

SOURCE=.\lib.png\pngtrans.c
# End Source File
# Begin Source File

SOURCE=.\lib.png\pngwio.c
# End Source File
# Begin Source File

SOURCE=.\lib.png\pngwtran.c
# End Source File
# End Group
# Begin Group "zlib"

# PROP Default_Filter ""
# Begin Group "zlib.header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lib.z\zconf.h
# End Source File
# Begin Source File

SOURCE=.\lib.z\zl_deflate.h
# End Source File
# Begin Source File

SOURCE=.\lib.z\zl_infblock.h
# End Source File
# Begin Source File

SOURCE=.\lib.z\zl_infcodes.h
# End Source File
# Begin Source File

SOURCE=.\lib.z\zl_inffast.h
# End Source File
# Begin Source File

SOURCE=.\lib.z\zl_inffixed.h
# End Source File
# Begin Source File

SOURCE=.\lib.z\zl_inftrees.h
# End Source File
# Begin Source File

SOURCE=.\lib.z\zl_infutil.h
# End Source File
# Begin Source File

SOURCE=.\lib.z\zl_mskin_defs.h
# End Source File
# Begin Source File

SOURCE=.\lib.z\zl_trees.h
# End Source File
# Begin Source File

SOURCE=.\lib.z\zlib.h
# End Source File
# Begin Source File

SOURCE=.\lib.z\zutil.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\lib.z\zl_adler32.c
# End Source File
# Begin Source File

SOURCE=.\lib.z\zl_compress.c
# End Source File
# Begin Source File

SOURCE=.\lib.z\zl_crc32.c
# End Source File
# Begin Source File

SOURCE=.\lib.z\zl_deflate.c
# End Source File
# Begin Source File

SOURCE=.\lib.z\zl_infblock.c
# End Source File
# Begin Source File

SOURCE=.\lib.z\zl_infcodes.c
# End Source File
# Begin Source File

SOURCE=.\lib.z\zl_inffast.c
# End Source File
# Begin Source File

SOURCE=.\lib.z\zl_inflate.c
# End Source File
# Begin Source File

SOURCE=.\lib.z\zl_inftrees.c
# End Source File
# Begin Source File

SOURCE=.\lib.z\zl_infutil.c
# End Source File
# Begin Source File

SOURCE=.\lib.z\zl_trees.c
# End Source File
# Begin Source File

SOURCE=.\lib.z\zl_uncompr.c
# End Source File
# Begin Source File

SOURCE=.\lib.z\zutil.c
# End Source File
# End Group
# Begin Group "ddraw"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lib.ddraw\creathcurso.cpp
# End Source File
# Begin Source File

SOURCE=.\lib.ddraw\ddraw_inst.cpp
# End Source File
# Begin Source File

SOURCE=.\lib.ddraw\ddraw_inst.h
# End Source File
# Begin Source File

SOURCE=.\lib.ddraw\tfft_bitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\lib.ddraw\tfft_dcpool.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\colorXtoX.cpp
# End Source File
# Begin Source File

SOURCE=.\picture.cpp
# End Source File
# Begin Source File

SOURCE=.\picture_bmp.cpp
# End Source File
# Begin Source File

SOURCE=.\picture_jpg.cpp
# End Source File
# Begin Source File

SOURCE=.\picture_png.cpp
# End Source File
# Begin Source File

SOURCE=.\picture_tga.cpp
# End Source File
# Begin Source File

SOURCE=.\reduction12.cpp
# End Source File
# Begin Source File

SOURCE=.\tffmedia.cpp
# End Source File
# Begin Source File

SOURCE=.\tffmedia.h
# End Source File
# End Target
# End Project
