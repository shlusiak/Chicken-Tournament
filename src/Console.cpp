/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "console.h"
#include "game.h"
#include "chicken.h"
#include "gartenkralle.h"
#include "golfer.h"
#include "drescher.h"
#include "world.h"
#include "multiplayer.h"
#include "tree.h"
#include "bauer.h"
#include "chickenplayer.h"
#include "config.h"
#include "plasmacannon.h"
#include "text.h"



const maxconsolechars=200;

const yellow=RGB(255,255,0);
const green=RGB(0,255,0);




CConsole::CConsole()
:surf(NULL),width(512),height(512),Bitmap(0),bRedraw(FALSE),maxconsolelength(57),visible(FALSE)
{ }

BOOLEAN CConsole::Init()
{
	akt=(PCHAR)malloc(maxconsolechars);
	prev=(PCHAR)malloc(maxconsolechars);
	akt[0]='\0';
	prev[0]='\0';
	
	for (int i=0;i<maxlines;i++)lines[i].text=NULL;

	surf=CreateTexture(game->lpDevice,&width,&height,0,FALSE,FALSE,FALSE);

	if ((width<512)||(height<512))
	{
		HDC dc;
		if (SUCCEEDED(surf->GetDC(&dc)))
		{
			Bitmap=CreateCompatibleBitmap(dc,512,512);

			surf->ReleaseDC(dc);
		}
	}

	Add(C_INTRO1,RGB(255,255,255));
	Add("-----",RGB(255,255,255));
	Add(C_INTRO2,RGB(255,0,0));
	Add("<<<<< ----- >>>>>",RGB(255,255,255));
	return TRUE;
}

CConsole::~CConsole()
{
	if (Bitmap)DeleteObject(Bitmap);
	SaveRelease(surf);
	for (int i=0;i<maxlines;i++)if (lines[i].text!=NULL)free(lines[i].text);
	free(prev);
	free(akt);
}

void CConsole::DrawUI(LPDIRECT3DDEVICE7 lpDevice)
{
	if (!visible)return;
	if (bRedraw)
	{
		MyDraw();
		bRedraw=FALSE;
	}

	float g=game->width-100.0f;
	if (game->height-100.0f<g)g=game->height-100.0f;
	const float left=(game->width-g)*0.5f;
	const float top=(game->height-g)*0.5f;

	const float faktor=0.8f;
	D3DTLVERTEX v[4]=
	{
		D3DTLVERTEX(D3DVECTOR(left,top,0),0.5f,D3DRGBA(1,1,1,faktor),0,0,0),
		D3DTLVERTEX(D3DVECTOR(left+g,top,0),0.5f,D3DRGBA(1,1,1,faktor),0,1,0),
		D3DTLVERTEX(D3DVECTOR(left,top+g,0),0.5f,D3DRGBA(1,1,1,faktor),0,0,1),
		D3DTLVERTEX(D3DVECTOR(left+g,top+g,0),0.5f,D3DRGBA(1,1,1,faktor),0,1,1)
	};

	lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,0);

	lpDevice->SetTexture(0,surf);

	if (Config.alpha)
	{
		lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
		lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
		lpDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
	}

	lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,v,4,0);

	if (Config.alpha)
	{
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,0);
	}
	lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,1);
}

const int CConsole::PrintLine(HDC dc,const PCHAR line,const int pos,int *xpos)const
{
	if (strlen(line)<1)
	{
		SIZE s;
		GetTextExtentPoint(dc,"X",1,&s);
		if (xpos)*xpos=0;
		return pos+s.cy;
	}
	PCHAR text=line;
	WORD length;

nextline:
	length=strlen(text);
	if (length>maxconsolelength)
		length=maxconsolelength;

	TextOut(dc,5,pos,text,length);

	SIZE s;
	GetTextExtentPoint(dc,text,length,&s);

	if (strlen(text)!=length)
	{
		text+=maxconsolelength;
		goto nextline;
	}
	if (xpos)*xpos=s.cx;
	return pos+s.cy;
}

void CConsole::MyDraw()
{
	HDC dc,surfdc=0;
	if (surf==NULL)return;
	if (FAILED(surf->GetDC(&dc)))return;
	if (Bitmap)
	{
		surfdc=dc;
		dc=CreateCompatibleDC(surfdc);
		SelectObject(dc,Bitmap);
	}

	const HBRUSH brush=CreateSolidBrush(RGB(64,64,64)),oldbrush=(HBRUSH)SelectObject(dc,brush);
	SelectObject(dc,GetStockObject(WHITE_PEN));

	Rectangle(dc,0,0,512-1,512-1);
	
	SelectObject(dc,oldbrush);
	DeleteObject(brush);


	SetBkMode(dc,TRANSPARENT);

	const int fh=(int)floorf(float(512-10)/float(maxlines+1));
	const int fw=(int)floorf(float(512-20)/float(61));

	const HFONT font=CreateFont(fh,fw,0,0,500,0,0,0,0,0,0,NONANTIALIASED_QUALITY,0,"Courier New"),
		oldfont=(HFONT)SelectObject(dc,font);

	SIZE size;
	GetTextExtentPoint(dc,"x",1,&size);
	maxconsolelength=(int)floorf(float(512-20)/size.cx);


	int zeilen=0;
	{	// Zählen, wieviele Zeilen ich mit Zeilenumbruch habe
		for (int i=0;i<maxlines;i++) if (lines[i].text!=NULL)
		{
			zeilen+=int((float)strlen(lines[i].text)/(maxconsolelength+1))+1;
		}
		// Aktuelle Zeile einrechnen
		zeilen+=int((float)strlen(akt)/(maxconsolelength))+1;
	}

	int pos=5;
	int startzeile=0;
	if (zeilen>maxlines+1)startzeile=zeilen-maxlines-1;

	for (int i=startzeile;i<maxlines;i++)
	{
		if (lines[i].text!=NULL)
		{
			SetTextColor(dc,lines[i].color);
			
			pos=PrintLine(dc,lines[i].text,pos,NULL);
		}
	}

	SetTextColor(dc,RGB(0,214,255));
//	TextOut(dc,5,pos,akt,strlen(akt));
	int xpos;
	pos=PrintLine(dc,akt,pos,&xpos);

	// cursor malen
	SIZE s2;
//	GetTextExtentPoint(dc,akt,strlen(akt),&s);
	GetTextExtentPoint(dc,"X",1,&s2);
	pos-=s2.cy;
	MoveToEx(dc,5+xpos,pos+s2.cy,NULL);
	LineTo(dc,5+xpos+s2.cx,pos+s2.cy);

	SelectObject(dc,oldfont);
	DeleteObject(font);


	if (Bitmap)
	{
		SetStretchBltMode(surfdc,HALFTONE);
		StretchBlt(surfdc,0,0,width,height,dc,0,0,512,512,SRCCOPY);
		RestoreDC(dc,-1);
		DeleteDC(dc);
		dc=surfdc;
	}

	surf->ReleaseDC(dc);
}

void CConsole::Toggle()
{
	visible=!visible;
	Redraw();
//	MyDraw();
}

void CConsole::AddChar(const PCHAR a,const CHAR c)
{
	if (strlen(a)>=maxconsolechars-1)return;
	int l=strlen(a);
	a[l]=CHAR(c);
	a[l+1]='\0';

	Redraw();
//	MyDraw();
}

void CConsole::WMKEYDOWN(const DWORD Key)
{
	switch (Key)
	{
	case VK_OEM_5:
		Toggle();
		return;
	case VK_ESCAPE:
		if (akt[0]=='\0')Toggle();
		else 
		{
			akt[0]='\0';
			Redraw();
		}
		return;
	case VK_BACK:
		if (strlen(akt)>0)akt[strlen(akt)-1]='\0';
		Redraw();
		return;
	case VK_UP:
		strcpy(akt,prev);
		Redraw();
		break;
	case VK_DOWN:
		akt[0]='\0';
		Redraw();
		break;
	case VK_RETURN:
		strcpy(prev,akt);
		Exec();
		akt[0]='\0';
		Redraw();
		return;
	}
}

void CConsole::WMCHAR(const WORD Key)
{
	const PCHAR ALLOWED=
		"? .,!*+-:;#\"$%/()=ß><_@\'\\{}[]|§&äöüÄÖÜ";

	if ((strchr(ALLOWED,CHAR(Key))!=NULL)||(isalnum(Key)))
		AddChar(akt,CHAR(Key));
}

inline const int Parse(const PCHAR org,PCHAR* p1,PCHAR* p2,PCHAR* p3,PCHAR* p4)
{
	*p1=*p2=*p3=*p4=NULL;
	int p=0;
	while (org[p]!='\0')
	{
		if (org[p]==' ')
		{
			org[p]='\0';
			p++;
			while (org[p]==' ')p++;
			if (org[p]=='\0')break;
			org[p-1]='\0';
			if (*p1==NULL)*p1=&org[p];
			else if (*p2==NULL)*p2=&org[p];
			else if (*p3==NULL)*p3=&org[p];
			else if (*p4==NULL)*p4=&org[p];
		}
		p++;
	}
	int i=0;
	if (*p1!=NULL)i++;
	if (*p2!=NULL)i++;
	if (*p3!=NULL)i++;
	if (*p4!=NULL)i++;
	return (i);
}

void CConsole::HandleVariable(const PCHAR name,float* var,const BOOLEAN ServerOnly,const int max,const PCHAR p1)
{
	if (max!=1)
	{	
		CHAR c[50];
		sprintf(&c[0],"%s=%.2f",name,*var);
		Add(&c[0],green);
		return;
	}
	if ((ServerOnly)&&(!game->IsServer))
	{
		Add(C_SERVERONLY,RGB(255,0,0));
		return;
	}
	
	float f=(float)atof(p1);
	if (f==0.0f)
	{
		Add(C_NOFLOAT,RGB(255,0,0));
		return;
	}
	*var=f;
	CHAR c[70];
	sprintf(&c[0],C_VARCHANGED,name,*var);
	Add(&c[0],green);
}

void CConsole::Exec()
{
	Add(akt);
	PCHAR p1,p2,p3,p4;

	const max=Parse(akt,&p1,&p2,&p3,&p4);
	_strupr(akt);	// Befehl in Grossbuchstaben wandeln

	if (strcmp(akt,"FOG")==0)
	{
		DWORD f;
		game->lpDevice->GetRenderState(D3DRENDERSTATE_FOGENABLE,&f);
		game->lpDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE,!f);
		if (f)Add(C_FOGOFF,yellow);else Add(C_FOGON,yellow);
		return;
	}
	if (strcmp(akt,"HELP")==0)
	{
		Add(C_ALLCOMMANDS,yellow);
		Add("   FOG, INFO, SPAWN, EXIT, KICK, KICKTYPE,",yellow);
		Add("   GRAVITY, SPECULAR, NEWWORLD, SAY, SENSITIVITY,",yellow);
		Add("   GORELEVEL, AIRCONTROL, JUMPSPEED, ALPHA, NAME,",yellow);
		Add("   TYPE, SITSPEED, THROWTIME, REPLENISHPOWER,",yellow);
		Add("   TAKEPOWER, DAMAGE, PLASMAREGENERATION,",yellow);
		Add("   PLASMARADIUS, PLASMARATE, KILL, RESPAWNTIME",yellow);
		Add("   PRIV, HARVESTERSPEED, HARVESTERROTSPEED",yellow);
		Add("-----");
		return;
	}
	if (strcmp(akt,"INFO")==0)
	{
		Info();
		return;
	}
	if (strcmp(akt,"SPAWN")==0)
	{
		if (!game->IsServer)
		{
			Add(C_SERVERONLY,RGB(255,0,0));
			return;
		}
		if (max!=1)
		{
			Add(C_SPAWNPARAMETER,yellow);
			for (int i=0;i<sizeof(C_SPAWNPARAMETERS)/sizeof(C_SPAWNPARAMETERS[0]);i++)
			{
				CHAR c[100];
				sprintf(&c[0],"  %d=%s",i+1,C_SPAWNPARAMETERS[i]);
				Add(&c[0],yellow);
			}
			return;
		}
	
		CHAR c[100];
		strcpy(&c[0],"");
		int n=0;
		CObject* no=NULL;

		if (strcmp(p1,"1")==0)
		{
			no=SpawnObject(IDChicken,&n);
			sprintf(&c[0],C_SPAWNED,C_SPAWNPARAMETERS[0],n);
		}
		if (strcmp(p1,"2")==0)
		{
			no=SpawnObject(IDGartenkralle,&n);
			sprintf(&c[0],C_SPAWNED,C_SPAWNPARAMETERS[1],n);
		}
		if (strcmp(p1,"3")==0)
		{
			no=SpawnObject(IDGolfer,&n);
			sprintf(&c[0],C_SPAWNED,C_SPAWNPARAMETERS[2],n);
		}
		if (strcmp(p1,"4")==0)
		{
			no=SpawnObject(IDDrescher,&n);
			sprintf(&c[0],C_SPAWNED,C_SPAWNPARAMETERS[3],n);
		}
		if (strcmp(p1,"5")==0)
		{
			no=SpawnObject(IDTree,&n);
			sprintf(&c[0],C_SPAWNED,C_SPAWNPARAMETERS[4],n);
		}
		if (strcmp(p1,"6")==0)
		{
			no=SpawnObject(IDStall,&n);
			sprintf(&c[0],C_SPAWNED,C_SPAWNPARAMETERS[5],n);
		}
		if (strcmp(p1,"7")==0)
		{
			no=SpawnObject(IDPlasmaCannon,&n);
			sprintf(&c[0],C_SPAWNED,C_SPAWNPARAMETERS[6],n);
		}
		if (strcmp(p1,"8")==0)
		{
			no=SpawnObject(IDItem,&n);;
			sprintf(&c[0],C_SPAWNED,C_SPAWNPARAMETERS[7],n);
		}
		if (strcmp(p1,"9")==0)
		{
			no=SpawnObject(IDBauer,&n);;
			sprintf(&c[0],C_SPAWNED,C_SPAWNPARAMETERS[8],n);
		}

		if (no!=NULL)
		{
			Add(&c[0],yellow);
		}else Add(C_INVALIDPARAMETER,RGB(255,0,0));

		return;
	}
	if (strcmp(akt,"EXIT")==0)
	{
		DestroyWindow(game->wnd);
		return;
	}
	if (strcmp(akt,"KICK")==0)
	{
		if (!game->IsServer)
		{
			Add(C_SERVERONLY,RGB(255,0,0));
			return;
		}
		if (max!=1)
		{
			Add(C_INVALIDUID,RGB(255,0,0));
			return;
		}
		DWORD id=atol(p1);
		if (id==0)
		{
			Add(C_INVALIDUID,RGB(255,0,0));
			return;
		}
		CObject* o=game->chain->Find(id);
		if (o==NULL)
		{
			Add(C_UIDNOTFOUND,RGB(255,0,0));
			return;
		}
		o->Unbind();
		o->Remove();
		Send(DPID_ALLPLAYERS,NM_REMOVE,o->id,0,0,0,NULL,0,TRUE);
		CHAR c[100];
		sprintf(&c[0],C_OBJECTREMOVED,id,GetTypeName(id));
		Add(&c[0],yellow);
		o->chain->CleanChain();

		if (o==player)
		{
			game->errormessage=E_KICKEDYOURSELF;
			PostQuitMessage(1);
		}
		return;
	}
	if (strcmp(akt,"KICKTYPE")==0)
	{
		if (!game->IsServer)
		{
			Add(C_SERVERONLY,RGB(255,0,0));
			return;
		}
		if (max!=1)
		{
			Add(C_INVALIDPARAMETER,RGB(255,0,0));
			return;
		}
		DWORD type=atol(p1);
		if (type==0)
		{
			Add(C_INVALIDPARAMETER,RGB(255,0,0));
			return;
		}

		CObject* akt=game->chain->GetFirst();
		while (akt)
		{
			if (GetType(akt->id)==type)
			{
				if ((type==IDChicken)&&(((CChicken*)akt)->isplayer))goto weiter;
				if ((type==IDBauer)&&(((CBauer*)akt)->isplayer))goto weiter;
	
				akt->Unbind();
				akt->Remove();
				Send(DPID_ALLPLAYERS,NM_REMOVE,akt->id,0,0,0,NULL,0,TRUE);

				CHAR c[100];
				sprintf(&c[0],C_OBJECTREMOVED,akt->id,GetTypeName(akt->id));
				Add(&c[0],yellow);

				akt->chain->CleanChain();
				return;	
			}
weiter:
			akt=akt->next;
		}

		Add(C_UIDNOTFOUND,RGB(255,0,0));
		return;
	}

	if (strcmp(akt,"SPECULAR")==0)
	{
		DWORD s;
		game->lpDevice->GetRenderState(D3DRENDERSTATE_SPECULARENABLE ,&s);
		game->lpDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE,!s);
		if (s)Add(C_SPECULAROFF,yellow);else Add(C_SPECULARON,yellow);
		return;
	}
	if (strcmp(akt,"NEWWORLD")==0)
	{
		if (!game->IsServer)
		{
			Add(C_SERVERONLY,RGB(255,0,0));
			return;
		}
		const float w=float(world->GetWidth()),h=float(world->GetHeight());
		world->CreateScene(int(w/5.0f),int(h/5.0f),w,h);
		world->SendData(DPID_ALLPLAYERS);

		Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,10,0,0,NULL,0,TRUE);

		CObject* akt=game->chain->GetFirst();
		while (akt)
		{
			akt->NewWorld();
			akt=akt->next;
		}
		Add(C_NEWWORLD,yellow);
		return;
	}
	if (strcmp(akt,"SAY")==0)
	{
		if (game->lpDirectPlay==NULL)
		{
			Add(E_SESSIONLOST,RGB(255,0,0));
			return;
		}
		if (max==0)
		{
			Add(C_INVALIDPARAMETER,RGB(255,0,0));
			return;
		}
		PCHAR t=prev;
		while ((*t!=' ')&&(*t!='\0'))t++;
		if (*t==' ')t++;

		Send(DPID_ALLPLAYERS,NM_CHAT,0,strlen(t),0,0,t,strlen(t)+1,TRUE);
		Add(C_MESSAGESENT,yellow);
		
		CHAR c[100];
		LPVOID v;
		DWORD s=0;
		game->lpDirectPlay->GetPlayerName(LocalPlayerID,NULL,&s);
		v=malloc(s);
		game->lpDirectPlay->GetPlayerName(LocalPlayerID,v,&s);

		sprintf(&c[0],"%s: %s",((DPNAME*)v)->lpszLongNameA,t);
		free(v);

		DebugOut(&c[0]);
		game->ticker->Add(&c[0],CHATCOLOR);

		return;
	}
	if (strcmp(akt,"PRIV")==0)
	{
		if (game->lpDirectPlay==NULL)
		{
			Add(E_SESSIONLOST,RGB(255,0,0));
			return;
		}
		if (max<=1)
		{
			Add(C_INVALIDPARAMETER,RGB(255,0,0));
			return;
		}
		UID uid=atoi(p1);
		DPID dpid=UID2DPID(uid);
		if ((uid==0)||(dpid==DPID(-1)))
		{
			Add(C_INVALIDPARAMETER,RGB(255,0,0));
			return;
		}

		PCHAR t=prev;
		while ((*t!=' ')&&(*t!='\0'))t++;
		if (*t==' ')t++;
		while ((*t!=' ')&&(*t!='\0'))t++;
		if (*t==' ')t++;

		Send(dpid,NM_CHAT,0,strlen(t),0,0,t,strlen(t)+1,TRUE);
		Add(C_MESSAGESENT,yellow);
		
		CHAR c[100];
		LPVOID v;
		DWORD s=0;
		game->lpDirectPlay->GetPlayerName(LocalPlayerID,NULL,&s);
		v=malloc(s);
		game->lpDirectPlay->GetPlayerName(LocalPlayerID,v,&s);

		sprintf(&c[0],"%s: %s",((DPNAME*)v)->lpszLongNameA,t);
		free(v);

		DebugOut(&c[0]);
		game->ticker->Add(&c[0],CHATCOLOR);

		return;
	}

	if (strcmp(akt,"SENSITIVITY")==0)
	{
		float f=(float)Config.sensitivity;
		HandleVariable("SENSITIVITY",&f,FALSE,max,p1);
		Config.sensitivity=int(f);
		return;
	}
	if (strcmp(akt,"GRAVITY")==0)
	{
		HandleVariable("GRAVITY",&game->gravity,TRUE,max,p1);
		if (game->IsServer)Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,1,0,0,&game->gravity,sizeof(game->gravity),TRUE);
		return;
	}
	if (strcmp(akt,"GORELEVEL")==0)
	{
		HandleVariable("GORELEVEL",&game->gorelevel,FALSE,max,p1);
		if (game->gorelevel>10.0f)
		{
			Add(C_INSANEGORELEVEL,RGB(255,0,0));
			if (game->gorelevel>125.0f)game->gorelevel=125.0f;
		}
		return;
	}
	if (strcmp(akt,"AIRCONTROL")==0)
	{
		HandleVariable("AIRCONTROL",&game->aircontrol,TRUE,max,p1);
		if (game->IsServer)Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,2,0,0,&game->aircontrol,sizeof(game->aircontrol),TRUE);
		return;
	}
	if (strcmp(akt,"JUMPSPEED")==0)
	{
		HandleVariable("JUMPSPEED",&game->jumpspeed,TRUE,max,p1);
		if (game->IsServer)Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,3,0,0,&game->jumpspeed,sizeof(game->jumpspeed),TRUE);
		return;
	}
	if (strcmp(akt,"ALPHA")==0)
	{
		Config.alpha=!Config.alpha;
		if (Config.alpha)Add(C_ALPHABLENDINGON,yellow);
			else Add(C_ALPHABLENDINGOFF,yellow);
		return;
	}
	if (strcmp(akt,"NAME")==0)
	{
		if (game->lpDirectPlay==NULL)
		{
			Add(E_SESSIONLOST,RGB(255,0,0));
			return;
		}
		CHAR c[100];
		LPVOID v;
		DWORD s=0;
		DPNAME name;
		if (max==0)
		{
			game->lpDirectPlay->GetPlayerName(LocalPlayerID,NULL,&s);
			v=malloc(s);
			game->lpDirectPlay->GetPlayerName(LocalPlayerID,v,&s);

			PCHAR old=((DPNAME*)v)->lpszLongNameA;

			sprintf(&c[0],C_YOURNAME,old);
			Add(&c[0],green);

			free(v);
			return;
		}
		PCHAR t=prev;
		while ((*t!=' ')&&(*t!='\0'))t++;
		if (*t==' ')t++;

		game->lpDirectPlay->GetPlayerName(LocalPlayerID,NULL,&s);
		v=malloc(s);
		game->lpDirectPlay->GetPlayerName(LocalPlayerID,v,&s);

		name.lpszLongNameA=name.lpszShortNameA=t;
		name.dwSize=sizeof(name);
		name.dwFlags=0;
		game->lpDirectPlay->SetPlayerName(LocalPlayerID,&name,DPSET_GUARANTEED|DPSET_REMOTE);

		PCHAR old=((DPNAME*)v)->lpszLongNameA;
		sprintf(&c[0],T_NAMECHANGED,old,t);
		game->ticker->Add(&c[0],D3DRGB(1,1,0));
		DebugOut(&c[0]);

		Send(DPID_ALLPLAYERS,NM_NAMECHANGED,0,LocalPlayerID,0,0,old,strlen(old)+2,TRUE);

		free(v);
		Add(C_NAMECHANGED,green);
		return;
	}
	if (strcmp(akt,"TYPE")==0)
	{
		int i=0;
		if (max==1)i=atoi(p1);
		if (max==2)i=atoi(p2);
		if ((i!=1)&&(i!=2))
		{
			Add(C_NEWRACE,RGB(255,255,0));
			for (i=0;i<sizeof(T_PLAYERTYPES)/sizeof(T_PLAYERTYPES[0]);i++)
			{
				CHAR c[100];
				sprintf(&c[0],"  %d=%s",i+1,T_PLAYERTYPES[i]);
				Add(&c[0],yellow);
			}
			return;
		}

		if (max==2)
		{
			if (!game->IsServer)
			{
				Add(C_SERVERONLY,RGB(255,0,0));
				return;
			}
			DPID dpid=UID2DPID(atoi(p1));
			if (dpid==DPID(-1))
			{
				Add(C_INVALIDPARAMETER,RGB(255,0,0));
				return;
			}
			Send(dpid,NM_TYPECHANGE,0,i,0,0,NULL,0,TRUE,NULL);
			Add(C_PLAYERTYPECHANGED,yellow);
			return;
		}
		

		player->Unbind();
		player->Remove();

		if (game->IsServer)
		{
			if (i==1)game->chain->Add(player=new CBauer());
			if (i==2)game->chain->Add(player=new CChickenPlayer());

			if (game->lpDirectPlay)if (FAILED(game->lpDirectPlay->SetPlayerData(LocalPlayerID,&player->id,sizeof(UID),DPSET_REMOTE|DPSET_GUARANTEED)))
			{
				DebugOut("Konnte SpielerID nicht global setzen!");
			}
			Send(DPID_ALLPLAYERS,NM_CREATEOBJECT,player->id,3,0,0,NULL,0,TRUE);
			player->SendData(DPID_ALLPLAYERS);
		}else{
			DebugOut("Requesting new player.");
			Send(DPID_SERVERPLAYER,NM_REQUESTPLAYER,0,i,0,0,NULL,0,TRUE);
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

		Add(C_PLAYERTYPECHANGED,yellow);
		return;
	}
	if (strcmp(akt,"SITSPEED")==0)
	{
		HandleVariable("SITSPEED",&game->sittime,TRUE,max,p1);
		if (game->IsServer)Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,4,0,0,&game->sittime,sizeof(game->sittime),TRUE);
		return;
	}
	if (strcmp(akt,"THROWTIME")==0)
	{
		HandleVariable("THROWTIME",&game->throwtime,TRUE,max,p1);
		if (game->IsServer)Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,5,0,0,&game->throwtime,sizeof(game->throwtime),TRUE);
		return;
	}
	if (strcmp(akt,"REPLENISHPOWER")==0)
	{
		HandleVariable("REPLENISHPOWER",&game->replenishpower,TRUE,max,p1);
		if (game->IsServer)Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,6,0,0,&game->replenishpower,sizeof(game->replenishpower),TRUE);
		return;
	}
	if (strcmp(akt,"TAKEPOWER")==0)
	{
		HandleVariable("TAKEPOWER",&game->takepower,TRUE,max,p1);
		if (game->IsServer)Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,7,0,0,&game->takepower,sizeof(game->takepower),TRUE);
		return;
	}
	if (strcmp(akt,"DAMAGE")==0)
	{
		HandleVariable("DAMAGE",&game->damage,TRUE,max,p1);
		if (game->IsServer)Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,8,0,0,&game->damage,sizeof(game->damage),TRUE);
		return;
	}
	if (strcmp(akt,"PLASMAREGENERATION")==0)
	{
		HandleVariable("PLASMAREGENERATION",&game->plasmaregeneration,TRUE,max,p1);
		if (game->IsServer)Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,11,0,0,&game->plasmaregeneration,sizeof(game->plasmaregeneration),TRUE);
		return;
	}
	if (strcmp(akt,"PLASMARADIUS")==0)
	{
		HandleVariable("PLASMARADIUS",&game->plasmaradius,TRUE,max,p1);
		if (game->IsServer)Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,12,0,0,&game->plasmaradius,sizeof(game->plasmaradius),TRUE);
		return;
	}
	if (strcmp(akt,"PLASMARATE")==0)
	{
		HandleVariable("PLASMARATE",&game->plasmarate,TRUE,max,p1);
		if (game->IsServer)Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,13,0,0,&game->plasmarate,sizeof(game->plasmarate),TRUE);
		return;
	}
	if (strcmp(akt,"RESPAWNTIME")==0)
	{
		HandleVariable("RESPAWNTIME",&game->respawntime,TRUE,max,p1);
		if (game->IsServer)Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,14,0,0,&game->respawntime,sizeof(game->respawntime),TRUE);
		return;
	}

	if (strcmp(akt,"HARVESTERSPEED")==0)
	{
		HandleVariable("HARVESTERSPEED",&game->harvesterspeed,TRUE,max,p1);
		if (game->IsServer)Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,15,0,0,&game->harvesterspeed,sizeof(game->harvesterspeed),TRUE);
		return;
	}
	if (strcmp(akt,"HARVESTERROTSPEED")==0)
	{
		HandleVariable("HARVESTERROTSPEED",&game->harvesterrotspeed,TRUE,max,p1);
		if (game->IsServer)Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,16,0,0,&game->harvesterrotspeed,sizeof(game->harvesterrotspeed),TRUE);
		return;
	}


	if (strcmp(akt,"KILL")==0)
	{
		if (max==1)
		{
			if (!game->IsServer)
			{
				Add(C_SERVERONLY,RGB(255,0,0));
				return;
			}

			CPlayer* player=(CPlayer*)game->chain->Find((UID)atoi(p1));
			if (player==NULL)
			{
				Add(C_INVALIDPARAMETER,RGB(255,0,0));
				return;
			}

			if (player->dead!=0.0f)return;
			switch (GetType(player->id))
			{
			case IDChicken:
				((CChicken*)player)->Splat(SPLAT_NORMAL);
				break;
			case IDBauer:
				((CBauer*)player)->Hit(150);
				((CBauer*)player)->SendNotify(22,FloatToLong(150.0f),0,NULL);
				break;
			default:Add(C_INVALIDPARAMETER,RGB(255,0,0));
				return;
				break;
			}

			return;
		}
		if (GetType(player->id)==IDChicken)
		{
			if (((CChicken*)player)->dead!=0.0f)goto schontot;
			if (game->IsServer) ((CChicken*)player)->Splat(SPLAT_NORMAL);
				else player->SendNotify(1,SPLAT_NORMAL);
		}
		if (GetType(player->id)==IDBauer)
		{
			if (((CBauer*)player)->dead!=0.0f)goto schontot;
			((CBauer*)player)->Hit(150);
			((CBauer*)player)->SendNotify(22,FloatToLong(150),0,NULL);
		}
schontot:
		Add(T_TOT,RGB(0,255,0));
		return;
	}

#ifdef _DEBUG
	if (strcmp(akt,"COUNT")==0)
	{
		CHAR c[100];
		sprintf(&c[0],"Chain:    %d",game->chain->Count());
		Add(&c[0],yellow);
		sprintf(&c[0],"Sounds:   %d",game->sounds->Count());
		Add(&c[0],yellow);
		sprintf(&c[0],"Gimmicks: %d",game->gimmick->Count());
		Add(&c[0],yellow);

		return;
	}
#endif
	if (strcmp(akt,"MEM")==0)
	{
		D3DDEVINFO_TEXTUREMANAGER info;
		CHAR c[100];

		game->lpDevice->GetInfo(D3DDEVINFOID_TEXTUREMANAGER,&info,sizeof(info));

		sprintf(&c[0],"Number of Textures used: %d",info.dwNumTexturesUsed);
		Add(&c[0],RGB(64,128,255));

		sprintf(&c[0],"Number of Textures in Videomemory used: %d",info.dwNumUsedTexInVid);
		Add(&c[0],RGB(64,128,255));

		sprintf(&c[0],"Number of Textures in Videomemory: %d",info.dwWorkingSet);
		Add(&c[0],RGB(64,128,255));

		sprintf(&c[0],"Videomemory used for Textures: %d KB",info.dwWorkingSetBytes/1024);
		Add(&c[0],green);

		DDSCAPS2 ddsCaps2;
		DWORD dwTotal;
		DWORD dwFree;

		ZeroMemory(&ddsCaps2, sizeof(ddsCaps2));
 
		ddsCaps2.dwCaps = DDSCAPS_TEXTURE; 
		game->lpDD->GetAvailableVidMem(&ddsCaps2, &dwTotal, &dwFree); 

		sprintf(&c[0],"Total Videomemory: %d KB",dwTotal/1024);
		Add(&c[0],green);
		sprintf(&c[0],"Free Videomemory: %d KB",dwFree/1024);
		Add(&c[0],green);

		return;
	}
#ifdef _DEBUG
	if (strcmp(akt,"FACES")==0)
	{
		int i=0;
		CHAR c[200];

		i+=game->chain->NumFaces();
		i+=game->gimmick->NumFaces();

		sprintf(&c[0],"Faces of all objects: %d",i);

		Add(&c[0],yellow);

		return;
	}
#endif
	if (strcmp(akt,"GRAS")==0)
	{
		Config.Gras=!Config.Gras;

		if (Config.Gras)
			Add("Gras aktiviert",yellow);
		else Add("Gras deaktiviert",yellow);
	
		return;
	}
	if (strcmp(akt,"HALT")==0)
	{
		__asm int 3
		return;
	}

	Add(C_INVALIDCOMMAND,RGB(255,0,0));
}

void CConsole::Add(const PCHAR s,const COLORREF color)
{
	if (s==NULL)return;
	if (s[0]=='\0')return;
	if (lines[maxlines-1].text!=NULL)
	{
		free(lines[0].text);
		for (int i=1;i<maxlines;i++)
		{
			lines[i-1].text=lines[i].text;
			lines[i-1].color=lines[i].color;
		}
		lines[maxlines-1].text=NULL;
	}
	lines[maxlines-1].text=(PCHAR)malloc(strlen(s)+2);
	strcpy(lines[maxlines-1].text,s);
	lines[maxlines-1].color=color;

	Redraw();
//	MyDraw();
}

void CConsole::Info()
{
	CObject* o=game->chain->GetFirst();
	Add(C_OBJECTINFOS,yellow);

	int i=0;
	
	CHAR c[80];
	while (o)
	{
		sprintf(&c[0],C_OBJECTINFO,i+1,o->id,GetTypeName(o->id));
		Add(&c[0],yellow);
		DebugOut(&c[0]);

		i++;
		o=o->next;
	}
}

void CConsole::Invalidate()
{
	SaveRelease(surf);
}

void CConsole::Restore()
{
	SaveRelease(surf);
	width=height=512;
	surf=CreateTexture(game->lpDevice,&width,&height,0,FALSE,FALSE,FALSE);
}