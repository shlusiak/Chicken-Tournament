/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "multiplayer.h"
#include "game.h"
#include "objects.h"
#include "player.h"
#include "bauer.h"
#include "chicken.h"
#include "gartenkralle.h"
#include "golfer.h"
#include "drescher.h"
#include "tree.h"
#include "chickenplayer.h"
#include "text.h"
#include "plasmacannon.h"
#include "config.h"
#include "computerbauer.h"
#include "ei.h"




DPID LocalPlayerID;


static NETWORKDATA data;
static LPVOID buf=NULL;
static DWORD bufsize=0;
static BOOLEAN otherplayers=FALSE;


const DWORD SysMsgColor=D3DRGB(1,1,1);


PNETWORKDATA PollNetwork()
{
	if (game->lpDirectPlay==NULL)return NULL;
	DPID f,t;

	DWORD size;
	const HRESULT hr=game->lpDirectPlay->Receive(&f,&t,DPRECEIVE_ALL,NULL,&size);
	if ((hr!=DPERR_BUFFERTOOSMALL)&&(hr!=DP_OK))return NULL;

	if (size>bufsize)
	{
		if (buf)free(buf);
		buf=malloc(bufsize=size);
	}

	game->lpDirectPlay->Receive(&f,&t,DPRECEIVE_ALL,buf,&size);

	data.from=f;
	data.to=t;
	data.Message=*PDWORD(buf);
	data.p1=data.p2=data.p3=data.id=0;
	data.userdata=NULL;
	data.msgdata=buf;

	if (size>=sizeof(DWORD)*5)
	{
		data.id=*(PDWORD(buf)+1);
		data.p1=*(PDWORD(buf)+2);
		data.p2=*(PDWORD(buf)+3);
		data.p3=*(PDWORD(buf)+4);
	}
	if (size>sizeof(DWORD)*5)
		data.userdata=(PDWORD(buf)+5);

	game->pi->AddReceivedNum();

	return &data;
}

void DispatchNetworkMessage(const PNETWORKDATA data)
{
	if (data==NULL)return;

	if (data->from==DPID_SYSMSG)
	{
		switch(data->Message)
		{
		case DPSYS_CREATEPLAYERORGROUP:
			{
				CHAR c[100];
				sprintf(&c[0],T_JOINED,((DPMSG_CREATEPLAYERORGROUP*)(data->msgdata))->dpnName.lpszLongNameA);
				DebugOut(&c[0]);
				game->ticker->Add(&c[0],SysMsgColor);
				otherplayers=TRUE;
			}
			break;
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				CHAR c[100];
				sprintf(&c[0],T_LEFT,((DPMSG_DESTROYPLAYERORGROUP*)(data->msgdata))->dpnName.lpszLongNameA);
				DebugOut(&c[0]);
				game->ticker->Add(&c[0],SysMsgColor);

				// Spieler Model entfernen, wenn noch nicht geschehen.
				UID* sid=(UID*)((DPMSG_DESTROYPLAYERORGROUP*)(data->msgdata))->lpRemoteData;
				if (sid)
				{
					CObject* o=game->chain->Find(*sid);
					if (o)
					{
						o->Unbind();
						o->Remove();
					}
				}
			}
			break;
		case DPSYS_HOST:
			{
				LPVOID n;
				DWORD s=0;
				game->lpDirectPlay->GetPlayerName(LocalPlayerID,NULL,&s);
				n=malloc(s);
				game->lpDirectPlay->GetPlayerName(LocalPlayerID,n,&s);
				game->lpDirectPlay->DestroyPlayer(LocalPlayerID);

				if (FAILED(game->lpDirectPlay->CreatePlayer(&LocalPlayerID,(DPNAME*)n,0,0,0,DPPLAYER_SERVERPLAYER )))
				{
					DestroyWindow(game->wnd);
				}
				game->lpDirectPlay->SetPlayerData(LocalPlayerID,&player->id,sizeof(UID),DPSET_REMOTE);
				game->pi->RefreshAll();

				free(n);

				SetWindowText(game->wnd,T_TITLESERVER);
				game->IsServer=TRUE;
				game->ticker->Add(T_NEWSERVER,SysMsgColor);

				// Besitz von Computergesteuerten Spielern übernehmen, um KI zu garantieren
				CObject* o=game->chain->GetFirst();
				while (o)
				{
					if ((o->IsType(IDChicken))||(o->IsType(IDBauer)))
					{
						CPlayer* p=(CPlayer*)o;
						if (!p->isplayer)
							p->IsLocal=TRUE;
					}

					o=o->next;
				}
			}
			break;
		case DPSYS_SENDCOMPLETE:
			{
				DPMSG_SENDCOMPLETE *msg=(DPMSG_SENDCOMPLETE*)(data->msgdata);
				if (msg->lpvContext==NULL)break;

				if ((msg->hr==DP_OK)||
					(msg->hr==DPERR_ABORTED)||
					(msg->hr==DPERR_GENERIC)||
					(msg->hr==DPERR_TIMEOUT))
				{
					CMessageChain* c=(CMessageChain*)msg->lpvContext;
					c->SendComplete();
				}
			}
			break;
		}
	}else{
		switch(data->Message)
		{
		case NM_CHAT:
			{
				CHAR c[600];
				LPVOID v;
				DWORD s=0;
				game->lpDirectPlay->GetPlayerName(data->from,NULL,&s);
				v=malloc(s);
				game->lpDirectPlay->GetPlayerName(data->from,v,&s);

				sprintf(&c[0],"%s: %s",((DPNAME*)v)->lpszLongNameA,(PCHAR)data->userdata);
				free(v);

				DebugOut(&c[0]);
				game->ticker->Add(&c[0],CHATCOLOR);
			}
			break;
		case NM_WORLDMSG:
			DebugOut("Empfange Weltdaten.");
			world->ReceiveData(data);
			break;
		case NM_REQUESTPLAYER:
			{
				if (data->p1==1)
				{
					DebugOut("Player verlangt (Bauer).");
				}else DebugOut("Player verlangt (Huhn).");
				CPlayer* p;
				if (data->p1==1)p=(new CBauer);else p=(new CChickenPlayer);
				game->chain->Add(p);
				p->IsLocal=FALSE;

				Send(DPID_ALLPLAYERS,NM_CREATEOBJECT,p->id,3,0,0,NULL,0,TRUE);
				p->SendData(DPID_ALLPLAYERS);
				Send(data->from,NM_COMPLETE,0,p->id,0,0,NULL,0,TRUE);
			}
			break;
		case NM_OBJECTDATA:
			{
				CObject* p=world->chain->Find(data->id);
				if (p)p->ReceiveData(data);else
				{
#ifdef _DEBUG
					CHAR c[200];
					sprintf(&c[0],"NM_OBJECTDATA für ungültiges Object %d (%s) empfangen!!!!!",data->id,GetTypeName(data->id));

					if (game->ticker)game->ticker->Add(&c[0],D3DRGB(1,0,0));
					DebugOut(&c[0]);
#endif
				}
			}
			break;
		case NM_CREATEOBJECT:
			{
				CObject* o=NULL;
				switch (GetType(data->id))
				{
				case IDBauer:
					if (data->p1==3)
					{
						o=new CBauer;
						((CPlayer*)o)->IsLocal=FALSE;
					}else{
						o=new CComputerBauer;
						((CPlayer*)o)->IsLocal=FALSE;
					}
					break;
				case IDChicken:
					if (data->p1==3)
					{
						o=new CChickenPlayer;
					}else 
					{
						o=new CChicken;
					}
					((CPlayer*)o)->IsLocal=FALSE;	
					break;
				case IDGartenkralle:
					o=new CGartenkralle;
					break;
				case IDGolfer:
					o=new CGolfer;
					break;
				case IDDrescher:
					o=new CDrescher;
					break;
				case IDPlasmaCannon:
					o=new CPlasmaCannon;
					break;
				case IDTree:
					o=new CTree();
					break;
				case IDStall:
					o=new CStall();
					break;
				case IDItem:
					o=new CItem();
					break;
				case IDEi:
					o=new CEi();
					break;
				}
				
				if (o!=NULL)
				{
					game->chain->Add(o);
					o->id=data->id;

					game->pi->RefreshAll();
				}else{
					CHAR c[200];
					sprintf(&c[0],"NM_CREATEOBJECT: Kein Objekt %d (%s) erstellt!!!",data->id,GetTypeName(data->id));
					if (game->ticker)game->ticker->Add(&c[0],D3DRGB(1,0,0));
					DebugOut(&c[0]);
				}
			}
			break;
		case NM_REQUESTOBJECTS:
			{
				CObject* akt=game->chain->GetFirst();
				while (akt)
				{
					if (akt->id!=data->id)
					{
						DWORD p1=0;
						if ((GetType(akt->id)==IDChicken)||(GetType(akt->id)==IDBauer))
							if (((CPlayer*)akt)->isplayer)p1=3;
						
						if ((akt!=world)&&							
							(akt!=game->console))							
						Send(data->from,NM_CREATEOBJECT,akt->id,p1,0,0,NULL,0,TRUE);
					}
					akt=akt->next;
				}
				akt=game->chain->GetFirst();
				while (akt)
				{
					if (akt->id!=data->id)
						akt->SendData(data->from);
					akt=akt->next;
				}

				game->SendVariables();
				Send(data->from,NM_COMPLETE,0,0,0,0,NULL,0,TRUE);
			}
			break;
		case NM_NOTIFY:
			{
				CObject* o=game->chain->Find(data->id);
				if (o)o->Notify(data);else
				{
#ifdef _DEBUG
					CHAR c[200];
					sprintf(&c[0],"NM_NOTIFY: Object %d (%s) not found!",data->id,GetTypeName(data->id));
					DebugOut(&c[0]);
					if (game->ticker)game->ticker->Add(&c[0],D3DRGB(1,0,0));
#endif
				}
			}
 			break;
		case NM_REMOVE:
			{
				CObject* o=game->chain->Find(data->id);
				if (o)
				{
					if (o==player)
					{
						game->errormessage=E_KICKED;
						PostQuitMessage(1);
					}
					else {
						o->Unbind();
						o->Remove();
						o->chain->CleanChain();
					}
				}else
				{
#ifdef _DEBUG
					CHAR c[200];
					sprintf(&c[0],"NM_REMOVE: Object %d (%s) not found!",data->id,GetTypeName(data->id));
					DebugOut(&c[0]);
					if (game->ticker)game->ticker->Add(&c[0],D3DRGB(1,0,0));
#endif
				}
			}
			break;
		case NM_PING:
			{
				if (data->p2==0)Send(data->from,NM_PING,data->id,data->p1,1,0,NULL,0,TRUE);
				else game->pi->ReceivePing(data);
			}
			break;
		case NM_VARCHANGED:
			{
				float f=0.0f;
				if (data->userdata!=NULL)f=*((float*)data->userdata);
				switch(data->p1)
				{
				case 1:game->gravity=*((float*)data->userdata);
					break;
				case 2:game->aircontrol=f;
					break;
				case 3:game->jumpspeed=f;
					break;
				case 4:game->sittime=f;
					break;
				case 5:game->throwtime=f;
					break;
				case 6:game->replenishpower=f;
					break;
				case 7:game->takepower=f;
					break;
				case 8:game->damage=f;
					break;
				case 10:
					{
						CObject* akt=game->chain->GetFirst();
						while (akt)
						{
							akt->NewWorld();
							akt=akt->next;
						}
						game->ticker->Add(T_SERVERCHANGEDWORLD,D3DRGB(1,0.5f,0));
					}
					break;
				case 11:game->plasmaregeneration=f;
					break;
				case 12:game->plasmaradius=f;
					break;
				case 13:game->plasmarate=f;
					break;
				case 14:game->respawntime=f;
					break;
				case 15:game->harvesterspeed=f;
					break;
				case 16:game->harvesterrotspeed=f;
					break;

				case 30:Config.KI=BOOLEAN(data->p2);
					break;
				case 31:game->PlasmaBallPressure=BOOLEAN(data->p2);
					break;
				}

				DebugOut("VarChanged");
			}
			break;
		case NM_NAMECHANGED:
			{
				CHAR c[100];
				LPVOID v;
				DWORD s=0;
				game->lpDirectPlay->GetPlayerName(data->from,NULL,&s);
				v=malloc(s);
				game->lpDirectPlay->GetPlayerName(data->from,v,&s);

				PCHAR n=((DPNAME*)v)->lpszLongNameA;

				sprintf(&c[0],T_NAMECHANGED,(PCHAR)data->userdata,n);
				game->ticker->Add(&c[0],D3DRGB(1,1,0));
				DebugOut(&c[0]);

				free(v);
			}
			break;
		case NM_TYPECHANGE:
			{
				player->Unbind();
				player->Remove();

				if (game->IsServer)
				{
					if (data->p1==1)game->chain->Add(player=new CBauer());
					if (data->p1==2)game->chain->Add(player=new CChickenPlayer());

					if (game->lpDirectPlay)if (FAILED(game->lpDirectPlay->SetPlayerData(LocalPlayerID,&player->id,sizeof(UID),DPSET_REMOTE|DPSET_GUARANTEED)))
					{	
						DebugOut("Konnte SpielerID nicht global setzen!");
					}
					Send(DPID_ALLPLAYERS,NM_CREATEOBJECT,player->id,3,0,0,NULL,0,TRUE);
					player->SendData(DPID_ALLPLAYERS);
				}else{
					DebugOut("Requesting new player.");
					Send(DPID_SERVERPLAYER,NM_REQUESTPLAYER,0,data->p1,0,0,NULL,0,TRUE);
					DWORD id=DispatchOnly(NM_CREATEOBJECT,NM_OBJECTDATA);
					player=(CPlayer*)game->chain->Find(id);
					player->IsLocal=TRUE;
					HRESULT hr;
					if (game->lpDirectPlay)if (FAILED(hr=game->lpDirectPlay->SetPlayerData(LocalPlayerID,&player->id,sizeof(UID),DPSET_REMOTE|DPSET_GUARANTEED)))
					{
						DebugOut("Konnte SpielerID nicht global setzen!");
					}
				}
				game->pi->RefreshAll();
			}
			break;
		default:
			{
#ifdef _DEBUG
				CHAR c[100];
				sprintf(&c[0],"Undefined Networkmessage: %d (%d, %d, %d)",data->Message,data->p1,data->p2,data->p3);
				DebugOut(&c[0]);
				if (game->ticker)game->ticker->Add(&c[0],D3DRGB(1,0,0));
#endif
			}
			break;
		}
	}
}

void DoneMultiplayer()
{
	if (buf)free(buf);
}

const DWORD Send(const DPID to,const DWORD msg,const DWORD id,const DWORD p1,const DWORD p2,const DWORD p3,const LPVOID data,const DWORD datasize,const BOOLEAN guaranteed,const LPVOID context)
{
	if (game->lpDirectPlay==NULL)return 0;
	if (!otherplayers)return 0;

	const int timeout=0;
	const int s=sizeof(DWORD)*5+datasize;
	PDWORD v=(PDWORD)malloc(s);
	v[0]=msg;
	v[1]=id;
	v[2]=p1;
	v[3]=p2;
	v[4]=p3;

	if (datasize!=0)
		memcpy(&v[5],data,datasize);
	
	DWORD msgid=0;

#if TRUE
	// Mit ID's senden 
	if ((game->lpDirectPlay->SendEx(
		LocalPlayerID,to,DPSEND_ASYNC|(context?0:DPSEND_NOSENDCOMPLETEMSG)|(guaranteed?DPSEND_GUARANTEED:0),
		v,s,0,(guaranteed?0:timeout),context,&msgid))!=DPERR_PENDING)
	{
		DebugOut("Failed sending network message.");
	}

#else
	// Ohne Schnickschnack senden
	if ((game->lpDirectPlay->Send(
		LocalPlayerID,to,(guaranteed?DPSEND_GUARANTEED:0),
		v,s)!=DP_OK))
	{
		DebugOut("Failed sending network message.");
	}

#endif

	free(v);
	game->pi->AddSendNum();
	return msgid;
}

const int GetPlayerName(const DPID playerid,PCHAR buffer)
{
	if (game->lpDirectPlay==NULL)return 0;
	LPVOID n;
	DWORD s=0;
	game->lpDirectPlay->GetPlayerName(playerid,NULL,&s);
	if (s<=0)return 0;
	n=malloc(s);
	game->lpDirectPlay->GetPlayerName(playerid,n,&s);

	strcpy(buffer,((DPNAME*)(n))->lpszLongNameA);

	free(n);

	return strlen(buffer);
}

struct ENUMPLAYERSTRUCT
{
	DPID dpid;
	UID uid;
	BOOLEAN succeeded;
};

BOOL FAR PASCAL EnumPlayersCallback3(DPID dpId,DWORD dwPlayerType,LPCDPNAME lpName,DWORD dwFlags,LPVOID lpContext)
{
	ENUMPLAYERSTRUCT *s=(ENUMPLAYERSTRUCT*)lpContext;
	DPID id=0;
	DWORD size=sizeof(DPID);
	game->lpDirectPlay->GetPlayerData(dpId,&id,&size,DPGET_REMOTE);

	if (id==s->uid)
	{
		s->dpid=dpId;
		s->succeeded=TRUE;
		return FALSE;
	}

	return TRUE;
}

const DPID UID2DPID(const UID uid)
{
	if (game->lpDirectPlay==NULL)return DPID(-1);
	ENUMPLAYERSTRUCT s;
	s.succeeded=FALSE;
	s.uid=uid;
	s.dpid=0;

	game->lpDirectPlay->EnumPlayers(NULL,EnumPlayersCallback3,&s,DPENUMPLAYERS_ALL);

	return (s.succeeded?s.dpid:DPID(-1));
}

CObject* SpawnObject(const UID type,int* anzahl)
{
	CObject* no=NULL;
	if (anzahl!=NULL)
	{
		*anzahl=1;
		CObject* o=game->chain->GetFirst();
		while (o)
		{
			if (GetType(o->id)==type)(*anzahl)++;
			o=o->next;
		}
	}

	if (type==IDChicken)
	{
		game->chain->Add(no=new CChicken);
	}
	if (type==IDGartenkralle)
	{
		game->chain->Add(no=new CGartenkralle);
	}
	if (type==IDGolfer)
	{
		game->chain->Add(no=new CGolfer);
	}
	if (type==IDDrescher)
	{
		game->chain->Add(no=new CDrescher);
	}
	if (type==IDTree)
	{
		game->chain->Add(no=new CTree(world->RandX(1.0f),world->RandY(1.0f),randf()*g_PI*2));
	}
	if (type==IDStall)
	{
		game->chain->Add(no=new CStall());
	}
	if (type==IDPlasmaCannon)
	{
		game->chain->Add(no=new CPlasmaCannon());
	}
	if (type==IDItem)
	{
		CItemSpawner::Spawn();
		return (CObject*)0x01;
	}
	if (type==IDBauer)
	{
		game->chain->Add(no=new CComputerBauer);
	}


	if (no==NULL)return NULL;

	Send(DPID_ALLPLAYERS,NM_CREATEOBJECT,no->id,0,0,0,NULL,0,TRUE);
	no->SendData(DPID_ALLPLAYERS);


	return no;
}


	
void CMessageChain::Clear()
{
	for (int i=0;i<sizeof(ids)/sizeof(ids[0]);i++)
	if (ids[i]!=0)
	{
		game->lpDirectPlay->CancelMessage(ids[i],0);
		ids[i]=0;
	}
}

const DWORD CMessageChain::Send(const DPID to,const DWORD msg,const DWORD id,const DWORD p1,const DWORD p2,const DWORD p3,const LPVOID data,const DWORD datasize,const BOOLEAN guaranteed)
{
	// bloss nicht zu häufig senden, wenn die Nachricht nicht unbedingt ankommen muss
	if (!guaranteed)
	{
		if (lastsend<0.11)return 0;
		lastsend=0.0f;
	}
	int nr;
	for (nr=0;nr<elements;nr++)
	{
		if (ids[nr]==0) break;
	}

	if (nr==elements)
	{
		nr--;
		if (ids[nr]!=0)game->lpDirectPlay->CancelMessage(ids[nr],0);
		DebugOut("Nachricht abgebrochen.");
	}

	return (ids[nr]=::Send(to,msg,id,p1,p2,p3,data,datasize,guaranteed,this));
}



const DWORD DispatchOnly(const DWORD message1,const DWORD message2,const DWORD message3,const DWORD message4)
{
	PNETWORKDATA d;
	do
	{
		d=PollNetwork();
		if (d==NULL)
		{
			Sleep(2);
			continue;
		}

		if ((d->Message==message1)||(d->Message==message2)||(d->Message==message3)||(d->Message==message4)||
			(d->Message==NM_PING)||(d->Message==NM_VARCHANGED))
		{
			DispatchNetworkMessage(d);
		}
	}while ((d==NULL)||(d->Message!=NM_COMPLETE));

	return d->p1;
}

const BOOLEAN SynchronizeWorld(const int playertype)
{
	otherplayers=TRUE;

	DPID playerid;
	DPNAME name;
	name.dwFlags=0;
	name.dwSize=sizeof(name);
	name.lpszLongNameA=name.lpszShortNameA=Config.Playername;
	
	game->errormessage=E_CREATEPLAYER;
	if (FAILED(game->lpDirectPlay->CreatePlayer(&playerid,&name,0,0,0,0)))
	{
//		game->lpDirectPlay->Close();
		return FALSE;
	}else{
		DebugOut("Multiplayer initialized successfully! Starting Game.");
		LocalPlayerID=playerid;
	}
	game->DrawLoading(LOADED_NETWORK-1,10);

	// Spieler erzeugen und empfangen
	DebugOut("Requesting player data.");
	const DWORD msg=Send(DPID_SERVERPLAYER,NM_REQUESTPLAYER,0,playertype,0,0,NULL,0,TRUE);
	if (msg==0)
	{
//		game->lpDirectPlay->DestroyPlayer(LocalPlayerID);
//		game->lpDirectPlay->Close();
		return FALSE;
	}
	const DWORD id=DispatchOnly(NM_CREATEOBJECT,NM_OBJECTDATA);

	game->DrawLoading(LOADED_NETWORK-1,33);

	player=(CPlayer*)game->chain->Find(id);
	if (player==NULL)return FALSE;
	player->IsLocal=TRUE;
	HRESULT hr;
	if (FAILED(hr=game->lpDirectPlay->SetPlayerData(LocalPlayerID,&player->id,sizeof(UID),DPSET_REMOTE|DPSET_GUARANTEED)))
	{
		DebugOut("Konnte SpielerID nicht global setzen!");
		game->lpDirectPlay->DestroyPlayer(LocalPlayerID);
		game->lpDirectPlay->Close();
		return FALSE;
	}

	game->DrawLoading(LOADED_NETWORK-1,66);
 
	// Objekte empfangen
	DebugOut("Requesting world data.");
	if (Send(DPID_SERVERPLAYER,NM_REQUESTOBJECTS,player->id,0,0,0,NULL,0,TRUE)==0)
	{
		game->lpDirectPlay->DestroyPlayer(LocalPlayerID);
		game->lpDirectPlay->Close();
		return FALSE;
	}
	DispatchOnly(NM_CREATEOBJECT,NM_OBJECTDATA,NM_WORLDMSG,NM_VARCHANGED);

	game->DrawLoading(LOADED_NETWORK-1,99);

	DebugOut("Objects synchronized.");
	return TRUE;
}