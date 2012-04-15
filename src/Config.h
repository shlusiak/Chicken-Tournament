/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _CONFIG_INCLUDED_
#define _CONFIG_INCLUDED_



struct TConfig
{
	CHAR Playername[32];
	int chicken,farmer,trees,stall;
	int width,height;
	int sensitivity;

	int resx,resy,bpp;
	BOOLEAN fullscreen;

	int spielertyp;

	BOOLEAN federn,gibs,blut;
	BOOLEAN ShowIntro;
	BOOLEAN vsync;
	BOOLEAN unrealext;
	BOOLEAN reversemouse,reverseflight,smoothmouse;
	BOOLEAN soundon,alpha;
	BOOLEAN golfergebnisse;
	BOOLEAN gackern;
	BOOLEAN KI;
	BOOLEAN reflections;
	BOOLEAN Crosshair;
	BOOLEAN Animations;
	BOOLEAN Gras,ShowPlayernames;
	BOOLEAN Textures32Bit;
	BOOLEAN Musik;
	BOOLEAN fire;
};

const BOOLEAN LoadConfig();
const BOOLEAN SaveConfig();


extern TConfig Config;


#endif