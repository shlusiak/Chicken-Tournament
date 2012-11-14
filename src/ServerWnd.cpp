/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "serverwnd.h"
#include "game.h"
#include "chicken.h"
#include "text.h"
#include "config.h"




class CCountGroupBox:public CGroupBox
{
protected:
	const UID type;
public:
	CCountGroupBox(const int vx,const int vy,const int vw,const int vh,const PCHAR title,CWnd* vowner,const UID vtype);
	virtual void Draw(HDC dc);
};



const int CountObjects(const UID objecttype)
{
	CObject* akt=game->chain->GetFirst();
	int nr=0;
	while (akt)
	{
		if (GetType(akt->id)==objecttype)nr++;
		akt=akt->next;
	}
	return nr;
}



CCountGroupBox::CCountGroupBox(const int vx,const int vy,const int vw,const int vh,const PCHAR title,CWnd* vowner,const UID vtype)
:CGroupBox(vx,vy,vw,vh,title,vowner),type(vtype)
{ }

void CCountGroupBox::Draw(HDC dc)
{
	CGroupBox::Draw(dc);
	SetTextColor(dc,RGB(255,255,255));
	SetBkMode(dc,TRANSPARENT);

	int a=CountObjects(type);
	static CHAR c[50];
	sprintf(&c[0],M_ANZAHL,a);
	TextOut(dc,10,25,&c[0],strlen(&c[0]));
}



CServerPage::CServerPage()
:CPage(game->lpDD,M_SERVEROPTIONS,game->width,game->height,game->wnd),lasttime(0),mustsend(FALSE)
{ }

void CServerPage::Create()
{
	AddChild(new CButton(w-140,h-55,120,35,this,M_BACK,1));

#define AddServerChild(x) { CWnd* w=x; AddChild(w); if (!game->IsServer)w->canfocus=FALSE; }

	int x=30;
	const int plus=35;
	const int minus=70;
	const int breite=200;
	const PCHAR pPlus="+";
	const PCHAR pMinus="-";
	{	// Hühner
		const int y=70;
		AddChild(new CCountGroupBox(x,y,breite,55,M_CHICKEN,this,IDChicken));
		AddServerChild(new CButton(x+breite-plus,y+20,25,25,this,pPlus,2));
		AddServerChild(new CButton(x+breite-minus,y+20,25,25,this,pMinus,3));
	}
	{	// Bauern
		const int y=135;
		AddChild(new CCountGroupBox(x,y,breite,55,M_FARMER,this,IDBauer));
		AddServerChild(new CButton(x+breite-plus,y+20,25,25,this,pPlus,16));
		AddServerChild(new CButton(x+breite-minus,y+20,25,25,this,pMinus,17));
	}
	{	// Ställe
		const int y=200;
		AddChild(new CCountGroupBox(x,y,breite,55,M_STAELLE,this,IDStall));
		AddServerChild(new CButton(x+breite-plus,y+20,25,25,this,pPlus,4));
		AddServerChild(new CButton(x+breite-minus,y+20,25,25,this,pMinus,5));
	}
	{	// Bäume
		const int y=265;
		AddChild(new CCountGroupBox(x,y,breite,55,M_TREES,this,IDTree));
		AddServerChild(new CButton(x+breite-plus,y+20,25,25,this,pPlus,6));
		AddServerChild(new CButton(x+breite-minus,y+20,25,25,this,pMinus,7));
	}
	{	// Gartenkrallen
		const int y=330;
		AddChild(new CCountGroupBox(x,y,breite,55,M_GARTENKRALLEN,this,IDGartenkralle));
		AddServerChild(new CButton(x+breite-plus,y+20,25,25,this,pPlus,8));
		AddServerChild(new CButton(x+breite-minus,y+20,25,25,this,pMinus,9));
	}
	{	// Golfschläger
		const int y=395;
		AddChild(new CCountGroupBox(x,y,breite,55,M_GOLFSCHLAEGER,this,IDGolfer));
		AddServerChild(new CButton(x+breite-plus,y+20,25,25,this,pPlus,10));
		AddServerChild(new CButton(x+breite-minus,y+20,25,25,this,pMinus,11));
	}
	{	// Mähdrescher
		const int y=460;
		AddChild(new CCountGroupBox(x,y,breite,55,M_DRESCHER,this,IDDrescher));
		AddServerChild(new CButton(x+breite-plus,y+20,25,25,this,pPlus,12));
		AddServerChild(new CButton(x+breite-minus,y+20,25,25,this,pMinus,13));
	}
	{	// Plasmakanonen
		const int y=525;
		AddChild(new CCountGroupBox(x,y,breite,55,M_PLASMACANNONS,this,IDPlasmaCannon));
		AddServerChild(new CButton(x+breite-plus,y+20,25,25,this,pPlus,14));
		AddServerChild(new CButton(x+breite-minus,y+20,25,25,this,pMinus,15));
	}

	AddServerChild(new CCheckBox(260,490,breite,25,this,M_KI,50,Config.KI));
	AddServerChild(new CCheckBox(260,520,breite+15,25,this,M_PRESSURE,51,game->PlasmaBallPressure));

	x=260;
	{	// Variablen
		CHAR c[100];
		sprintf(&c[0],M_EINSTELLUNGEN);
		AddChild(new CGroupBox(x,70,800-x-20,410,&c[0],this));

#define AddVariable(xpos,ypos,title,min,max,pos,step,command) \
	AddChild(new CStaticText(x+20+(xpos-1)*250,90-60+ypos*60,230,25,this,title));	\
	AddServerChild(new CSlider(x+20+(xpos-1)*250,90-60+ypos*60+25,230,35,this,min,max,pos,step,command,TRUE,FALSE));

		{
			AddVariable(1,1,M_AIRCONTROL,0.0f,3.0f,game->aircontrol,0.2f,20);
			AddVariable(1,2,M_EGGDAMAGE,1.0f,50.0f,game->damage,2.0f,21);
			AddVariable(1,3,M_GRAVITY,1.0f,20.0f,game->gravity,1.0f,22);
			AddVariable(1,4,M_JUMPMULTIPLIER,0.0f,3.0f,game->jumpspeed,0.2f,23);
		}
		{
			AddVariable(1,5,M_PLASMARATE,0.2f,3.0f,game->plasmarate,0.2f,24);
			AddVariable(1,6,M_PLASMAREGENERATION,0.05f,1.0f,game->plasmaregeneration,0.05f,25);
		}
		{
			AddVariable(2,1,M_REGENERATEPOWER,1.0f,10.0f,game->replenishpower,0.5f,26);
			AddVariable(2,2,M_SITSPEED,1.0f,15.0f,game->sittime,0.5f,27);
			AddVariable(2,3,M_TAKEPOWER,1.0f,20.0f,game->takepower,1.0f,28);
			AddVariable(2,4,M_THROWTIME,0.1f,2.0f,game->throwtime,0.1f,29);
			AddVariable(2,5,M_RESPAWNTIME,1.0f,10.0f,game->respawntime,0.5f,30);
		}
	}
}

void CServerPage::PerformCommand(int command,DWORD p1,int p2)
{
	switch(command)
	{
	case 1:Close(0);
		break;

	case 2:SpawnObject(IDChicken,NULL);
		break;
	case 3:RemoveObject(IDChicken);
		break;

	case 16:SpawnObject(IDBauer,NULL);
		break;
	case 17:RemoveObject(IDBauer);
		break;

	case 4:SpawnObject(IDStall,NULL);
		break;
	case 5:RemoveObject(IDStall);
		break;

	case 6:SpawnObject(IDTree,NULL);
		break;
	case 7:RemoveObject(IDTree);
		break;

	case 8:SpawnObject(IDGartenkralle,NULL);
		break;
	case 9:RemoveObject(IDGartenkralle);
		break;

	case 10:SpawnObject(IDGolfer,NULL);
		break;
	case 11:RemoveObject(IDGolfer);
		break;

	case 12:SpawnObject(IDDrescher,NULL);
		break;
	case 13:RemoveObject(IDDrescher);
		break;

	case 14:SpawnObject(IDPlasmaCannon,NULL);
		break;
	case 15:RemoveObject(IDPlasmaCannon);
		break;


	case 20:game->aircontrol=LongToFloat(p2);
		mustsend=TRUE;
		break;
	case 21:game->damage=LongToFloat(p2);
		mustsend=TRUE;
		break;
	case 22:game->gravity=LongToFloat(p2);
		mustsend=TRUE;
		break;
	case 23:game->jumpspeed=LongToFloat(p2);
		mustsend=TRUE;
		break;
	case 24:game->plasmarate=LongToFloat(p2);
		mustsend=TRUE;
		break;
	case 25:game->plasmaregeneration=LongToFloat(p2);
		mustsend=TRUE;
		break;
	case 26:game->replenishpower=LongToFloat(p2);
		mustsend=TRUE;
		break;
	case 27:game->sittime=LongToFloat(p2);
		mustsend=TRUE;
		break;
	case 28:game->takepower=LongToFloat(p2);
		mustsend=TRUE;
		break;
	case 29:game->throwtime=LongToFloat(p2);
		mustsend=TRUE;
		break;
	case 30:game->respawntime=LongToFloat(p2);
		mustsend=TRUE;
		break;

	case 50:Config.KI=p2;
		mustsend=TRUE;
		break;
	case 51:game->PlasmaBallPressure=p2;
		mustsend=TRUE;
		break;
	}
	InvalidateAll();
}

void CServerPage::RemoveObject(const UID type)const
{
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
			akt->chain->CleanChain();
			return;	
		}
weiter:
		akt=akt->next;
	}
}

void CServerPage::Execute(float elapsed)
{
	CPage::Execute(elapsed);

	lasttime+=elapsed;

	if (lasttime>1.5f)
	{
		lasttime=0.0f;
		if (game->IsServer)
		{
			if (mustsend)game->SendVariables();
			mustsend=FALSE;
		}else{
			int last=GetFocusedChild();
			DeleteChilds();
			Create();
			SetFocus(last);
		}
	}
}