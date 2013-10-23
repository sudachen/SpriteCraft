# Microsoft Developer Studio Project File - Name="moleskin2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=MoleSkin2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MoleSkin2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MoleSkin2.mak" CFG="MoleSkin2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "moleskin2 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "moleskin2 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/spritecraft.4/moleskin2", CXAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "moleskin2 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../temp/Lib.Scraft3.MoleSkin2.Release"
# PROP Intermediate_Dir "../../temp/Lib.Scraft3.MoleSkin2.Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /Gz /MD /W3 /Zi /Ox /Og /Oi /I ".." /I "../.." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"precompile.h" /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\moleskin2.lib"

!ELSEIF  "$(CFG)" == "moleskin2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../temp/Lib.Scraft3.MoleSkin2.Debug"
# PROP Intermediate_Dir "../../temp/Lib.Scraft3.MoleSkin2.Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /Gz /MTd /W3 /Gm /ZI /Od /I ".." /I "../.." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"precompile.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\MoleSkin2_d.lib"

!ENDIF 

# Begin Target

# Name "moleskin2 - Win32 Release"
# Name "moleskin2 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\nvmath\algebra.cpp
# End Source File
# Begin Source File

SOURCE=.\mstreams\embedded\arial14a.bgz.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\mstreams\embedded\arial14a.txt.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_bitblt.cpp
# End Source File
# Begin Source File

SOURCE=.\mcore\m_core.cpp
# End Source File
# Begin Source File

SOURCE=.\mstreams\m_cpystream.cpp
# End Source File
# Begin Source File

SOURCE=.\gfxd3d7\m_d3d7.cpp
# End Source File
# Begin Source File

SOURCE=.\gfxd3d7\m_d3d7_api.cpp
# End Source File
# Begin Source File

SOURCE=.\gfxd3d7\m_d3d7_device.cpp
# End Source File
# Begin Source File

SOURCE=.\gfxd3d7\m_d3d7_light.cpp
# End Source File
# Begin Source File

SOURCE=.\gfxd3d7\m_d3d7_mode.cpp
# End Source File
# Begin Source File

SOURCE=.\gfxd3d7\m_d3d7_primitive.cpp
# End Source File
# Begin Source File

SOURCE=.\gfxd3d7\m_d3d7_states.cpp
# End Source File
# Begin Source File

SOURCE=.\gfxd3d7\m_d3d7_texture.cpp
# End Source File
# Begin Source File

SOURCE=.\gfxd3d7\m_d3d7_transform.cpp
# End Source File
# Begin Source File

SOURCE=.\mcore\m_debug.cpp
# End Source File
# Begin Source File

SOURCE=.\mutil\m_def_parser.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\mstreams\m_dsoman.cpp
# End Source File
# Begin Source File

SOURCE=.\mstreams\m_dsource.cpp
# End Source File
# Begin Source File

SOURCE=.\mcore\m_eth.cpp
# End Source File
# Begin Source File

SOURCE=.\mstreams\m_fio.cpp
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_font_parser.cpp
# End Source File
# Begin Source File

SOURCE=.\mcore\m_format.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\mutil\m_fps.cpp
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_gfx.cpp
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_gfx3d.cpp
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_glyphsource.cpp
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_glyphsource_marked.cpp
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_glyphsource_ttf.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\mstreams\m_igzstream.cpp
# End Source File
# Begin Source File

SOURCE=.\mstreams\m_istream.cpp
# End Source File
# Begin Source File

SOURCE=.\mcore\m_lh.cpp
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_loadmodel.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_loadmodel_3ds.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_loadsurf.cpp
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_loadsurf_bmp.cpp
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_loadsurf_jpg.cpp
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_loadsurf_png.cpp
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_loadsurf_taf.cpp
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_loadsurf_tga.cpp
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_loadtexture.cpp
# End Source File
# Begin Source File

SOURCE=.\mcore\m_log.cpp
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_lwsa_basic.cpp
# End Source File
# Begin Source File

SOURCE=.\mcore\m_mem.cpp
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_model3d.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\mstreams\m_ogzstream.cpp
# End Source File
# Begin Source File

SOURCE=.\mstreams\m_ostream.cpp
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_particle.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\mutil\m_properties.cpp
# End Source File
# Begin Source File

SOURCE=.\mutil\m_randomizer.cpp
# End Source File
# Begin Source File

SOURCE=.\mutil\m_roundpow2.cpp
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_screen.cpp
# End Source File
# Begin Source File

SOURCE=.\mcore\m_singletons.cpp
# End Source File
# Begin Source File

SOURCE=.\mstreams\m_slots.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_sprites.cpp
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_surface.cpp
# End Source File
# Begin Source File

SOURCE=.\mcore\m_symbols.cpp
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_texfont2.cpp
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_tilemap.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\mcore\m_time.cpp
# End Source File
# Begin Source File

SOURCE=.\mstreams\m_wcharsbuf.cpp
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_win32wnd.cpp
# End Source File
# Begin Source File

SOURCE=.\mutil\m_xml.cpp
# End Source File
# Begin Source File

SOURCE=.\mutil\m_xml_parser.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\mstreams\m_zipdiskfile.cpp
# End Source File
# Begin Source File

SOURCE=.\precompile.cpp
# ADD CPP /Yc"precompile.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\nvmath\algebra.h
# End Source File
# Begin Source File

SOURCE=.\mcore\m_autoconf.h
# End Source File
# Begin Source File

SOURCE=.\mcore\m_automem.h
# End Source File
# Begin Source File

SOURCE=.\mcore\m_autoref.h
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_color.h
# End Source File
# Begin Source File

SOURCE=.\mutil\m_com.h
# End Source File
# Begin Source File

SOURCE=.\mcore\m_core.h
# End Source File
# Begin Source File

SOURCE=.\mstreams\m_cpystream.h
# End Source File
# Begin Source File

SOURCE=.\gfxd3d7\m_d3d7_debug.h
# End Source File
# Begin Source File

SOURCE=.\gfxd3d7\m_d3d7_device.h
# End Source File
# Begin Source File

SOURCE=.\mcore\m_debug.h
# End Source File
# Begin Source File

SOURCE=.\mcore\m_detect_compiler.h
# End Source File
# Begin Source File

SOURCE=.\mstreams\m_dsource.h
# End Source File
# Begin Source File

SOURCE=.\mcore\m_errors.h
# End Source File
# Begin Source File

SOURCE=.\mcore\m_eth.h
# End Source File
# Begin Source File

SOURCE=.\mcore\m_fastcopy.h
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_font.h
# End Source File
# Begin Source File

SOURCE=.\mutil\m_fps.h
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_gfx.h
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_gfx3d.h
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_glyphsource.h
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_graphics.h
# End Source File
# Begin Source File

SOURCE=.\mcore\m_hmgr.h
# End Source File
# Begin Source File

SOURCE=.\mstreams\m_igzstream.h
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_input.h
# End Source File
# Begin Source File

SOURCE=.\mcore\m_irefobj.h
# End Source File
# Begin Source File

SOURCE=.\mstreams\m_istream.h
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_loadmodel.h
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_loadsurf.h
# End Source File
# Begin Source File

SOURCE=.\mcore\m_log.h
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_lwsa_basic.inl
# End Source File
# Begin Source File

SOURCE=.\mutil\m_math2d.h
# End Source File
# Begin Source File

SOURCE=.\mcore\m_nonstatic.h
# End Source File
# Begin Source File

SOURCE=.\mstreams\m_ogzstream.h
# End Source File
# Begin Source File

SOURCE=.\mstreams\m_ostream.h
# End Source File
# Begin Source File

SOURCE=.\mcore\m_process.h
# End Source File
# Begin Source File

SOURCE=.\mutil\m_properties.h
# End Source File
# Begin Source File

SOURCE=.\mutil\m_randomizer.h
# End Source File
# Begin Source File

SOURCE=.\mutil\m_roundpow2.h
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_screen.h
# End Source File
# Begin Source File

SOURCE=.\mcore\m_singletons.h
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_sprites.h
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_sprites0.h
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_sprites3d.h
# End Source File
# Begin Source File

SOURCE=.\mcore\m_static.h
# End Source File
# Begin Source File

SOURCE=.\mstreams\m_streams.h
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_surface.h
# End Source File
# Begin Source File

SOURCE=.\mcore\m_sync.h
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_texfont.h
# End Source File
# Begin Source File

SOURCE=.\mcore\m_time.h
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_types.h
# End Source File
# Begin Source File

SOURCE=.\mcore\m_vsnprintf.h
# End Source File
# Begin Source File

SOURCE=.\mstreams\m_wcharsbuf.h
# End Source File
# Begin Source File

SOURCE=.\mgraphics\m_win32wnd.h
# End Source File
# Begin Source File

SOURCE=.\mutil\m_xml.h
# End Source File
# Begin Source File

SOURCE=.\mstreams\m_xxconf.h
# End Source File
# Begin Source File

SOURCE=.\mstreams\m_zipdiskfile.h
# End Source File
# Begin Source File

SOURCE=.\mcore.h
# End Source File
# Begin Source File

SOURCE=.\mgraphics.h
# End Source File
# Begin Source File

SOURCE=.\moleskin2.h
# End Source File
# Begin Source File

SOURCE=.\msound.h
# End Source File
# Begin Source File

SOURCE=.\mstreams.h
# End Source File
# Begin Source File

SOURCE=.\mutil.h
# End Source File
# Begin Source File

SOURCE=.\precompile.h
# End Source File
# End Group
# End Target
# End Project
