# Microsoft Developer Studio Project File - Name="ScraftE" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=ScraftE - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ScraftE.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ScraftE.mak" CFG="ScraftE - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ScraftE - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "ScraftE - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "ScraftE - Win32 Release"

# PROP Use_Debug_Libraries 0
# PROP Output_Dir "."
# PROP Intermediate_Dir "."
# PROP Cmd_Line "..\..\My.Tools\builder.py  release"
# PROP Rebuild_Opt "rebuild"
# PROP Target_File "scrafte.dll"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "ScraftE - Win32 Debug"

# PROP Use_Debug_Libraries 1
# PROP Output_Dir "."
# PROP Intermediate_Dir "."
# PROP Cmd_Line "..\..\My.Tools\builder.py  -j _CL_COMPILER_VERSION=6 debug"
# PROP Rebuild_Opt "rebuild"
# PROP Target_File "scraft.dll"
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "ScraftE - Win32 Release"
# Name "ScraftE - Win32 Debug"

!IF  "$(CFG)" == "ScraftE - Win32 Release"

!ELSEIF  "$(CFG)" == "ScraftE - Win32 Debug"

!ENDIF 

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
# Begin Source File

SOURCE=.\BuildRules
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\py_initscraft.cpp
# End Source File
# Begin Source File

SOURCE=.\ScraftE.rc
# End Source File
# Begin Source File

SOURCE=.\ScraftE.tlb
# End Source File
# Begin Source File

SOURCE=.\SpriteCraft.SRC\spritecraft.cpp
# End Source File
# Begin Source File

SOURCE=.\synchronize.h
# End Source File
# End Target
# End Project
