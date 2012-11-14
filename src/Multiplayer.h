/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _MULTIPLAYER_INCLUDED_
#define _MULTIPLAYER_INCLUDED_





extern DPID LocalPlayerID;


typedef struct NETWORKDATA
{
	DPID from,to;
	DWORD Message;
	DWORD id,p1,p2,p3;

	LPVOID userdata,msgdata;
}NETWORKDATA,*PNETWORKDATA;




class CMessageChain
{
private:
	DWORD ids[2];
	float lastsend;
	const BYTE elements;
	BOOLEAN shuttingdown;
public:
	CMessageChain(const BYTE velements)
		:elements(velements),lastsend(0.0f),shuttingdown(FALSE)
	{
		for (int i=0;i<elements;i++)
			ids[i]=0;
	}

	~CMessageChain() { Clear(); /*DebugOut("CMessageChain::~CMessageChain();");*/}

	const DWORD Send(const DPID to,const DWORD msg,const DWORD id,const DWORD p1,const DWORD p2,const DWORD p3,const LPVOID data,const DWORD datasize,const BOOLEAN guaranteed);
	void Execute(const float elapsed) { lastsend+=elapsed; }

	void SendComplete()
	{
		int i;
		for (i=0;i<elements-1;i++)
			ids[i]=ids[i+1];
		ids[i]=0;
		if ((ids[0]==0)&&(shuttingdown))delete this;
	}
	void Clear();
	void ShutDown() { shuttingdown=TRUE; if (ids[0]==0)delete this; }
};


const int NM_CHAT=1;
const int NM_WORLDMSG=2;
const int NM_COMPLETE=4;
const int NM_REQUESTPLAYER=5;
const int NM_OBJECTDATA=7;
const int NM_REQUESTOBJECTS=8;
const int NM_CREATEOBJECT=9;
const int NM_NOTIFY=10;
const int NM_REMOVE=11;
const int NM_PING=12;
const int NM_VARCHANGED=13;
const int NM_NAMECHANGED=14;
const int NM_TYPECHANGE=15;


const int CHATCOLOR=D3DRGB(1,0,0.5f);


PNETWORKDATA PollNetwork();
void DispatchNetworkMessage(const PNETWORKDATA data);
void DoneMultiplayer();
const DWORD Send(const DPID to,const DWORD msg,const DWORD id,const DWORD p1,const DWORD p2,const DWORD p3,const LPVOID data,const DWORD datasize,const BOOLEAN guaranteed,const LPVOID context=NULL);
const BOOLEAN SynchronizeWorld(const int playertype);
const DWORD DispatchOnly(const DWORD message1,const DWORD message2=0,const DWORD message3=0,const DWORD message4=0);
const int GetPlayerName(const DPID playerid,PCHAR buffer);


class CObject;


#include "objects.h"
const DPID UID2DPID(const UID uid);
CObject* SpawnObject(const UID type,int* anzahl);



#endif