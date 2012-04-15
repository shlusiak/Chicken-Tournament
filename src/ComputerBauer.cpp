/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "computerbauer.h"
#include "weapon.h"
#include "drescher.h"
#include "plasmacannon.h"
#include "gartenkralle.h"
#include "golfer.h"




CComputerBauer::CComputerBauer()
{
	isplayer=FALSE;
	ang.x=0.0f;
}

const float CComputerBauer::RotateTo(const D3DVECTOR lookat,const float elapsed)
{
	float alpha=atan2f(lookat.z-pos.z,lookat.x-pos.x)-g_PI/2.0f;

	const float omega=2.0f;

	while ((alpha>ang.y)&&(abs(alpha-g_PI*2.0f-ang.y)<abs(alpha-ang.y)))
		alpha-=g_PI*2.0f;
	while ((alpha<ang.y)&&(abs(alpha+g_PI*2.0f-ang.y)<abs(alpha-ang.y)))
		alpha+=g_PI*2.0f;

	if (alpha>ang.y)ang.y+=elapsed*omega;
	if (alpha<ang.y)ang.y-=elapsed*omega;
	return (abs(ang.y-alpha));
}

const float CComputerBauer::RotateTo(const float winkel,const float elapsed)
{
	const float omega=2.0f;
	if (winkel<0.0f)ang.y+=elapsed*omega;
	if (winkel>0.0f)ang.y-=elapsed*omega;
	return (abs(winkel)-elapsed*omega);
}

void CComputerBauer::Execute(float elapsed)
{
	CBauer::Execute(elapsed);
	if (dead!=0.0f)return;

	if (game->IsServer)
	{
		// !!! Hier irgendwas intelligentes machen !!!
		if (object!=NULL)HandleWeapon(elapsed);

		if (object==NULL)SearchWeapon(elapsed);
	}
}

void CComputerBauer::Revive()
{
	CBauer::Revive(); 

	// Einfach so den Hut wechseln
	if (IsLocal)NextHut();
}

const BOOLEAN CComputerBauer::SearchWeapon(const float elapsed)
{
	// Nächste Waffe in der Umgebung suchen
	ang.x=0.0f;
	CWeapon* found=NULL;
	float entf=150.0f;

	CObject* o=chain->GetFirst();
	while (o)
	{
		switch(GetType(o->id))
		{
		case IDDrescher:
		case IDPlasmaCannon:
		case IDGartenkralle:
		case IDGolfer:
			{
				CWeapon *w=(CWeapon*)o;
				if ((w->parent==NULL)&&(Magnitude(w->pos-pos)<entf))
				{
					if ((o->IsType(IDPlasmaCannon))&&(((CPlasmaCannon*)w)->munition<3))goto dochnicht;
					if ((o->IsType(IDGartenkralle))&&(((CGartenkralle*)w)->werfer!=NULL))goto dochnicht;
					
					found=w;
					entf=Magnitude(w->pos-pos);

				}
			}
			break;
		}
dochnicht:
		o=o->next;
	}
	if (found!=NULL)
	{
		float w=RotateTo(found->pos,elapsed);
		if (w<0.3f)speed.z=5.0f;else speed.z=1.5f;
		return TRUE;
	}else{
		// Keine brauchbare Waffe gefunden -> Dumm rumlaufen?
		speed=D3DVECTOR(0,0,0);
		return FALSE;
	}
}

void CComputerBauer::HandleDrescher(const float elapsed)
{
	CChicken* c=SearchNextChicken();
	CDrescher* d=(CDrescher*)object;

	if (c==NULL)
	{	// Ist kein Huhn in der Nähe, einfach stumpfsinnig in einem kleinen Kreis weiterfahren
		d->rot=0.35f;	
		return;
	}

	d->acc.z=5.5f;

	float w=GetWinkel(d->ang.y,c->pos);
	if (abs(w)<0.1f)
	{	// Wenn Winkeldiffenenz zu klein, Kurs einfach beibehalten ohne zu lenken
		d->rot=0.0f;
		return;
	}
	// Drehen
	if ((abs(w)>g_PI/2.0f)&&(Magnitude(pos-c->pos)<12.0f)&&(world->GetDistanceFromWall(pos)>5.0f))
	{	// Wenn Ziel hinter Mähdrescher ist, weiterfahren bis es mit der Kurve erwischt werden kann
		d->rot=0.0f;
		return;
	}

	// Sonst Mähdrescher in Richtung des Huhns drehen
	if (w<0.0f)d->rot=-1.0f;
	else d->rot=+1.0f;
}

const float CComputerBauer::GetSchussY(const float x,const float alpha)const
{
	const float c=cosf(alpha);
	const float s=sinf(alpha);
	const float v0=Magnitude(D3DVECTOR(20.0f*c,20.0f*s+3.5f,0));

	return -(game->gravity*0.7f*sqr(x))/(2*sqr(v0)*sqr(c))+x*(s/c)+3.5f*x/(v0*c);
}

void CComputerBauer::HandlePlasmaCannon(const float elapsed)
{
	CPlasmaCannon* p=(CPlasmaCannon*)object;
	if (p->munition<1.0f)
	{	// Weglegen, wenn keine Munni mehr
		p->Throw();
		p->SendNotify(2);
		object=NULL;

//		RotateTo(D3DVECTOR(0,0,0),elapsed);
		return;
	}

	CChicken* c=(CChicken*)SearchNextChicken();
	if (c==NULL)return;

	// um 0.07 RAD weiter nach links drehen um noch richtig zu zielen
	float w=GetWinkel(ang.y,c->pos)-0.07f;
	float entf=Magnitude(pos-c->pos);

	if (abs(w)>0.02f)w=RotateTo(w,elapsed*((entf<13.0f)?1.85f:0.9f));

	if (entf<2.5f)speed.z=-2.5f;
		else if (entf>5.0f)speed.z=3.3f;
		else speed.z=0.0f;

	if ((w<0.1f)&&(p->CanShoot())&&(entf<17.0f))
	{	// Schießen
		float sx=sqrtf(sqr(pos.x-c->pos.x)+sqr(pos.z-c->pos.z));
		// 0.65 weiter oben ansetzen, bei Plasmakanone
		float sy=pos.y+0.65f-c->pos.y;
		float alpha=0.0f;
		float bestdistance=4.0f;

		float aktalpha=-0.7f;
		do
		{
			float y=GetSchussY(sx,aktalpha);

			if (abs(y-sy)<bestdistance)
			{
				bestdistance=abs(y-sy);
				alpha=aktalpha;
			}

			aktalpha+=0.05f;
		}while (aktalpha<g_PI*0.5f);

		if (bestdistance<4.0f)
		{
			ang.x=-alpha;

			if (p->Shoot())p->SendNotify(1);
		}else ang.x=0.0f;
	}
}

void CComputerBauer::HandleGartenkralle(const float elapsed)
{
	CChicken* c=(CChicken*)SearchNextChicken();
	if (c==NULL)return;

	// um 0.07 RAD weiter nach links drehen um noch richtig zu zielen
	float w=GetWinkel(ang.y,c->pos)-0.07f;
	float entf=Magnitude(pos-c->pos);

	if (abs(w)>0.02f)w=RotateTo(w,elapsed*((entf<13.0f)?2.25f:1.0f));

	if (entf>1.3f)speed.z=4.0f;
	else speed.z=0.0f;

	if ((w<0.1f)&&(entf<1.8f))
	{	// Zustechen
		ang.x=-0.45f;

		CGartenkralle* g=(CGartenkralle*)object;
		if (g->Stoss())
		{
			g->SendNotify(2);
			g->EndStoss();
		}
	}
}

void CComputerBauer::HandleGolfer(const float elapsed)
{
	CChicken* c=(CChicken*)SearchNextChicken();
	if (c==NULL)return;

	// um 0.07 RAD weiter nach links drehen um noch richtig zu zielen
	float w=GetWinkel(ang.y,c->pos)-0.07f;
	float entf=Magnitude(pos-c->pos);

	if (abs(w)>0.025f)w=RotateTo(w,elapsed*((entf<13.0f)?2.25f:1.0f));

	if (entf>1.5f)speed.z=4.4f;
	else speed.z=0.0f;

	if ((w<0.1f)&&(entf<2.0f))
	{	// Zuschlagen
		ang.x=-0.2f;

		CGolfer* g=(CGolfer*)object;
		if (g->Schlag())
		{
			g->SendNotify(3);
		}
	}
}

void CComputerBauer::HandleWeapon(const float elapsed)
{
	if (object==NULL)return;

	switch(GetType(object->id))
	{
	case IDDrescher:
		HandleDrescher(elapsed);
		break;
	case IDPlasmaCannon:
		HandlePlasmaCannon(elapsed);
		break;
	case IDGartenkralle:
		HandleGartenkralle(elapsed);
		break;
	case IDGolfer:
		HandleGolfer(elapsed);
		break;

	default:
		Unbind();
		object=NULL;
		break;
	}
}

CChicken* CComputerBauer::SearchNextChicken()const
{
	float entf=80.0f;
	CObject* o=chain->GetFirst();
	CChicken* found=NULL;
	while (o)
	{
		if (o->IsType(IDChicken))
		{
			CChicken* c=(CChicken*)o;
			const BOOLEAN INCLUDEINSTALL=FALSE;
			const BOOLEAN INCLUDEBOUND=TRUE;
			const BOOLEAN INCLUDEGOLFED=TRUE;

			if ((!INCLUDEINSTALL)&&(c->install!=NULL))goto weiter;
			if ((!INCLUDEBOUND)&&(c->bound!=NULL))goto weiter;
			if ((!INCLUDEGOLFED)&&(c->Golfspieler!=0))goto weiter;

			if ((c->dead==0.0f)&&(Magnitude(c->pos-pos)<entf))
			{
				found=c;
				entf=Magnitude(c->pos-pos);
			}
		}
weiter:

		o=o->next;
	}
	return found;
}

const float CComputerBauer::GetWinkel(const float aktw,const D3DVECTOR lookat)const
{
	float alpha=atan2f(lookat.z-pos.z,lookat.x-pos.x)-g_PI*0.5f;

	const float omega=2.0f;

	while ((alpha>ang.y)&&(abs(alpha-g_PI*2.0f-ang.y)<abs(alpha-ang.y)))
		alpha-=g_PI*2.0f;
	while ((alpha<ang.y)&&(abs(alpha+g_PI*2.0f-ang.y)<abs(alpha-ang.y)))
		alpha+=g_PI*2.0f;

	return (ang.y-alpha);
}
