/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "radar.h"
#include "player.h"
#include "game.h"
#include "config.h"
#include "chicken.h"
#include "bauer.h"
#include "tree.h"
#include "gartenkralle.h"
#include "golfer.h"
#include "drescher.h"
#include "plasmacannon.h"







CRadar::CRadar(CPlayer* vparent)
:size(0),parent(vparent),lpSurface(NULL),Enabled(TRUE),lastupdate(0)
{
}

CRadar::~CRadar()
{
	SaveRelease(lpSurface);
}

void CRadar::Update()
{
	HDC dc;
	if (SUCCEEDED(lpSurface->GetDC(&dc)))
	{
		HBRUSH oldbrush;
		{	// Löschen / Auf Farbe setzen
			PatBlt(dc,0,0,size,size,BLACKNESS);
			SelectObject(dc,GetStockObject(WHITE_PEN));
			const HBRUSH green=CreateSolidBrush(RGB(0,64,0));
			oldbrush=(HBRUSH)SelectObject(dc,green);
			Ellipse(dc,0,0,size,size);
			SelectObject(dc,oldbrush);
			DeleteObject(green);
		}

		const float sx=parent->pos.x,sy=parent->pos.z;
		const float mx=size/2.0f,my=size/2.0f;

		const float InvRation=1.0f/((0.4f)*(256.0f/float(size)));

		D3DMATRIX m;
		if (parent->IsType(IDChicken))
			D3DUtil_SetRotateYMatrix(m,-parent->ang.y);
		else D3DUtil_SetRotateYMatrix(m,parent->ang.y);
	
		D3DVECTOR v;
		const HRGN rgn=CreateEllipticRgn(1,1,size-1,size-1);
		SelectClipRgn(dc,rgn);

		{	// Begrenzung rendern
			D3DVECTOR b[4]={
				D3DVECTOR(world->minx(),0,world->minz())-parent->pos,
				D3DVECTOR(world->maxx(),0,world->minz())-parent->pos,
				D3DVECTOR(world->maxx(),0,world->maxz())-parent->pos,
				D3DVECTOR(world->minx(),0,world->maxz())-parent->pos};

			POINT p[4];
			for (int i=0;i<4;i++)
			{
				D3DMath_VectorMatrixMultiply(b[i],b[i],m);

				b[i].x=b[i].x*InvRation+mx;
				b[i].z=-b[i].z*InvRation+my;
				p[i].x=(int)b[i].x;
				p[i].y=(int)b[i].z;
			}

			SelectObject(dc,GetStockObject(WHITE_PEN));
			const HBRUSH green=CreateSolidBrush(RGB(0,128,0));
			oldbrush=(HBRUSH)SelectObject(dc,green);

			Polygon(dc,&p[0],4);
			SelectObject(dc,oldbrush);
			DeleteObject(green);
		}

//	Grid rendern

		{
			D3DVECTOR v2;
			const HPEN pen1=CreatePen(PS_SOLID,1,RGB(0,192,0)),pen2=CreatePen(PS_SOLID,1,RGB(128,128,0)),pen3=CreatePen(PS_SOLID,1,RGB(0,128,128)),oldpen=(HPEN)SelectObject(dc,pen1);

#define Grid 20

			int ox=0,oy=0,oz=0,x,z;
			int num=int((size)/(Grid*InvRation))/2;

			if (parent->pos.x>0)
				while (ox<parent->pos.x)ox+=Grid;

			if (parent->pos.x<0)
				while (ox>parent->pos.x)ox-=Grid;

			if (parent->pos.z>0)
				while (oz<parent->pos.z)oz+=Grid;

			if (parent->pos.z<0)
				while (oz>parent->pos.z)oz-=Grid;


			if (TRUE)
			{	// x/z
				SelectObject(dc,pen1);
				for (x=-num-1;x<=num+1;x++)	// x-Achse
				{
					v.x=float(x*(Grid*InvRation))-(parent->pos.x*InvRation)+(ox*InvRation);
					v.y=0;
					v.z=size*0.5f;

					v2.x=float(x*(Grid*InvRation))-(parent->pos.x*InvRation)+(ox*InvRation);
					v2.y=0;
					v2.z=-int(size)*0.5f;
					D3DMath_VectorMatrixMultiply(v,v,m);
					D3DMath_VectorMatrixMultiply(v2,v2,m);
					MoveToEx(dc,int(mx+(v.x)),int(my+(-v.z)),NULL);
					LineTo(dc,int(mx+(v2.x)),int(my+(-v2.z)));
				}
	
				for (z=-num-1;z<=num+1;z++)	// z-Achse
				{
					v.x=size*0.5f;
					v.y=0;
					v.z=float(z*(Grid*InvRation))-(parent->pos.z*InvRation)+(oz*InvRation);
		
					v2.x=-int(size)*0.5f;
					v2.y=0;
					v2.z=float(z*(Grid*InvRation))-(parent->pos.z*InvRation)+(oz*InvRation);
					D3DMath_VectorMatrixMultiply(v,v,m);
					D3DMath_VectorMatrixMultiply(v2,v2,m);
					MoveToEx(dc,int(mx+(v.x)),int(my+(-v.z)),NULL);
					LineTo(dc,int(mx+(v2.x)),int(my+(-v2.z)));
				}
			}
	
	
	
			SelectObject(dc,oldpen);
			::DeleteObject(pen1);
			::DeleteObject(pen2);
			::DeleteObject(pen3);
		}

//	---


		COLORREF c;
		float s,x,y;
		D3DVECTOR p;
		HBRUSH brush;

		// Objekte rendern

		CObject* akt=game->chain->GetFirst();
		while (akt)
		{
			c=RGB(255,0,0);
			s=3;
			switch(GetType(akt->id))
			{
			case IDChicken:
				if (((CChicken*)akt)->dead!=0.0f)goto weiter;
				p=((CChicken*)akt)->pos;
				c=RGB(255,255,255);
				s=3;
				break;
			case IDBauer:
				if (((CBauer*)akt)->dead!=0.0f)goto weiter;
				if (((CBauer*)akt)->HasObject(IDDrescher))goto weiter;
				p=((CBauer*)akt)->pos;
				c=RGB(192,128,0);
				s=5;
				break;
			case IDStall:
				p=((CStall*)akt)->pos;
				c=RGB(214,160,32);
				s=7;
				break;
			case IDTree:
				p=((CTree*)akt)->pos;
				c=RGB(0,230,0);
				s=4;
				break;
			case IDGartenkralle:
				p=((CGartenkralle*)akt)->pos;
				c=RGB(255,214,0);
				s=2;
				break;
			case IDDrescher:
				p=((CDrescher*)akt)->pos;
				c=RGB(255,0,0);
				s=7;
				break;
			case IDGolfer:
				p=((CGolfer*)akt)->pos;
				c=RGB(192,192,192);
				s=2;
				break;
			case IDPlasmaCannon:
				p=((CPlasmaCannon*)akt)->pos;
				c=RGB(255,255,0);
				s=3;
				break;



			default:goto weiter;
			}
	
			p=p-parent->pos;
			D3DMath_VectorMatrixMultiply(p,p,m);
			x=p.x*InvRation;
			y=-p.z*InvRation;

			if ((x)*(x)+(y)*(y)<((size*0.5f)*(size*0.5f))-(4.0f*4.0f*4.0f*4.0f))
			{
				x+=mx;
				y+=my;

				SelectObject(dc,brush=CreateSolidBrush(c));

				SelectObject(dc,GetStockObject(NULL_PEN));
				Ellipse(dc,int(x-s),int(y-s),int(x+s),int(y+s));
	
				SelectObject(dc,oldbrush);
				::DeleteObject(brush);
			}	


weiter:
			akt=akt->next;
		}

		SelectObject(dc,GetStockObject(WHITE_PEN));
		Line(dc,size/2,(size*10)/18,size/2,(size*8)/18);
		Line(dc,(size*10)/18,size/2,(size*8)/18,size/2);

		SelectClipRgn(dc,NULL);
		::DeleteObject(rgn);

		lpSurface->ReleaseDC(dc);
	}

	MakeTransparent(lpSurface,FALSE);
}

void CRadar::UpdateSurface()
{
	SaveRelease(lpSurface);
	size=256;
	lpSurface=CreateTexture(game->lpDevice,&size,&size,0,TRUE,FALSE,FALSE);
}

void CRadar::Execute(float elapsed)
{
	if (!Enabled)return;

	if (lpSurface==NULL)
	{
		UpdateSurface();
		Update();
		return;
	}

	lastupdate+=elapsed;
	if (lastupdate>0.08f)
	{
		lastupdate=0.0f;

		Update();
	}
}

void CRadar::DrawUI(LPDIRECT3DDEVICE7 lpDevice)
{
	if (!Enabled)return;
	if ((lpSurface==NULL)||(lpSurface->IsLost()))
	{
		UpdateSurface();
		Update();
	}

	const float w=130.0f*game->width/640.0f;
	const float h=130.0f*game->width/640.0f;

	const float x=game->width-20-w;
	const float y=game->height-20-h;
	const int c=D3DRGBA(1,1,1,0.45f);
	const float r=0.9f;
	D3DTLVERTEX v[4]={
		D3DTLVERTEX(D3DVECTOR(x,y,0),r,c,0,0,0),
		D3DTLVERTEX(D3DVECTOR(x+w,y,0),r,c,0,1,0),
		D3DTLVERTEX(D3DVECTOR(x,y+h,0),r,c,0,0,1),
		D3DTLVERTEX(D3DVECTOR(x+w,y+h,0),r,c,0,1,1)
	};

	if (Config.alpha)
	{
//		lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,FALSE);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
		lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
		lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);
		lpDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);

		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,TRUE);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHAREF,8);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHAFUNC,D3DCMP_GREATEREQUAL);
	}

	lpDevice->SetTexture(0,lpSurface);

	lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,v,4,0);

	lpDevice->SetTexture(0,NULL);

	lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,0);
	lpDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,0);
//	lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,TRUE);
}

void CRadar::Invalidate()
{
	SaveRelease(lpSurface);
}

