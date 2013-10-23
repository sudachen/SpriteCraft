# Microsoft Developer Studio Project File - Name="ScraftE_LIB" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=ScraftE_LIB - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ScraftE_LIB.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ScraftE_LIB.mak" CFG="ScraftE_LIB - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ScraftE_LIB - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "ScraftE_LIB - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "ScraftE_LIB - Win32 Release"

# PROP Use_Debug_Libraries 0
# PROP Output_Dir "."
# PROP Intermediate_Dir "."
# PROP Cmd_Line "..\..\My.Tools\builder.py  -j _CL_COMPILER_VERSION=6 -f DLL.BuildRules_LIB release"
# PROP Rebuild_Opt "rebuild"
# PROP Target_File "scrafte.lib"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "ScraftE_LIB - Win32 Debug"

# PROP Use_Debug_Libraries 1
# PROP Output_Dir "."
# PROP Intermediate_Dir "."
# PROP Cmd_Line "..\..\My.Tools\builder.py  -j _CL_COMPILER_VERSION=6 -f BuildRules_LIB debug"
# PROP Rebuild_Opt "rebuild"
# PROP Target_File "scrafte.lib"
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "ScraftE_LIB - Win32 Release"
# Name "ScraftE_LIB - Win32 Debug"

!IF  "$(CFG)" == "ScraftE_LIB - Win32 Release"

!ELSEIF  "$(CFG)" == "ScraftE_LIB - Win32 Debug"

!ENDIF 

# Begin Group "Level Hi"

# PROP Default_Filter ""
# Begin Group "Engine"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SpriteCraft.SRC\console.cpp
# End Source File
# Begin Source File

SOURCE=.\spritecraft.src\console.h
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\Engine.cpp
# End Source File
# Begin Source File

SOURCE=.\spritecraft.src\Engine.h
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\Engine_DInput.cpp
# End Source File
# Begin Source File

SOURCE=.\spritecraft.src\Engine_DInput.h
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\Engine_globals.cpp
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\Engine_input.cpp
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\Engine_timer.cpp
# End Source File
# Begin Source File

SOURCE=.\spritecraft.src\Engine_timer.h
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\GenericExecutor.cpp
# End Source File
# End Group
# Begin Group "Sprites"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SpriteCraft.SRC\SpriteHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\spritecraft.src\SpriteHelper.h
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\SpriteHierarchy.cpp
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\SpriteLwsArray.cpp
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\SpritePrimitive.cpp
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\SpritesEnumerator.cpp
# End Source File
# Begin Source File

SOURCE=.\spritecraft.src\SpritesEnumerator.h
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\SpriteSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\SpriteTextView.cpp
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\TileMap.cpp
# End Source File
# Begin Source File

SOURCE=.\spritecraft.src\TileMap.h
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\WeakSprite.cpp
# End Source File
# End Group
# Begin Group "Managers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SpriteCraft.SRC\AnimationMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\spritecraft.src\AnimationMgr.h
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\CollideMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\spritecraft.src\CollideMgr.h
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\MListenMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\spritecraft.src\MListenMgr.h
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\SoundMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\spritecraft.src\SoundMgr.h
# End Source File
# End Group
# Begin Group "Dispatchers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SpriteCraft.SRC\dispsubimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\spritecraft.src\dispsubimpl.h
# End Source File
# End Group
# Begin Group "Lists"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SpriteCraft.SRC\ObjList.cpp
# End Source File
# Begin Source File

SOURCE=.\spritecraft.src\ObjList.inl
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\SpritesList.cpp
# End Source File
# Begin Source File

SOURCE=.\spritecraft.src\SpritesList.h
# End Source File
# End Group
# Begin Group "Particles"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SpriteCraft.SRC\Particles.cpp
# End Source File
# Begin Source File

SOURCE=.\spritecraft.src\Particles.h
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\ParticlesEnumerator.cpp
# End Source File
# Begin Source File

SOURCE=.\spritecraft.src\ParticlesEnumerator.h
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\ps_default.c
# End Source File
# Begin Source File

SOURCE=.\spritecraft.src\ps_default.ps
# End Source File
# End Group
# Begin Group "Util Classes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SpriteCraft.SRC\Randomizer.cpp
# End Source File
# Begin Source File

SOURCE=.\spritecraft.src\Randomizer.h
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\RBGFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\spritecraft.src\RBGFilter.h
# End Source File
# End Group
# Begin Group "Hi Level Xdata"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SpriteCraft.SRC\xdata_cow.cpp
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\xdata_cow.h
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\XdataHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\XdataHelper.h
# End Source File
# End Group
# End Group
# Begin Group "Level Low"

# PROP Default_Filter ""
# Begin Group "Sound System"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SpriteCraft.SRC\SND_dxs_factory.cpp
# End Source File
# Begin Source File

SOURCE=.\spritecraft.src\SND_internal.h
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\SND_sndsubsys.cpp
# End Source File
# Begin Source File

SOURCE=.\spritecraft.src\SND_sound.h
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\SND_stream_ogg.cpp
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\SND_stream_wav.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\SpriteCraft.SRC\SST.cpp
# End Source File
# Begin Source File

SOURCE=.\spritecraft.src\SST.h
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SpriteCraft.SRC\loginfo.cpp
# End Source File
# Begin Source File

SOURCE=.\spritecraft.src\logout.h
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\scerror.cpp
# End Source File
# Begin Source File

SOURCE=.\spritecraft.src\scerror.h
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\spritecraft.idl
# End Source File
# End Group
# Begin Group "Flat"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SpriteCraft.SRC\flat_colide.cpp
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\flat_display.cpp
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\flat_e.cpp
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\flat_e.h
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\flat_e0.h
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\flat_e3.cpp
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\flat_e3.h
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\flat_inst.cpp
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\flat_layout.cpp
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\flat_lwsa.cpp
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\flat_lwsa.inl
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\flat_restor.cpp
# End Source File
# End Group
# Begin Group "python_over_com"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\python_over_com\kaa.h
# End Source File
# Begin Source File

SOURCE=.\python_over_com\py_external_module.cpp
# End Source File
# Begin Source File

SOURCE=.\python_over_com\python_dispatcher.cpp
# End Source File
# Begin Source File

SOURCE=.\python_over_com\python_over_com.cpp
# End Source File
# Begin Source File

SOURCE=.\python_over_com\python_over_com.h
# End Source File
# Begin Source File

SOURCE=.\python_over_com\python_over_com_intern.h
# End Source File
# End Group
# Begin Group "Dsource"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SpriteCraft.SRC\dsscfilter.cpp
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\dsscfilter.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\BuildRules_LIB
# End Source File
# Begin Source File

SOURCE=.\ScraftAPI\spritecraft.h
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\synchronize.h
# End Source File
# End Target
# End Project
