# Microsoft Developer Studio Project File - Name="CT" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=CT - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "CT.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "CT.mak" CFG="CT - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "CT - Win32 Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE "CT - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CT", BAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CT - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /Ox /Ot /Og /Oi /Op /Gf /Gy /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Oa /Os
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ddraw.lib d3dim.lib winmm.lib dxguid.lib d3dx.lib dinput.lib dsound.lib d3dxof.lib amstrmid.lib resources.lib /nologo /stack:0x1,0x1 /subsystem:windows /machine:I386 /out:"Release/CT.exe"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "CT - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ddraw.lib d3dim.lib winmm.lib dxguid.lib d3dx.lib dinput.lib dsound.lib d3dxof.lib amstrmid.lib helperlib.lib resources.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/CT.exe"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "CT - Win32 Release"
# Name "CT - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Animation.cpp
# End Source File
# Begin Source File

SOURCE=.\Bauer.cpp
# End Source File
# Begin Source File

SOURCE=.\Camera.cpp
# End Source File
# Begin Source File

SOURCE=.\ChatWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\Chicken.cpp
# End Source File
# Begin Source File

SOURCE=.\ChickenPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\Collision.cpp
# End Source File
# Begin Source File

SOURCE=.\ComputerBauer.cpp
# End Source File
# Begin Source File

SOURCE=.\Config.cpp
# End Source File
# Begin Source File

SOURCE=.\Console.cpp
# End Source File
# Begin Source File

SOURCE=.\Drescher.cpp
# End Source File
# Begin Source File

SOURCE=.\Effects.cpp
# End Source File
# Begin Source File

SOURCE=.\Ei.cpp
# End Source File
# Begin Source File

SOURCE=.\EnvMap.cpp
# End Source File
# Begin Source File

SOURCE=.\Fire.cpp
# End Source File
# Begin Source File

SOURCE=.\Game.cpp
# End Source File
# Begin Source File

SOURCE=.\GameInit.cpp
# End Source File
# Begin Source File

SOURCE=.\Gartenkralle.cpp
# End Source File
# Begin Source File

SOURCE=.\Gibs.cpp
# End Source File
# Begin Source File

SOURCE=.\Golfer.cpp
# End Source File
# Begin Source File

SOURCE=.\Gras.cpp
# End Source File
# Begin Source File

SOURCE=.\Items.cpp
# End Source File
# Begin Source File

SOURCE=.\ki.cpp
# End Source File
# Begin Source File

SOURCE=.\LensFlare.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\Menu.cpp
# End Source File
# Begin Source File

SOURCE=.\Multiplayer.cpp
# End Source File
# Begin Source File

SOURCE=.\Music.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjChain.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects.cpp
# End Source File
# Begin Source File

SOURCE=.\ParticleSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\PlasmaCannon.cpp
# End Source File
# Begin Source File

SOURCE=.\Player.cpp
# End Source File
# Begin Source File

SOURCE=.\Radar.cpp
# End Source File
# Begin Source File

SOURCE=.\ServerWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Shadows.cpp
# End Source File
# Begin Source File

SOURCE=.\Sounds.cpp
# End Source File
# Begin Source File

SOURCE=.\Sprites.cpp
# End Source File
# Begin Source File

SOURCE=.\Stall.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Text.cpp
# End Source File
# Begin Source File

SOURCE=.\Transitions.cpp
# End Source File
# Begin Source File

SOURCE=.\Tree.cpp
# End Source File
# Begin Source File

SOURCE=.\ui.cpp
# End Source File
# Begin Source File

SOURCE=.\Weapon.cpp
# End Source File
# Begin Source File

SOURCE=.\wnd.cpp
# End Source File
# Begin Source File

SOURCE=.\World.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Animation.h
# End Source File
# Begin Source File

SOURCE=.\Bauer.h
# End Source File
# Begin Source File

SOURCE=.\Camera.h
# End Source File
# Begin Source File

SOURCE=.\ChatWindow.h
# End Source File
# Begin Source File

SOURCE=.\Chicken.h
# End Source File
# Begin Source File

SOURCE=.\ChickenPlayer.h
# End Source File
# Begin Source File

SOURCE=.\Collision.h
# End Source File
# Begin Source File

SOURCE=.\ComputerBauer.h
# End Source File
# Begin Source File

SOURCE=.\Config.h
# End Source File
# Begin Source File

SOURCE=.\Console.h
# End Source File
# Begin Source File

SOURCE=.\Drescher.h
# End Source File
# Begin Source File

SOURCE=.\Effects.h
# End Source File
# Begin Source File

SOURCE=.\Ei.h
# End Source File
# Begin Source File

SOURCE=.\EnvMap.h
# End Source File
# Begin Source File

SOURCE=.\Fire.h
# End Source File
# Begin Source File

SOURCE=.\Game.h
# End Source File
# Begin Source File

SOURCE=.\Gartenkralle.h
# End Source File
# Begin Source File

SOURCE=.\General.h
# End Source File
# Begin Source File

SOURCE=.\Gibs.h
# End Source File
# Begin Source File

SOURCE=.\Golfer.h
# End Source File
# Begin Source File

SOURCE=.\Gras.h
# End Source File
# Begin Source File

SOURCE=.\ids.h
# End Source File
# Begin Source File

SOURCE=.\Items.h
# End Source File
# Begin Source File

SOURCE=.\ki.h
# End Source File
# Begin Source File

SOURCE=.\Lensflare.h
# End Source File
# Begin Source File

SOURCE=.\Multiplayer.h
# End Source File
# Begin Source File

SOURCE=.\Music.h
# End Source File
# Begin Source File

SOURCE=.\ObjChain.h
# End Source File
# Begin Source File

SOURCE=.\Objects.h
# End Source File
# Begin Source File

SOURCE=.\ParticleSystem.h
# End Source File
# Begin Source File

SOURCE=.\PlasmaCannon.h
# End Source File
# Begin Source File

SOURCE=.\Player.h
# End Source File
# Begin Source File

SOURCE=.\Radar.h
# End Source File
# Begin Source File

SOURCE=.\ServerWnd.h
# End Source File
# Begin Source File

SOURCE=.\Shadows.h
# End Source File
# Begin Source File

SOURCE=.\Sounds.h
# End Source File
# Begin Source File

SOURCE=.\Sprites.h
# End Source File
# Begin Source File

SOURCE=.\Stall.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\text.h
# End Source File
# Begin Source File

SOURCE=.\Transitions.h
# End Source File
# Begin Source File

SOURCE=.\Tree.h
# End Source File
# Begin Source File

SOURCE=.\ui.h
# End Source File
# Begin Source File

SOURCE=.\Weapon.h
# End Source File
# Begin Source File

SOURCE=.\wnd.h
# End Source File
# Begin Source File

SOURCE=.\World.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\1.bin
# End Source File
# Begin Source File

SOURCE=.\ICON1.ICO
# End Source File
# Begin Source File

SOURCE=.\CT.RC
# End Source File
# Begin Source File

SOURCE=.\Resources\Resources.h
# End Source File
# End Group
# End Target
# End Project
