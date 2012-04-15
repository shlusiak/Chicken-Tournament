/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "config.h"
#include "text.h"


TConfig Config;
BOOLEAN Loaded=FALSE;




void ZeroConfig()
{
	Config.chicken=10;
	Config.farmer=0;
	Config.trees=10;
	Config.stall=1;

	Config.sensitivity=30;
	Config.spielertyp=1;

	Config.height=100;
	Config.width=100;

	Config.resx=1024;
	Config.resy=768;
	Config.bpp=32;
	Config.Textures32Bit=FALSE;

	Config.fullscreen=TRUE;

	Config.federn=TRUE;
	Config.gibs=TRUE;
	Config.blut=TRUE;
	Config.ShowIntro=TRUE;
	Config.unrealext=TRUE;
	
	Config.reversemouse=FALSE;
	Config.reverseflight=TRUE;
	Config.soundon=TRUE;
	Config.Musik=TRUE;
	Config.alpha=TRUE;
	Config.smoothmouse=TRUE;

	Config.golfergebnisse=TRUE;

	Config.gackern=TRUE;
	Config.vsync=TRUE;

	Config.KI=TRUE;
	Config.Crosshair=TRUE;
	Config.reflections=TRUE;
	Config.Animations=TRUE;
	Config.Gras=FALSE;
	Config.ShowPlayernames=TRUE;

	Config.fire=TRUE;

	DWORD l=MAX_COMPUTERNAME_LENGTH+1;
	CHAR cn[MAX_COMPUTERNAME_LENGTH+1];
	if (GetComputerName(&cn[0],&l)==TRUE)
	{
		if (strlen(cn)>1)
			_tcslwr(cn+1);
		strcpy(&Config.Playername[0],&cn[0]);
	}else strcpy(&Config.Playername[0],"Default");
}

const BOOLEAN LoadConfig()
{
	ZeroMemory(&Config,sizeof(Config));

	HANDLE file=CreateFile(T_CONFIGFILE,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,0);
	if (file==INVALID_HANDLE_VALUE)
	{	// Datei nicht gefunden. Standardparameter einstellen.
dochnicht:
		ZeroConfig();

		DebugOut("Config-Datei nicht geladen!");
		Loaded=TRUE;
		return FALSE;
	}

	DWORD read;
	ReadFile(file,&Config,sizeof(Config),&read,NULL);

	CloseHandle(file);
	if (read!=sizeof(Config))goto dochnicht;

	DebugOut("Configuration geladen!");
	Loaded=TRUE;
	return TRUE;
}

const BOOLEAN SaveConfig()
{
	if (Loaded==FALSE)return FALSE;
	HANDLE file=CreateFile(T_CONFIGFILE,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,0);
#ifdef _DEBUG
	int e=GetLastError();
#endif
	if (file==INVALID_HANDLE_VALUE)
	{	// Datei nicht erstellt. 
#ifdef _DEBUG
		CHAR c[100];
		ConvertLastErrorToString(&c[0],100);
		DebugOut("Config-Datei nicht gespeichert! Konnte nicht erstellt werden.");
		DebugOut(&c[0]);
#endif
		return FALSE;
	}

	DWORD written;
	WriteFile(file,&Config,sizeof(Config),&written,NULL);

	CloseHandle(file);
	DebugOut("Configuration gespeichert!");
	return TRUE;
}
