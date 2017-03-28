# Microsoft Developer Studio Project File - Name="ray" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ray - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ray.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ray.mak" CFG="ray - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ray - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ray - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ray - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /Ob2 /I "local\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "SAMPLE_SOLUTION" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 fltkd.lib wsock32.lib opengl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /profile /map /machine:I386 /nodefaultlib:"libcmt" /libpath:"local\lib"

!ELSEIF  "$(CFG)" == "ray - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GR /GX /ZI /Od /I "local\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_WINDOWS" /D "SAMPLE_SOLUTION" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 local/lib/fltkd.lib wsock32.lib opengl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcmtd" /libpath:"local\lib"
# SUBTRACT LINK32 /profile /map

!ENDIF 

# Begin Target

# Name "ray - Win32 Release"
# Name "ray - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "ui"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\ui\TraceGLWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ui\TraceUI.cpp
# End Source File
# End Group
# Begin Group "fileio"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\fileio\bitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\src\fileio\parse.cpp
# End Source File
# Begin Source File

SOURCE=.\src\fileio\read.cpp
# End Source File
# End Group
# Begin Group "vecmath"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\vecmath\vecmath.cpp
# End Source File
# End Group
# Begin Group "scene"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\scene\camera.cpp
# End Source File
# Begin Source File

SOURCE=.\src\scene\light.cpp
# End Source File
# Begin Source File

SOURCE=.\src\scene\material.cpp
# End Source File
# Begin Source File

SOURCE=.\src\scene\ray.cpp
# End Source File
# Begin Source File

SOURCE=.\src\scene\scene.cpp
# End Source File
# End Group
# Begin Group "SceneObjects"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\SceneObjects\Box.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SceneObjects\Cone.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SceneObjects\Cylinder.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SceneObjects\Sphere.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SceneObjects\Square.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SceneObjects\trimesh.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\getopt.cpp
# End Source File
# Begin Source File

SOURCE=.\src\main.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RayTracer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "ui."

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\ui\TraceGLWindow.h
# End Source File
# Begin Source File

SOURCE=.\src\ui\TraceUI.h
# End Source File
# End Group
# Begin Group "fileio."

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\fileio\bitmap.h
# End Source File
# Begin Source File

SOURCE=.\src\fileio\parse.h
# End Source File
# Begin Source File

SOURCE=.\src\fileio\read.h
# End Source File
# End Group
# Begin Group "vecmath."

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\vecmath\vecmath.h
# End Source File
# End Group
# Begin Group "scene."

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\scene\camera.h
# End Source File
# Begin Source File

SOURCE=.\src\scene\light.h
# End Source File
# Begin Source File

SOURCE=.\src\scene\material.h
# End Source File
# Begin Source File

SOURCE=.\src\scene\ray.h
# End Source File
# Begin Source File

SOURCE=.\src\scene\scene.h
# End Source File
# End Group
# Begin Group "SceneObjects."

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\SceneObjects\Box.h
# End Source File
# Begin Source File

SOURCE=.\src\SceneObjects\Cone.h
# End Source File
# Begin Source File

SOURCE=.\src\SceneObjects\Cylinder.h
# End Source File
# Begin Source File

SOURCE=.\src\SceneObjects\Sphere.h
# End Source File
# Begin Source File

SOURCE=.\src\SceneObjects\Square.h
# End Source File
# Begin Source File

SOURCE=.\src\SceneObjects\trimesh.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\RayTracer.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\Makefile
# End Source File
# End Target
# End Project
