/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "text.h"
#include "resources/resources.h"
#include "resources/resource.h"


// Zu mappende Strings mit Voreinstellungen

PCHAR E_CRITICAL="This game was terminated due to a critical error.\nErrorcode: 0x%.8x, Address: 0x%.8x.";
PCHAR E_TITLE="Error!";
PCHAR E_INIT="(#1)";
PCHAR E_MODELS="(#2)";
PCHAR E_INTRO="(#3)";
PCHAR E_MUSIC="(#4)";
PCHAR E_INITWINDOW="(#5)";
PCHAR E_DDRAW="(#6)";
PCHAR E_SETCOOPERATIVELEVEL="(#7)";
PCHAR E_DISPLAYMODE="(#8)";
PCHAR E_PRIMARYSURFACE="(#9)";
PCHAR E_CLIPPER="(#10)";
PCHAR E_GETBACKBUFFER="(#11)";
PCHAR E_CREATEBACKBUFFER="(#12)";
PCHAR E_NOZBUFFER="(#13)";
PCHAR E_CREATEZBUFFER="(#14)";
PCHAR E_NOUPDATEAFTERZBUFFER="(#15)";
PCHAR E_D3D="(#16)";
PCHAR E_NOD3D7="(#17)";
PCHAR E_CREATEDEVICE="(#18)";
PCHAR E_VIEWPORT="(#19)";
PCHAR E_DIRECTSOUND="(#20)";
PCHAR E_DSSETCOOPERATIVELEVEL="(#21)";
PCHAR E_PRIMARYBUFFER="(#22)";
PCHAR E_DIRECTINPUT="(#23)";
PCHAR E_LOADSOUNDS="(#24)";
PCHAR E_LOADTEXTURES="(#25)";
PCHAR E_LOADSPRITES="(#26)";
PCHAR E_RESTORETEXTURES="(#27)";
PCHAR E_KICKED="Du wurdest gekickt!";
PCHAR E_KICKEDYOURSELF=E_KICKED;


PCHAR T_CLASSNAME="MyMainWindow";
PCHAR T_TITLESERVER="Chicken Tournament - Server";
PCHAR T_TITLECLIENT="Chicken Tournament - Client";

PCHAR T_WIREFRAME1="Wireframe 2";
PCHAR T_WIREFRAME2="Wireframe 0";
PCHAR T_WIREFRAME3="Wireframe 1";

PCHAR T_LOADING="Loading...";
PCHAR T_LOADED[9]=
{
	"3D Engine",
	"Sounds",
	"Input Devices",
	"Timer",
	"Texturen",
	"Models",
	"Objects",
	"Music",
	"Synchronisating Network"
};

PCHAR T_SOUNDON="Sound an";
PCHAR T_SOUNDOFF="Sound aus";
PCHAR T_SCREENSHOTSAVED="Screenshot gespeichert.";
PCHAR T_MOUSEREVERSED="Maus umkehren an.";
PCHAR T_MOUSENORMAL="Maus umkehren aus.";

PCHAR T_TOT="Du bist tot!";
PCHAR T_GOLFEND="%.1f Meter";
PCHAR T_CHICKENHASHIT="Du hast %s auf dem Gewissen.";
PCHAR T_FARMERWASHIT="Du wurdest von %s getötet.";
PCHAR T_FARMERHASHIT="Du hast %s auf dem Gewissen.";
PCHAR T_CHICKENWASHIT="Du wurdest von %s getötet.";

PCHAR T_HATS[8]=
{	"Keinen Hut","Zylinder","Cowboyhut","Krone","Zipfelmütze","Bratpfanne","Kochmütze","Sonnenbrille" };
PCHAR T_HAT="%s aufgesetzt.";

PCHAR T_GOTGARTENKRALLE="Gartenkralle Gold 500";
PCHAR T_GOTGOLFER="Golfschläger";
PCHAR T_GOTDRESCHER="Mähdrescher";
PCHAR T_GOTPLASMA="Plasma Kanone";

PCHAR T_GAMENAME="Spiel";
PCHAR T_INVALIDSIZE="Ungültige Eingaben.";
PCHAR T_INVALIDSIZETITLE="Wahnsinniger!!!";
PCHAR T_ERRORENUMSESSIONS="Fehler (#%d).";
PCHAR T_NOSESSIONS="Nix gefunden.";
PCHAR T_REFRESH="Bitte auf Aktualisieren drücken!";
PCHAR E_CREATEGAME="Kann Spiel nicht erstellen!";
PCHAR E_CREATEPLAYER="Kann Spieler nicht erstellen";
PCHAR E_JOINGAME="Kann Spiel nicht beitreten!";
PCHAR E_SESSIONLOST="Netzwerkverbindung verloren.";

PCHAR T_JOINED="%s ist dem Spiel beigetreten.";
PCHAR T_LEFT="%s hat das Spiel verlassen.";
PCHAR T_NEWSERVER="Du bist der neue Leiter des Spiels.";
PCHAR T_SERVERCHANGEDWORLD="Der Server hat die Welt verändert.";
PCHAR T_NAMECHANGED="%s heisst jetzt %s.";

PCHAR T_MUSICOFF="Musik aus";
PCHAR T_MUSICON="Musik an";


PCHAR T_LOSGEHTS="Los gehts!";
PCHAR T_SERVERCAT=" (Server)";
PCHAR T_PLAYERTYPES[2]={"Bauer","Huhn"};
PCHAR T_PLAYER="Spieler";
PCHAR T_NAME="Name";
PCHAR T_ID="ID";
PCHAR T_KILLS="Kills";
PCHAR T_PING="Ping";
PCHAR T_TYPE="Typ";

PCHAR M_MAINMENU="Hauptmenü";
PCHAR M_CONTINUE="Fortsetzen";
PCHAR M_OPTIONS="Optionen";
PCHAR M_SERVEROPTIONS="Server";
PCHAR M_INFO="Info";
PCHAR M_EXIT="Beenden";
PCHAR M_BACK="Zurück";

PCHAR M_ALPHABLENDING="Alphablending";
PCHAR M_FOG="Nebel";
PCHAR M_SPECULAR="Glanzlichter";
PCHAR M_VSYNC="Vertikale Synchronisation";
PCHAR M_REFLECTIONS="Reflektionen";
PCHAR M_BLOOD="Blut";
PCHAR M_FEDERN="Federn";
PCHAR M_GIBS="Leichenteile";
PCHAR M_FIRE="Feuer";
PCHAR M_BLOODAMOUNT="Menge Blut:";
PCHAR M_SOUND="Sound";
PCHAR M_UNREALSOUND="Unreal Feedback";
PCHAR M_GACKERN="Hühnergackern";
PCHAR M_MUSIC="Musik";
PCHAR M_REVERSEMOUSE="Maus umkehren";
PCHAR M_REVERSEFLIGHT="Maus im Flug umkehren";
PCHAR M_SMOOTHMOUSE="Mausbewegung glätten";
PCHAR M_MOUSESENSITIVITY="Maus Empfindlichkeit:";
PCHAR M_GOLFERGEBNISSE="Golfergebnisse anzeigen";
PCHAR M_GRAFIK="Grafik";
PCHAR M_SONSTIGES="Sonstiges";

PCHAR M_HAUPTIDEE="Hauptidee:";
PCHAR M_VORSCHLAEGE="Konstruktive Vorschläge:";
PCHAR M_PROGRAMMIERER="Programmierer:";
PCHAR M_GRAFIKER="Grafiker:";
PCHAR M_TEXTUREN="Texturen:";
PCHAR M_SPRITES="Sprites:";
PCHAR M_MODELLE="Modelle:";
PCHAR M_SOUNDS="Sounds:";
PCHAR M_MUSIK="Musik:";
PCHAR M_NETWORK="Netzwerk:";
PCHAR M_AI="Künstliche Dummheit:";
PCHAR M_BETATESTER="Beta Tester:";
PCHAR M_THANKS="Dank an:";
PCHAR M_ENDE="- = ENDE = -";
PCHAR M_EMAILS="Emails an: Spam84@gmx.de";
PCHAR M_INFOABOUT="Über";

PCHAR M_ANZAHL="Anzahl: %d";
PCHAR M_CHICKEN="Hühner";
PCHAR M_STAELLE="Ställe";
PCHAR M_TREES="Bäume";
PCHAR M_GARTENKRALLEN="Gartenkrallen";
PCHAR M_GOLFSCHLAEGER="Golfschläger";
PCHAR M_DRESCHER="Mähdrescher";
PCHAR M_PLASMACANNONS="Plasma Kanonen";
PCHAR M_EINSTELLUNGEN="Einstellungen";
PCHAR M_AIRCONTROL="AirControl [m/(s*s)]";
PCHAR M_EGGDAMAGE="Eier Schaden [1/100]";
PCHAR M_GRAVITY="Gravitation [m/(s*s)]";
PCHAR M_JUMPMULTIPLIER="Sprungmultiplikator [ ]";
PCHAR M_PLASMARATE="Plasmarate [s]";
PCHAR M_PLASMAREGENERATION="Plasmaregeneration [1/s]";
PCHAR M_REGENERATEPOWER="Flugkraftregeneration [1/s]";
PCHAR M_SITSPEED="Brütgeschwindigkeit [1/s]";
PCHAR M_TAKEPOWER="Flugkraftverbrauch [1/s]";
PCHAR M_THROWTIME="Wurfzeit der Eier [s]";

PCHAR M_KI="Künstliche Intelligenz";
PCHAR M_PRESSURE="Druckwelle der Plasmakanone";
PCHAR M_CROSSHAIR="Fadenkreuz";
PCHAR M_RESPAWNTIME="Respawn Zeit";

PCHAR C_INTRO1="<<<<< Konsole >>>>>";
PCHAR C_INTRO2="HELP eingeben, für alle Befehle. Escape zum Verlassen.";
PCHAR C_SERVERONLY="Das darf nur der Server!";
PCHAR C_NOFLOAT="Keine gültige Fließkommazahl.";
PCHAR C_VARCHANGED="%s geändert auf %.2f";
PCHAR C_FOGOFF="Nebel deaktiviert.";
PCHAR C_FOGON="Nebel aktiviert.";
PCHAR C_ALLCOMMANDS="Alle Befehle:";
PCHAR C_SPAWNPARAMETER="Erwarte einen Parameter:";
PCHAR C_SPAWNPARAMETERS[9]=
{"Huhn","Gartenkralle","Golfschläger","Mähdrescher","Baum","Hühnerstall","Plasma Cannon","Item","Bauer"};
PCHAR C_SPAWNED="%s erstellt. Sind jetzt %d.";
PCHAR C_INVALIDPARAMETER="Kein gültiger Parameter angegeben.";
PCHAR C_INVALIDUID="Keine gültige UID angegeben.";
PCHAR C_UIDNOTFOUND="Object mit der angegebenen UID wurde nicht gefunden.";
PCHAR C_OBJECTREMOVED="Object %d (%s) wurde entfernt.";
PCHAR C_SPECULAROFF="Glanzlichter deaktiviert.";
PCHAR C_SPECULARON="Glanzlichter aktiviert.";
PCHAR C_NEWWORLD="Du hast die Welt verändert.";
PCHAR C_MESSAGESENT="Nachricht gesendet.";
PCHAR C_INSANEGORELEVEL="Bist du bekloppt??? Auf eigene Verantwortung!!!";
PCHAR C_ALPHABLENDINGON="Alpha-Blending aktiviert.";
PCHAR C_ALPHABLENDINGOFF="Alpha-Blending deaktiviert.";
PCHAR C_YOURNAME="Dein Name ist %s.";
PCHAR C_NAMECHANGED="Name geändert.";
PCHAR C_NEWRACE="Bitte neuen Typ angeben:";
PCHAR C_PLAYERTYPECHANGED="Spielertyp geändert.";
PCHAR C_INVALIDCOMMAND="< Eingabe nicht kapiert. Alle Befehle gibts mit HELP. >";
PCHAR C_OBJECTINFOS="Objektinfos:";
PCHAR C_OBJECTINFO="Objekt %d: UID=%d (%s)";

PCHAR M_RESOLUTION="Auflösung";
PCHAR M_APPLY="Übernehmen";
PCHAR M_FULLSCREEN="Vollbild";
PCHAR M_ANIMATIONS="Animationen";
PCHAR M_GRASS="Gras";
PCHAR M_SHOWPLAYERNAMES="Spielernamen zeigen";
PCHAR M_DIRESTRAITS="";
PCHAR M_FARMER="Bauer";




inline const PCHAR MapString(const DWORD ident)
{
	CHAR temp[500];
	int max=LoadString(GetDllInstance(),ident,&temp[0],499);

	if (max==0)return NULL;
	PCHAR r=(PCHAR)malloc(max+2);
	strcpy(r,&temp[0]);
	return r;
}

const BOOLEAN MapStrings()
{
#ifdef _DEBUG
	DWORD time=GetTickCount();
#endif

#define string(x) DWORD(&x),IDS_##x

	const DWORD stringlist[]={
		string(E_CRITICAL),
		string(E_TITLE),
		string(E_INIT),
		string(E_MODELS),
		string(E_INTRO),
		string(E_MUSIC),
		string(E_INITWINDOW),
		string(E_DDRAW),
		string(E_SETCOOPERATIVELEVEL),
		string(E_DISPLAYMODE),
		string(E_PRIMARYSURFACE),
		string(E_CLIPPER),
		string(E_GETBACKBUFFER),
		string(E_CREATEBACKBUFFER),
		string(E_NOZBUFFER),
		string(E_CREATEZBUFFER),
		string(E_NOUPDATEAFTERZBUFFER),
		string(E_D3D),
		string(E_NOD3D7),
		string(E_CREATEDEVICE),
		string(E_VIEWPORT),
		string(E_DIRECTSOUND),
		string(E_DSSETCOOPERATIVELEVEL),
		string(E_PRIMARYBUFFER),
		string(E_DIRECTINPUT),
		string(E_LOADSOUNDS),
		string(E_LOADTEXTURES),
		string(E_LOADSPRITES),
		string(E_RESTORETEXTURES),
		string(E_KICKED),
		string(E_KICKEDYOURSELF),

		string(T_CLASSNAME),
		string(T_TITLESERVER),
		string(T_TITLECLIENT),
		string(T_WIREFRAME1),
		string(T_WIREFRAME2),
		string(T_WIREFRAME3),
		string(T_LOADING),
		DWORD(&T_LOADED[0]),IDS_T_LOADED0,
		DWORD(&T_LOADED[1]),IDS_T_LOADED1,
		DWORD(&T_LOADED[2]),IDS_T_LOADED2,
		DWORD(&T_LOADED[3]),IDS_T_LOADED3,
		DWORD(&T_LOADED[4]),IDS_T_LOADED4,
		DWORD(&T_LOADED[5]),IDS_T_LOADED5,
		DWORD(&T_LOADED[6]),IDS_T_LOADED6,
		DWORD(&T_LOADED[7]),IDS_T_LOADED7,
		DWORD(&T_LOADED[8]),IDS_T_LOADED8,

		string(T_SOUNDON),
		string(T_SOUNDOFF),
		string(T_MUSICOFF),
		string(T_MUSICON),
		string(T_SCREENSHOTSAVED),
		string(T_MOUSEREVERSED),
		string(T_MOUSENORMAL),
		string(T_TOT),
		string(T_GOLFEND),
		string(T_CHICKENHASHIT),
		string(T_FARMERWASHIT),
		string(T_FARMERHASHIT),
		string(T_CHICKENWASHIT),

		DWORD(&T_HATS[0]),IDS_T_HATS0,
		DWORD(&T_HATS[1]),IDS_T_HATS1,
		DWORD(&T_HATS[2]),IDS_T_HATS2,
		DWORD(&T_HATS[3]),IDS_T_HATS3,
		DWORD(&T_HATS[4]),IDS_T_HATS4,
		DWORD(&T_HATS[5]),IDS_T_HATS5,
		DWORD(&T_HATS[6]),IDS_T_HATS6,
		DWORD(&T_HATS[7]),IDS_T_HATS7,
		string(T_HAT),
		string(T_GOTGARTENKRALLE),
		string(T_GOTGOLFER),
		string(T_GOTDRESCHER),
		string(T_GOTPLASMA),

		string(T_GAMENAME),
		string(T_INVALIDSIZE),
		string(T_INVALIDSIZETITLE),
		string(T_ERRORENUMSESSIONS),
		string(T_NOSESSIONS),
		string(T_REFRESH),
		string(E_CREATEGAME),
		string(E_CREATEPLAYER),
		string(E_JOINGAME),
		string(E_SESSIONLOST),

		string(T_JOINED),
		string(T_LEFT),
		string(T_NEWSERVER),
		string(T_SERVERCHANGEDWORLD),
		string(T_NAMECHANGED),

		string(T_LOSGEHTS),
		string(T_SERVERCAT),
		DWORD(&T_PLAYERTYPES[0]),IDS_T_PLAYERTYPES0,
		DWORD(&T_PLAYERTYPES[1]),IDS_T_PLAYERTYPES1,
		string(T_PLAYER),
		string(T_NAME),
		string(T_ID),
		string(T_KILLS),
		string(T_PING),
		string(T_TYPE),

		string(M_MAINMENU),
		string(M_CONTINUE),
		string(M_OPTIONS),
		string(M_SERVEROPTIONS),
		string(M_INFO),
		string(M_EXIT),
		string(M_BACK),

		string(M_ALPHABLENDING),
		string(M_FOG),
		string(M_SPECULAR),
		string(M_VSYNC),
		string(M_REFLECTIONS),
		string(M_BLOOD),
		string(M_FEDERN),
		string(M_GIBS),
		string(M_BLOODAMOUNT),
		string(M_SOUND),
		string(M_UNREALSOUND),
		string(M_GACKERN),
		string(M_MUSIC),
		string(M_REVERSEMOUSE),
		string(M_REVERSEFLIGHT),
		string(M_SMOOTHMOUSE),
		string(M_MOUSESENSITIVITY),
		string(M_GOLFERGEBNISSE),
		string(M_GRAFIK),
		string(M_SONSTIGES),

		string(M_HAUPTIDEE),
		string(M_VORSCHLAEGE),
		string(M_PROGRAMMIERER),
		string(M_GRAFIKER),
		string(M_TEXTUREN),
		string(M_SPRITES),
		string(M_MODELLE),
		string(M_SOUNDS),
		string(M_MUSIK),
		string(M_NETWORK),
		string(M_BETATESTER),
		string(M_THANKS),
		string(M_ENDE),
		string(M_EMAILS),
		string(M_INFOABOUT),

		string(M_ANZAHL),
		string(M_CHICKEN),
		string(M_STAELLE),
		string(M_TREES),
		string(M_GARTENKRALLEN),
		string(M_GOLFSCHLAEGER),
		string(M_DRESCHER),
		string(M_PLASMACANNONS),
		string(M_EINSTELLUNGEN),
		string(M_AIRCONTROL),
		string(M_AIRCONTROL),
		string(M_EGGDAMAGE),
		string(M_GRAVITY),
		string(M_JUMPMULTIPLIER),
		string(M_PLASMARATE),
		string(M_PLASMAREGENERATION),
		string(M_REGENERATEPOWER),
		string(M_SITSPEED),
		string(M_TAKEPOWER),
		string(M_THROWTIME),
		string(M_KI),
		string(M_PRESSURE),
		string(M_RESPAWNTIME),
		string(M_CROSSHAIR),
		string(M_FIRE),

		
		string(C_INTRO1),
		string(C_INTRO2),
		string(C_SERVERONLY),
		string(C_NOFLOAT),
		string(C_VARCHANGED),
		string(C_FOGON),
		string(C_FOGOFF),
		string(C_ALLCOMMANDS),
		string(C_SPAWNPARAMETER),
		DWORD(&C_SPAWNPARAMETERS[0]),IDS_C_SPAWNPARAMETERS0,
		DWORD(&C_SPAWNPARAMETERS[1]),IDS_C_SPAWNPARAMETERS1,
		DWORD(&C_SPAWNPARAMETERS[2]),IDS_C_SPAWNPARAMETERS2,
		DWORD(&C_SPAWNPARAMETERS[3]),IDS_C_SPAWNPARAMETERS3,
		DWORD(&C_SPAWNPARAMETERS[4]),IDS_C_SPAWNPARAMETERS4,
		DWORD(&C_SPAWNPARAMETERS[5]),IDS_C_SPAWNPARAMETERS5,
		DWORD(&C_SPAWNPARAMETERS[6]),IDS_C_SPAWNPARAMETERS6,
		DWORD(&C_SPAWNPARAMETERS[7]),IDS_C_SPAWNPARAMETERS7,
		string(C_SPAWNED),
		string(C_INVALIDPARAMETER),
		string(C_INVALIDUID),
		string(C_UIDNOTFOUND),
		string(C_OBJECTREMOVED),
		string(C_SPECULARON),
		string(C_SPECULAROFF),
		string(C_NEWWORLD),
		string(C_MESSAGESENT),
		string(C_INSANEGORELEVEL),
		string(C_ALPHABLENDINGON),
		string(C_ALPHABLENDINGOFF),
		string(C_YOURNAME),
		string(C_NAMECHANGED),
		string(C_NEWRACE),
		string(C_PLAYERTYPECHANGED),
		string(C_INVALIDCOMMAND),
		string(C_OBJECTINFOS),
		string(C_OBJECTINFO),

		string(M_RESOLUTION),
		string(M_APPLY),
		string(M_FULLSCREEN),
		string(M_ANIMATIONS),
		string(M_GRASS),
		string(M_SHOWPLAYERNAMES),
		string(M_AI),
		string(M_DIRESTRAITS),
		string(M_FARMER)
	};


	for (int i=0;i<sizeof(stringlist)/sizeof(stringlist[0]);i+=2)
	{
		PCHAR *p=(PCHAR*)(stringlist[i]);
		const PCHAR z=MapString(stringlist[i+1]);
		if (z==NULL)
		{
			DebugOut(*p);
			return FALSE; 
		}
		*p=z;
	}


#ifdef _DEBUG
	time=GetTickCount()-time;
	CHAR c[100];
	sprintf(&c[0],"Strings gemappt in %d ms.",time);
	DebugOut(&c[0]);
#endif

	return TRUE;
}
