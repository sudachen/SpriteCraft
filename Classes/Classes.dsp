# Microsoft Developer Studio Project File - Name="Classes" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=Classes - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Classes.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Classes.mak" CFG="Classes - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Classes - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "Classes - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "Classes - Win32 Release"

# PROP Use_Debug_Libraries 0
# PROP Output_Dir "."
# PROP Intermediate_Dir "."
# PROP Cmd_Line "..\..\My.Tools\builder.py release"
# PROP Rebuild_Opt "rebuild"
# PROP Target_File "Classes"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "Classes - Win32 Debug"

# PROP Use_Debug_Libraries 1
# PROP Output_Dir "."
# PROP Intermediate_Dir "."
# PROP Cmd_Line "..\..\My.Tools\builder.py debug"
# PROP Rebuild_Opt "rebuild"
# PROP Target_File "Classes"
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "Classes - Win32 Release"
# Name "Classes - Win32 Debug"

!IF  "$(CFG)" == "Classes - Win32 Release"

!ELSEIF  "$(CFG)" == "Classes - Win32 Debug"

!ENDIF 

# Begin Group "_Library"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Classes.SRC\_adler32.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\_codecop.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\_crc32.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\_specific.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\collection.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\com_dynamic.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\com_register.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\com_support.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\defparser.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\format.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\genericio.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\hinstance.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\keylist.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\logger.cpp
# End Source File
# Begin Source File

SOURCE=".\Classes.SRC\lz+.cpp"
# End Source File
# Begin Source File

SOURCE=".\Classes.SRC\lz+decoder.cpp"
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\lzss.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\messages.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\newdes.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\saxparser.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\streams.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\string.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\symboltable.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\sysutil.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\xdom.cpp
# End Source File
# End Group
# Begin Group "_Core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Classes.SRC\_adler32.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\_algo.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\_avltree.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\_codecop.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\_codecop.inl
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\_crc32.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\_cxxproperties.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\_qsort.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\_shash.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\_specific.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\_specific.inl
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\_strategy_copy.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\_template.h
# End Source File
# End Group
# Begin Group "Containers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Classes.SRC\array.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\collection.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\collection.inl
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\flatmap.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\flatset.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\keylist.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\keylist.inl
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\slist.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\splitter.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\stack.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\tuple.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\xhash.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\xhash.inl
# End Source File
# End Group
# Begin Group "COM"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Classes.SRC\com_dynamic.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\com_dynamic.inl
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\com_generic.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\com_register.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\com_register.inl
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\guid.h
# End Source File
# End Group
# Begin Group "Text"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Classes.SRC\format.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\genericio.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\genericio.inl
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\string.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\string.inl
# End Source File
# End Group
# Begin Group "Crypto"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Classes.SRC\md5.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\newdes.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\newdes.inl
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\xoredstring.h
# End Source File
# End Group
# Begin Group "Math"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Classes.SRC\mymath.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\polygon.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\rectangle.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\vector.h
# End Source File
# End Group
# Begin Group "Smart Pointers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Classes.SRC\ptr_mem.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\ptr_rcc.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\refcounted.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\weaklink.h
# End Source File
# End Group
# Begin Group "Xdocument"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Classes.SRC\defparser.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\defparser.inl
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\saxparser.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\saxparser.inl
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\xdom.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\xdom.inl
# End Source File
# End Group
# Begin Group "IO Streams"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Classes.SRC\streams.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\streams.inl
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\streams_u2.inl
# End Source File
# End Group
# Begin Group "System"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Classes.SRC\hinstance.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\logger.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\logger.inl
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\sysutil.h
# End Source File
# End Group
# Begin Group "Signals & Actions"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Classes.SRC\messages.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\messages.inl
# End Source File
# End Group
# Begin Group "Syntax"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Classes.SRC\ast.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\symboltable.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\symboltable.inl
# End Source File
# End Group
# Begin Group "Compressor"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\Classes.SRC\lz+.h"
# End Source File
# Begin Source File

SOURCE=".\Classes.SRC\lz+.inl"
# End Source File
# Begin Source File

SOURCE=".\Classes.SRC\lz+decoder.inl"
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\lzsh.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\lzss.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\lzss.inl
# End Source File
# End Group
# Begin Source File

SOURCE=.\BuildRules
# End Source File
# Begin Source File

SOURCE=.\classes.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\detect_compiler.h
# End Source File
# Begin Source File

SOURCE=.\Classes.SRC\wexcept.h
# End Source File
# End Target
# End Project
