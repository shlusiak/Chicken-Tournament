/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "game.h"
#include "player.h"
#include "sounds.h"
#include "world.h"
#include "config.h"
#include "text.h"



CPlayer::~CPlayer()
{
	if (lpRadar)delete lpRadar;
	if (message)message->Remove();
	if (multikill)multikill->Remove();
	if (godlike)godlike->Remove();
}

BOOLEAN CPlayer::Init()
{
	pos=world->Rand(0.0f);
	speed=addspeed=ang=D3DVECTOR(0,0,0);
	hit=0.0f;
	lasthit=0.0f;
	killsarow=0;
	killswithoutdeath=0;
	camera=0;
	MakeBillboardVertices(&BurgerVertices[0],10,90-int(bh*0.5f),60,bh);

	if (isplayer)
	{
		message=new CFadeText(1);
		godlike=new CFadeText(-1);
		multikill=new CFadeText(-2);
		game->AddGimmick(message);
		game->AddGimmick(multikill);
		game->AddGimmick(godlike);

		lpRadar=new CRadar(this);
	}
	return TRUE;
}

void CPlayer::GetViewMatrix(D3DMATRIX &view)
{
	D3DMATRIX m;
	D3DUtil_SetTranslateMatrix(view,-pos);
	D3DUtil_SetRotateYMatrix(m,ang.y);
	D3DMath_MatrixMultiply(view,view,m);

	D3DUtil_SetRotateXMatrix(m,ang.x);
	D3DMath_MatrixMultiply(view,view,m);
}

void CPlayer::Execute(float elapsed)
{
	if (dead>0)
	{
		dead-=elapsed;
		if (dead<0)
		{
			dead=0.0f;
			Revive();
		}
	}
	if (lasthit!=0)
	{
		lasthit-=elapsed;
		if (lasthit<0.0f)
		{
			lasthit=0.0f;
			killsarow=0;
		}
	}
	if (name)
	{
		namevalidity-=elapsed;
		if (namevalidity<=0.0f)
		{
			free(name);
			name=NULL;
		}
	}
	
	if (IsLocal)
		if (lpRadar)lpRadar->Execute(elapsed);
}

void CPlayer::SetSoundListener()
{
	if (!IsLocal)return;
	if (game->lpDirectSoundListener==NULL)return;

	D3DMATRIX m;

	D3DUtil_SetRotateYMatrix(m,-ang.y);
	
	D3DVECTOR v1;
	D3DMath_VectorMatrixMultiply(v1,D3DVECTOR(0,0,1),m);

	DS3DLISTENER LP;
	ZeroMemory(&LP,sizeof(DS3DLISTENER));
	LP.dwSize=sizeof(DS3DLISTENER);
	game->lpDirectSoundListener->GetAllParameters(&LP);

	LP.vPosition=D3DVECTOR(pos.x,pos.y,pos.z);
	LP.vOrientFront=v1;
	LP.vOrientTop=D3DVECTOR(0,1,0);
	v1*=Magnitude(speed);
	LP.vVelocity=v1;
	game->lpDirectSoundListener->SetAllParameters(&LP,DS3D_DEFERRED);

	game->lpDirectSoundListener->CommitDeferredSettings();
}

void CPlayer::HasHit(const UID whom)
{
	killswithoutdeath++;
	if ((!IsLocal)||(!isplayer))return;

	if ((lasthit>0.0f)&&(Config.unrealext))
	{	// Soundausgabe bei Multikill
		// Erstmal dynamisch UT Sounds nachladen, wenn noch nicht geschehen
		game->LoadUTSounds();

		int tnr=killsarow-1;
		if (tnr>=7)tnr=7-1;	// Auf max. 7 Stufen beschränken

		const PCHAR text=UnrealText[tnr];

		static const BYTE soundmap[]=
		{
			6,7,8,9,31,32,33
		};
		game->PlayAnnouncer(game->lpBuffers[soundmap[tnr]]);

		multikill->SetText(text,RGB(255,0,0));
	}
	const int num=9;
	if ((Config.unrealext)&&((killswithoutdeath%num)==0)&&(killswithoutdeath<=(num)*6))
	{	// "Killingspree", "godlike", ...
		game->LoadUTSounds();

		const int nr=20-1+killswithoutdeath/num;
		const int tnr=killswithoutdeath/num-1;
		const PCHAR text=UnrealText2[tnr];

		game->PlayAnnouncer(game->lpBuffers[nr]);

		godlike->SetText(text,RGB(255,0,0));
	}
	lasthit=4.0f;
	killsarow++;
	if (IsLocal)game->lpMusic->PlayModif("Hit");
}

void CPlayer::NextCam()
{
	++camera%=10;
}

void CPlayer::DrawUI(LPDIRECT3DDEVICE7 lpDevice)
{
	if ((dead==0.0f)&&(Config.ShowPlayernames))
	{
		D3DVECTOR p=pos;

		if (isplayer && IsLocal)goto weiter;
		p.y+=0.85f;

		if (name==NULL)
		{
			UpdateName();
			if (name==NULL)goto weiter;

			// Nach 8 Sekunden wird der Name aktualisiert
			namevalidity=8.0f;
		}

		DWORD Color;
		if (player!=NULL)
			if (GetType(player->id)==GetType(id))
				Color=D3DRGB(0.2f,1.0f,0.2f);
			else Color=D3DRGB(1.0f,0.2f,0.2f);
		else Color=D3DRGB(1,1,1);

		game->DrawName(p,name,Color);
	}

weiter:

	if ((!IsLocal)||(!isplayer))return;

	if (lpRadar)lpRadar->DrawUI(lpDevice);

	if (lpBurger()==NULL)return;

	const float grenze=3.8f;
	DWORD color=D3DRGB(1,1,1);
	if (lasthit>grenze)
	{
		float c=(lasthit-grenze)/(4.0f-grenze);	// von 1 bis 0

		c=1-sinf(c*g_PI);

		color=D3DRGB(0.7f+c*0.3f,c,c);
	}

	for (int i=0;i<4;i++)BurgerVertices[i].color=color;

	if (Config.alpha)
	{
		lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,FALSE);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,TRUE);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHAREF,200);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHAFUNC,D3DCMP_GREATEREQUAL);
	}

	lpDevice->SetTexture(0,lpBurger());

	lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,BurgerVertices,4,0);

	lpDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,0);
	lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,TRUE);
}

void CPlayer::Die()
{
	speed=addspeed=D3DVECTOR(0,0,0);
	killswithoutdeath=0;
}

void CPlayer::Invalidate()
{
	if (lpRadar)lpRadar->Invalidate();
}

void CPlayer::Restore()
{
	if (lpRadar)lpRadar->Restore();
}

void CPlayer::UpdateName()
{
	CHAR c[100];
	name=NULL;

	const DPID dpid=UID2DPID(id);
	if (dpid==DPID(-1))goto nix;

	if (GetPlayerName(dpid,&c[0])==0)goto nix;

	name=(PCHAR)malloc(strlen(&c[0])+2);
	strcpy(name,&c[0]);
	return;

nix:
#ifdef _DEBUG
	sprintf(&c[0],"UID #%d",id);
	name=(PCHAR)malloc(strlen(&c[0])+2);
	strcpy(name,&c[0]);
#endif
	return;
}