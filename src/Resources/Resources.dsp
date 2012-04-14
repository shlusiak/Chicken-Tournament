# Microsoft Developer Studio Project File - Name="Resources" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Resources - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "Resources.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "Resources.mak" CFG="Resources - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "Resources - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Resources - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Resources - Win32 Release Lite" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/KFC/Resources", BDAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Resources - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RESOURCES_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Ox /Og /Oi /Os /Op /Oy- /Gf /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RESOURCES_EXPORTS" /D "UTSOUNDS" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Ot
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG" /d "UTSOUNDS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib dsound.lib dxguid.lib /nologo /dll /machine:I386 /def:".\Resources.def" /out:"../Resources.dll" /implib:"../Resources.lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Resources - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RESOURCES_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RESOURCES_EXPORTS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG" /d "IMAGES" /d "UTSOUNDS" /d "ANNELI"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib dsound.lib dxguid.lib /nologo /dll /debug /machine:I386 /out:"../Resources.dll" /implib:"../Resources.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Resources - Win32 Release Lite"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Resources___Win32_Release_Lite"
# PROP BASE Intermediate_Dir "Resources___Win32_Release_Lite"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Resources___Win32_Release_Lite"
# PROP Intermediate_Dir "Resources___Win32_Release_Lite"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /Ox /Og /Oi /Os /Op /Oy- /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RESOURCES_EXPORTS" /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /Ot
# ADD CPP /nologo /MT /W3 /GX /Ox /Og /Oi /Os /Op /Oy- /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RESOURCES_EXPORTS" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Ot
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG" /d "IMAGES"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib dsound.lib dxguid.lib /nologo /dll /machine:I386 /def:".\Resources.def" /out:"../Resources.dll" /implib:"../Resources.lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib dsound.lib dxguid.lib /nologo /dll /machine:I386 /def:".\Resources.def" /out:"../Resources.dll" /implib:"../Resources.lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Resources - Win32 Release"
# Name "Resources - Win32 Debug"
# Name "Resources - Win32 Release Lite"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Images.cpp
# End Source File
# Begin Source File

SOURCE=.\MMIO.cpp
# End Source File
# Begin Source File

SOURCE=.\Packages.cpp
# End Source File
# Begin Source File

SOURCE=.\Resources.cpp
# End Source File
# Begin Source File

SOURCE=.\Resources.def

!IF  "$(CFG)" == "Resources - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Resources - Win32 Debug"

!ELSEIF  "$(CFG)" == "Resources - Win32 Release Lite"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Resources.rc
# End Source File
# Begin Source File

SOURCE=.\Sound.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Wave.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\MMIO.h
# End Source File
# Begin Source File

SOURCE=.\Resources.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Wave.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\bark.JPG
# End Source File
# Begin Source File

SOURCE=.\bodysplat.wav
# End Source File
# Begin Source File

SOURCE=.\brathuhn.jpg
# End Source File
# Begin Source File

SOURCE=.\COOL.wav
# End Source File
# Begin Source File

SOURCE=.\cur00001.cur
# End Source File
# Begin Source File

SOURCE=.\cursor1.bmp
# End Source File
# Begin Source File

SOURCE=.\cursor2.bmp
# End Source File
# Begin Source File

SOURCE=.\cursor3.bmp
# End Source File
# Begin Source File

SOURCE=.\cursor4.bmp
# End Source File
# Begin Source File

SOURCE=.\cursor5.bmp
# End Source File
# Begin Source File

SOURCE=.\denied.wav
# End Source File
# Begin Source File

SOURCE=.\dominating.wav
# End Source File
# Begin Source File

SOURCE=.\doublekill.wav
# End Source File
# Begin Source File

SOURCE=.\eisplat.wav
# End Source File
# Begin Source File

SOURCE=.\engine.wav
# End Source File
# Begin Source File

SOURCE=.\fire.jpg
# End Source File
# Begin Source File

SOURCE=.\fire.wav
# End Source File
# Begin Source File

SOURCE=.\flare0.jpg
# End Source File
# Begin Source File

SOURCE=.\flare1.JPG
# End Source File
# Begin Source File

SOURCE=.\flare2.JPG
# End Source File
# Begin Source File

SOURCE=.\flare3.JPG
# End Source File
# Begin Source File

SOURCE=.\fork.wav
# End Source File
# Begin Source File

SOURCE=.\GLUCKE.WAV
# End Source File
# Begin Source File

SOURCE=.\glucke2.wav
# End Source File
# Begin Source File

SOURCE=.\godlike.wav
# End Source File
# Begin Source File

SOURCE=.\ground_small.JPG
# End Source File
# Begin Source File

SOURCE=.\HolyShit.wav
# End Source File
# Begin Source File

SOURCE=.\ICON1.ICO
# End Source File
# Begin Source File

SOURCE=.\image1.jpg
# End Source File
# Begin Source File

SOURCE=.\image10.jpg
# End Source File
# Begin Source File

SOURCE=.\image11.jpg
# End Source File
# Begin Source File

SOURCE=.\image12.jpg
# End Source File
# Begin Source File

SOURCE=.\image13.jpg
# End Source File
# Begin Source File

SOURCE=.\image14.jpg
# End Source File
# Begin Source File

SOURCE=.\image15.jpg
# End Source File
# Begin Source File

SOURCE=.\image2.jpg
# End Source File
# Begin Source File

SOURCE=.\image3.jpg
# End Source File
# Begin Source File

SOURCE=.\image4.jpg
# End Source File
# Begin Source File

SOURCE=.\image5.JPG
# End Source File
# Begin Source File

SOURCE=.\image6.JPG
# End Source File
# Begin Source File

SOURCE=.\image7.JPG
# End Source File
# Begin Source File

SOURCE=.\image8.JPG
# End Source File
# Begin Source File

SOURCE=.\image9.jpg
# End Source File
# Begin Source File

SOURCE=.\item.wav
# End Source File
# Begin Source File

SOURCE=.\jump.wav
# End Source File
# Begin Source File

SOURCE=.\killingspree.wav
# End Source File
# Begin Source File

SOURCE=.\LudicrousKill.wav
# End Source File
# Begin Source File

SOURCE=.\medic.jpg
# End Source File
# Begin Source File

SOURCE=.\megakill.wav
# End Source File
# Begin Source File

SOURCE=.\metal.wav
# End Source File
# Begin Source File

SOURCE=.\models.x
# End Source File
# Begin Source File

SOURCE=.\monsterkill.wav
# End Source File
# Begin Source File

SOURCE=.\multikill.wav
# End Source File
# Begin Source File

SOURCE=..\Musik\Music.dat
# End Source File
# Begin Source File

SOURCE=.\plasma.jpg
# End Source File
# Begin Source File

SOURCE=..\sprites\plasma.spr
# End Source File
# Begin Source File

SOURCE=.\Plasma.wav
# End Source File
# Begin Source File

SOURCE=.\plasma2.wav
# End Source File
# Begin Source File

SOURCE=.\rampage.wav
# End Source File
# Begin Source File

SOURCE=.\Sky0.jpg
# End Source File
# Begin Source File

SOURCE=.\Sky1.jpg
# End Source File
# Begin Source File

SOURCE=.\Sky2.jpg
# End Source File
# Begin Source File

SOURCE=.\Sky3.jpg
# End Source File
# Begin Source File

SOURCE=.\Sky4.jpg
# End Source File
# Begin Source File

SOURCE=.\Sky5.jpg
# End Source File
# Begin Source File

SOURCE=.\spheremap.jpg
# End Source File
# Begin Source File

SOURCE=.\splash.jpg
# End Source File
# Begin Source File

SOURCE=.\splat.wav
# End Source File
# Begin Source File

SOURCE=.\stall.jpg
# End Source File
# Begin Source File

SOURCE=.\step1.wav
# End Source File
# Begin Source File

SOURCE=.\step2.wav
# End Source File
# Begin Source File

SOURCE=.\step3.wav
# End Source File
# Begin Source File

SOURCE=.\step4.wav
# End Source File
# Begin Source File

SOURCE=.\stripes.jpg
# End Source File
# Begin Source File

SOURCE=.\throw.wav
# End Source File
# Begin Source File

SOURCE=.\ultrakill.wav
# End Source File
# Begin Source File

SOURCE=.\unstoppable.wav
# End Source File
# Begin Source File

SOURCE=.\wade.wav
# End Source File
# Begin Source File

SOURCE=.\WhickedSick.wav
# End Source File
# End Group
# End Target
# End Project
