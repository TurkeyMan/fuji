# Microsoft Developer Studio Project File - Name="minifmod" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=minifmod - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "minifmod.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "minifmod.mak" CFG="minifmod - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "minifmod - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "minifmod - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Perforce Project"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "minifmod - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"minifmod.lib" /LINK50COMPAT

!ELSEIF  "$(CFG)" == "minifmod - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"minifmod.lib"

!ENDIF 

# Begin Target

# Name "minifmod - Win32 Release"
# Name "minifmod - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=lib\Fmusic.c
# End Source File
# Begin Source File

SOURCE=lib\Fsound.c
# End Source File
# Begin Source File

SOURCE=lib\mixer_clipcopy.c
# End Source File
# Begin Source File

SOURCE=lib\mixer_fpu_ramp.c
# End Source File
# Begin Source File

SOURCE=lib\music_formatxm.c
# End Source File
# Begin Source File

SOURCE=lib\system_file.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=lib\minifmod.h
# End Source File
# Begin Source File

SOURCE=lib\Mixer.h
# End Source File
# Begin Source File

SOURCE=lib\mixer_clipcopy.h
# End Source File
# Begin Source File

SOURCE=lib\mixer_fpu_ramp.h
# End Source File
# Begin Source File

SOURCE=lib\Music.h
# End Source File
# Begin Source File

SOURCE=lib\music_formatxm.h
# End Source File
# Begin Source File

SOURCE=lib\Sound.h
# End Source File
# Begin Source File

SOURCE=lib\system_file.h
# End Source File
# Begin Source File

SOURCE=lib\system_memory.h
# End Source File
# Begin Source File

SOURCE=lib\xmeffects.h
# End Source File
# End Group
# End Target
# End Project
