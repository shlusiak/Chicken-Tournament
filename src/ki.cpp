/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "chicken.h"
#include "ki.h"
#include "stall.h"
#include "bauer.h"
#include "items.h"
#include "ei.h"
#include "drescher.h"






inline float GetWurfY(const float x,const float alpha)
{
	const float c=cosf(alpha);
	const float s=sinf(alpha);
	const float v0=Magnitude(D3DVECTOR(ei_wurfspeed*c,ei_wurfspeed*s+ei_offsety,0));

	return -(game->gravity*sqr(x))/(2*sqr(v0)*sqr(c))+x*(s/c)+ei_offsety*x/(v0*c);
}




CChickenKI::CChickenKI(CChicken* vowner):
omega(2.1f+randf()*1.0f),
escapespeed(2.6f+randf()*0.9f),
movespeed(1.6f+randf()*0.4f),
walkspeed(0.8f+randf()*0.25f),
bauerentfernung(12.0f+randf()*15.0f),
stallentfernung(13.0f+randf()*12.0f),
maxattackrange(7.5f+randf()*2.5f),
minescaperange(3.3f+randf()*0.7f),
owner(vowner)
{ }



CStall* CChickenKI::FindNextStall(const float maxdistance)const
{
	CObject *akt=owner->chain->GetFirst();
	CStall* found=NULL;
	float distance=maxdistance;

	while (akt)
	{
		if ((GetType(akt->id)==IDStall))
		{
			const float d=Magnitude(((CStall*)akt)->pos-owner->pos);
			if (d<distance)
			{
				found=(CStall*)akt;
				distance=d;
			}
		}
		akt=akt->next;
	}
	return found;
}

CBauer* CChickenKI::FindNextBauer(const float maxdistance)const
{
	CObject *akt=owner->chain->GetFirst();
	CBauer* found=NULL;
	float distance=maxdistance;

	while (akt)
	{
		if ((GetType(akt->id)==IDBauer)&&(((CBauer*)akt)->dead==0.0f))
		{
			const float d=Magnitude(((CBauer*)akt)->pos-owner->pos);
			if (d<distance)
			{
				found=(CBauer*)akt;
				distance=d;
			}
		}
		akt=akt->next;
	}
	return found;
}

CItem* CChickenKI::FindNextItem(const DWORD subid,const float maxdistance)const
{
	CObject *akt=owner->chain->GetFirst();
	CItem* found=NULL;
	float distance=maxdistance;

	while (akt)
	{
		if ((GetType(akt->id)==IDItem)&&(((CItem*)akt)->subid==subid))
		{
			const float d=Magnitude(((CItem*)akt)->pos-owner->pos);
			if (d<distance)
			{
				found=(CItem*)akt;
				distance=d;
			}
		}
		akt=akt->next;
	}
	return found;
}

void CChickenKI::Move(const float speed,const float elapsed)
{
	D3DVECTOR s2;
	owner->speed=D3DVECTOR(0,0,speed);
	D3DMATRIX m;
	D3DUtil_SetRotateYMatrix(m,owner->ang.y);
	D3DMath_VectorMatrixMultiply(s2,owner->speed,m);
	owner->pos+=(s2*float(elapsed));

	owner->morphphase+=speed*elapsed*2.2f;
}

const float CChickenKI::RotateTo(const D3DVECTOR viewpoint,const float elapsed)
{
	float alpha=atan2f(viewpoint.x-owner->pos.x,viewpoint.z-owner->pos.z);

	while ((alpha>owner->ang.y)&&(abs(alpha-g_PI*2.0f-owner->ang.y)<abs(alpha-owner->ang.y)))
		alpha-=g_PI*2.0f;
	while ((alpha<owner->ang.y)&&(abs(alpha+g_PI*2.0f-owner->ang.y)<abs(alpha-owner->ang.y)))
		alpha+=g_PI*2.0f;

	if (alpha>owner->ang.y)owner->ang.y+=elapsed*omega;
	if (alpha<owner->ang.y)owner->ang.y-=elapsed*omega;
	return (abs(owner->ang.y-alpha));
}

inline void CChickenKI::MoveTo(const D3DVECTOR dest,const float elapsed)
{
	const float x=RotateTo(dest,elapsed);
//	if (x<2.2f)
		Move(movespeed,elapsed);
}

inline void CChickenKI::EscapeFrom(const D3DVECTOR from,const float elapsed)
{
	RotateTo(owner->pos-2*(from-owner->pos),elapsed);
	Move(escapespeed,elapsed);
}

void CChickenKI::AttackFarmer(CBauer* bauer,const float elapsed)
{
	const float d=Magnitude(bauer->pos-owner->pos);

	if (bauer->HasObject(IDDrescher))
	{
		if (d<maxattackrange-0.5f)
		{
			EscapeFrom(((CDrescher*)bauer->object)->pos,elapsed);
			return;
		}
	}
	if (d<minescaperange)
	{
		EscapeFrom(bauer->pos,elapsed);
		return;
	}
	if (d>maxattackrange)
	{
		MoveTo(bauer->pos,elapsed);
		return;
	}

	CStall* stall=FindNextStall(4.0f);
	if (stall)
	{
		EscapeFrom(stall->pos,elapsed);
		return;
	}

	const float a=RotateTo(bauer->pos,elapsed);

	if ((owner->lastthrow<=0.0f)&&(a<0.6f))
	{
		float sx=sqrtf(sqr(owner->pos.x-bauer->pos.x)+sqr(owner->pos.z-bauer->pos.z));
		float sy=bauer->pos.y-owner->pos.y-0.55f;
		float alpha=0.0f;
		float bestdistance=3.0f;

		float aktalpha=-0.8f;
		do
		{
			float y=GetWurfY(sx,aktalpha);

			if (abs(y-sy)<bestdistance)
			{
				bestdistance=abs(y-sy);
				alpha=aktalpha;
			}

			aktalpha+=0.1f;
		}while (aktalpha<g_PI/2.0f);


/*		float alpha1,alpha2;
		{	// Neue Formel benutzen
#define c1 (-(sx*sx*game->gravity)/(2.0f*ei_wurfspeed))
#define c2 (ei_offsety*sx/ei_wurfspeed)

			const float nenner=c2*c2-sx*sx;
			if (nenner==0.0f)return;

			float var;

			var=sqr((sx*sy-c1*sx)/(nenner))-(2*sy*c1-sqr(sy)-sqr(c1)+sqr(c2))/nenner;
			if (var<0.0f)return;
			var=sqrtf(var);

			float salpha=(sx*sy-c1*sx)/(nenner);

			alpha1=-atanf(salpha+var);
			alpha2=-atanf(salpha-var);


			alpha=alpha1;
		}

#ifdef _DEBUG
		CHAR c[100];
		sprintf(&c[0],"Alpha1 = %.2f, Alpha2 = %.2f",alpha1,alpha2);
		DebugOut(&c[0]);
#endif
*/

		UID id=0;
		owner->Throw(alpha,1,&id);
		if (id!=0)owner->SendNotify(11,0,1,&id,sizeof(id));
	}
}

void CChickenKI::Sit(CBauer* bauer,const float elapsed)
{
	CItem* item=FindNextItem(1,15.0f);
	if (item)
	{
		MoveTo(item->pos,elapsed);
		return;
	}

	CStall* stall=FindNextStall(stallentfernung);
	if (stall)
	{
		const float d=Magnitude(stall->pos-owner->pos);
		if (d>1.3f)
		{
			MoveTo(stall->pos,elapsed);
			return;
		}
		owner->produziere=TRUE;
		owner->SendFlags();
		return;
	}
	if (bauer)
	{
		EscapeFrom(bauer->pos,elapsed);
		return;
	}
	owner->produziere=TRUE;
	owner->SendFlags();
}

void CChickenKI::Walk(const float elapsed)
{
	if (owner->eier<25)
	{
		CItem* item=FindNextItem(1,5.0f);
		if (item)
		{
			MoveTo(item->pos,elapsed);
			return;
		}
	}
	walkingtime-=elapsed;
	if (walkingtime<0.0f)
	{
		walkingtime=1.5f+randf()*1.0f;
		walkingdir=rand()%3;
	}
	switch(walkingdir)
	{
	case 0:
		break;
	case 1:
		owner->ang.y+=elapsed*omega*0.3f;
		break;
	case 2:
		owner->ang.y-=elapsed*omega*0.3f;
		break;
	}
	Move(walkspeed,elapsed);
}



void CChickenKI::Think(const float elapsed)
{
	CBauer* bauer=FindNextBauer(bauerentfernung);

	owner->speed=D3DVECTOR(0,0,0);
	if (owner->produziere)
	{
		if ((owner->eier>=10)||((owner->eier>=6)&&(bauer)))
		{
			owner->produziere=FALSE;
			owner->SendFlags();
		}
		return;
	}

	if (bauer)
	{
		if (owner->eier>1)AttackFarmer(bauer,elapsed);
		else Sit(bauer,elapsed);
		return;
	}else{
		if (owner->eier<8)Sit(bauer,elapsed);
		else Walk(elapsed);
	}
}