/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _TEXT_INCLUDED_
#define _TEXT_INCLUDED_



// Fehlermeldungen
extern PCHAR E_CRITICAL;
extern PCHAR E_TITLE;
extern PCHAR E_INIT;
extern PCHAR E_MODELS;
extern PCHAR E_INTRO;
extern PCHAR E_MUSIC;
extern PCHAR E_INITWINDOW;
extern PCHAR E_DDRAW;
extern PCHAR E_SETCOOPERATIVELEVEL;
extern PCHAR E_DISPLAYMODE;
extern PCHAR E_PRIMARYSURFACE;
extern PCHAR E_CLIPPER;
extern PCHAR E_GETBACKBUFFER;
extern PCHAR E_CREATEBACKBUFFER;
extern PCHAR E_NOZBUFFER;
extern PCHAR E_CREATEZBUFFER;
extern PCHAR E_NOUPDATEAFTERZBUFFER;
extern PCHAR E_D3D;
extern PCHAR E_NOD3D7;
extern PCHAR E_CREATEDEVICE;
extern PCHAR E_VIEWPORT;
extern PCHAR E_DIRECTSOUND;
extern PCHAR E_DSSETCOOPERATIVELEVEL;
extern PCHAR E_PRIMARYBUFFER;
extern PCHAR E_DIRECTINPUT;
extern PCHAR E_LOADSOUNDS;
extern PCHAR E_LOADTEXTURES;
extern PCHAR E_LOADSPRITES;
extern PCHAR E_RESTORETEXTURES;
extern PCHAR E_KICKED;
extern PCHAR E_KICKEDYOURSELF;

extern PCHAR T_CLASSNAME;
extern PCHAR T_TITLESERVER;
extern PCHAR T_TITLECLIENT;


extern PCHAR T_WIREFRAME1;
extern PCHAR T_WIREFRAME2;
extern PCHAR T_WIREFRAME3;

extern PCHAR T_LOADING;
extern PCHAR T_LOADED[9];
enum LOADED_NR
{
	LOADED_3DENGINE=1,
	LOADED_SOUNDS,
	LOADED_INPUT,
	LOADED_TIMER,
	LOADED_TEXTURES,
	LOADED_MODELS,
	LOADED_OBJECTS,
	LOADED_MUSIC,
	LOADED_NETWORK
};


extern PCHAR T_SOUNDON;
extern PCHAR T_SOUNDOFF;
extern PCHAR T_SCREENSHOTSAVED;
extern PCHAR T_MOUSEREVERSED;
extern PCHAR T_MOUSENORMAL;
extern PCHAR T_TOT;
extern PCHAR T_GOLFEND;
extern PCHAR T_CHICKENHASHIT;
extern PCHAR T_CHICKENWASHIT;
extern PCHAR T_FARMERWASHIT;
extern PCHAR T_FARMERHASHIT;

extern PCHAR T_HATS[8];
extern PCHAR T_HAT;
extern PCHAR T_GOTGARTENKRALLE;
extern PCHAR T_GOTGOLFER;
extern PCHAR T_GOTDRESCHER;
extern PCHAR T_GOTPLASMA;


extern PCHAR T_GAMENAME;
extern PCHAR T_INVALIDSIZE;
extern PCHAR T_INVALIDSIZETITLE;
extern PCHAR T_ERRORENUMSESSIONS;
extern PCHAR T_NOSESSIONS;
extern PCHAR T_REFRESH;
extern PCHAR E_CREATEGAME;
extern PCHAR E_CREATEPLAYER;
extern PCHAR E_JOINGAME;
extern PCHAR E_SESSIONLOST;


extern PCHAR T_JOINED;
extern PCHAR T_LEFT;
extern PCHAR T_NEWSERVER;
extern PCHAR T_SERVERCHANGEDWORLD;
extern PCHAR T_NAMECHANGED;

extern PCHAR T_MUSICOFF;
extern PCHAR T_MUSICON;


extern PCHAR T_LOSGEHTS;
extern PCHAR T_SERVERCAT;
extern PCHAR T_PLAYERTYPES[2];
extern PCHAR T_PLAYER;
extern PCHAR T_NAME;
extern PCHAR T_ID;
extern PCHAR T_KILLS;
extern PCHAR T_PING;
extern PCHAR T_TYPE;
 
extern PCHAR M_MAINMENU;
extern PCHAR M_CONTINUE;
extern PCHAR M_OPTIONS;
extern PCHAR M_SERVEROPTIONS;
extern PCHAR M_INFO;
extern PCHAR M_EXIT;
extern PCHAR M_BACK;
 
extern PCHAR M_ALPHABLENDING;
extern PCHAR M_FOG;
extern PCHAR M_SPECULAR;
extern PCHAR M_VSYNC;
extern PCHAR M_REFLECTIONS;
extern PCHAR M_BLOOD;
extern PCHAR M_FEDERN;
extern PCHAR M_GIBS;
extern PCHAR M_BLOODAMOUNT;
extern PCHAR M_SOUND;
extern PCHAR M_UNREALSOUND;
extern PCHAR M_GACKERN;
extern PCHAR M_MUSIC;
extern PCHAR M_REVERSEMOUSE;
extern PCHAR M_REVERSEFLIGHT;
extern PCHAR M_SMOOTHMOUSE;
extern PCHAR M_MOUSESENSITIVITY;
extern PCHAR M_GOLFERGEBNISSE;
extern PCHAR M_GRAFIK;
extern PCHAR M_SONSTIGES;
extern PCHAR M_FIRE;
 
extern PCHAR M_HAUPTIDEE;
extern PCHAR M_VORSCHLAEGE;
extern PCHAR M_PROGRAMMIERER;
extern PCHAR M_GRAFIKER;
extern PCHAR M_TEXTUREN;
extern PCHAR M_SPRITES;
extern PCHAR M_MODELLE;
extern PCHAR M_SOUNDS;
extern PCHAR M_MUSIK;
extern PCHAR M_NETWORK;
extern PCHAR M_AI;
extern PCHAR M_BETATESTER;
extern PCHAR M_THANKS;
extern PCHAR M_ENDE;
extern PCHAR M_EMAILS;
extern PCHAR M_DIRESTRAITS;

extern PCHAR M_INFOABOUT;

extern PCHAR M_ANZAHL;
extern PCHAR M_CHICKEN;
extern PCHAR M_STAELLE;
extern PCHAR M_TREES;
extern PCHAR M_GARTENKRALLEN;
extern PCHAR M_GOLFSCHLAEGER;
extern PCHAR M_DRESCHER;
extern PCHAR M_PLASMACANNONS;
extern PCHAR M_EINSTELLUNGEN;
extern PCHAR M_AIRCONTROL;
extern PCHAR M_EGGDAMAGE;
extern PCHAR M_GRAVITY;
extern PCHAR M_JUMPMULTIPLIER;
extern PCHAR M_PLASMARATE;
extern PCHAR M_PLASMAREGENERATION;
extern PCHAR M_REGENERATEPOWER;
extern PCHAR M_SITSPEED;
extern PCHAR M_TAKEPOWER;
extern PCHAR M_THROWTIME;


extern PCHAR M_KI;

extern PCHAR C_INTRO1;
extern PCHAR C_INTRO2;
extern PCHAR C_SERVERONLY;
extern PCHAR C_NOFLOAT;
extern PCHAR C_VARCHANGED;
extern PCHAR C_FOGOFF;
extern PCHAR C_FOGON;
extern PCHAR C_ALLCOMMANDS;
extern PCHAR C_SPAWNPARAMETER;
extern PCHAR C_SPAWNPARAMETERS[9];
extern PCHAR C_SPAWNED;
extern PCHAR C_INVALIDPARAMETER;
extern PCHAR C_INVALIDUID;
extern PCHAR C_UIDNOTFOUND;
extern PCHAR C_OBJECTREMOVED;
extern PCHAR C_SPECULAROFF;
extern PCHAR C_SPECULARON;
extern PCHAR C_NEWWORLD;
extern PCHAR C_MESSAGESENT;
extern PCHAR C_INSANEGORELEVEL;
extern PCHAR C_ALPHABLENDINGON;
extern PCHAR C_ALPHABLENDINGOFF;
extern PCHAR C_YOURNAME;
extern PCHAR C_NAMECHANGED;
extern PCHAR C_NEWRACE;
extern PCHAR C_PLAYERTYPECHANGED;
extern PCHAR C_INVALIDCOMMAND;
extern PCHAR C_OBJECTINFOS;
extern PCHAR C_OBJECTINFO;
extern PCHAR M_CROSSHAIR;
extern PCHAR M_PRESSURE;
extern PCHAR M_RESPAWNTIME;

extern PCHAR M_RESOLUTION;
extern PCHAR M_APPLY;
extern PCHAR M_FULLSCREEN;
extern PCHAR M_ANIMATIONS;
extern PCHAR M_GRASS;
extern PCHAR M_SHOWPLAYERNAMES;
extern PCHAR M_FARMER;




// Nicht zu mappende Strings


const PCHAR T_CONFIGFILE="config.dat";
const PCHAR E_LANGUAGE="Language strings could not be set. Continue anyway?";
const PCHAR T_SCREENSHOTFILE="CT_%s_%s";
const PCHAR UnrealText[]=
{
	"Doublekill",
	"Multikill",
	"Megakill",
	"Ultrakill",
	"Monsterkill",
	"Ludicrouskill",
	"Holy Shit"
};
const PCHAR UnrealText2[]=
{
	"Killingspree",
	"Rampage",
	"Dominating",
	"Unstoppable",
	"Godlike",
	"Wicked Sick"
};

#ifdef _DEBUG
	const PCHAR IntroFile="intro\\intro_hi.avi";
#else
	const PCHAR IntroFile="intro.avi";
#endif






const BOOLEAN MapStrings();


#endif