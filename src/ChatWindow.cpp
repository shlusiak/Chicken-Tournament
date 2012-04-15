/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "game.h"
#include "chatwindow.h"
#include "config.h"



CChatWindow::~CChatWindow()
{
	if (lpText)free(lpText);
	SaveRelease(lpSurface);
}

void CChatWindow::Toggle()
{
	if (game->lpDirectPlay==NULL)return;
	visible=!visible;
	if (visible)
	{
		MyInvalidate();
		lpText=PCHAR(malloc(500));
		lpText[0]='\0';
	}else{
		free(lpText);
		lpText=NULL;
	}
}

void CChatWindow::WMCHAR(const WORD wParam)
{
	const PCHAR allowed="äöüßÄÖÜ^";
	if (strlen(lpText)>490)return;
	if ((!isprint(wParam))&&(strchr(allowed,wParam)==NULL))return;

	lpText[strlen(lpText)+1]='\0';
	lpText[strlen(lpText)]=CHAR(wParam);
	MyInvalidate();
}

void CChatWindow::WMKEYDOWN(const DWORD Key)
{
	switch(Key)
	{
	case VK_ESCAPE:
		Toggle();
		break;

	case VK_BACK:
		if (strlen(lpText)>0)lpText[strlen(lpText)-1]='\0';
		MyInvalidate();
		break;

	case VK_RETURN:
		{
			if (strlen(lpText)<1)
			{
				Toggle();
				return;
			}
			Send(DPID_ALLPLAYERS,NM_CHAT,0,strlen(lpText),0,0,lpText,strlen(lpText)+1,TRUE);
		
			CHAR c[600];
			LPVOID v;
			DWORD s=0;
			game->lpDirectPlay->GetPlayerName(LocalPlayerID,NULL,&s);
			v=malloc(s);
			game->lpDirectPlay->GetPlayerName(LocalPlayerID,v,&s);

			sprintf(&c[0],"%s: %s",((DPNAME*)v)->lpszLongNameA,lpText);
			free(v);

			DebugOut(&c[0]);
			game->ticker->Add(&c[0],CHATCOLOR);

			Toggle();
		}
		break;
	}
}

void CChatWindow::UpdateSurface()
{
	SaveRelease(lpSurface);

	width=256;
	height=32;

	lpSurface=CreateTexture(game->lpDevice,&width,&height,0,FALSE,FALSE,FALSE);
}

void CChatWindow::DrawUI(LPDIRECT3DDEVICE7 lpDevice)
{
	if (visibility<=0.0f)return;

	if (lpSurface==NULL)UpdateSurface();
	if (lpSurface==NULL)return;
	if (mustupdate)
	{
		HDC dc;
		lpSurface->GetDC(&dc);
		MyDraw(dc);
		lpSurface->ReleaseDC(dc);
		mustupdate=FALSE;
	}

	const float w=game->width/2.0f;
	const float h=game->height*4.0f/3.0f/2.0f/8.0f;

	const float left=(game->width-w)/2.0f;
	const float top=(game->height-h*1.5f);

	const D3DTLVERTEX v[4]=
	{
		D3DTLVERTEX(D3DVECTOR(left,top,0),0.5f,D3DRGBA(1,1,1,visibility),0,0,0),
		D3DTLVERTEX(D3DVECTOR(left+w,top,0),0.5f,D3DRGBA(1,1,1,visibility),0,1,0),
		D3DTLVERTEX(D3DVECTOR(left,top+h,0),0.5f,D3DRGBA(1,1,1,visibility),0,0,1),
		D3DTLVERTEX(D3DVECTOR(left+w,top+h,0),0.5f,D3DRGBA(1,1,1,visibility),0,1,1)
	};

	lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,0);

	lpDevice->SetTexture(0,lpSurface);

	if (Config.alpha)
	{
		lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
		lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
		lpDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
	}

	lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,(void*)&v,4,0);

	if (Config.alpha)
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,0);
	
	lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,1);
}

void CChatWindow::MyDraw(const HDC dc)const
{
	const HBRUSH brush=CreateSolidBrush(RGB(64,64,64)),oldbrush=(HBRUSH)SelectObject(dc,brush);
	SelectObject(dc,GetStockObject(BLACK_PEN));

	Rectangle(dc,0,0,256,32);

	SelectObject(dc,oldbrush);
	DeleteObject(brush);

	SetBkMode(dc,TRANSPARENT);
	const int fh=20;
	const int fw=7;

	const HFONT font=CreateFont(fh,fw,0,0,400,0,0,0,0,0,0,NONANTIALIASED_QUALITY,0,"Courier New"),
		oldfont=(HFONT)SelectObject(dc,font);

	SetTextColor(dc,RGB(255,255,255));
	RECT r={5,0,250,32};
	SIZE s;
	DrawText(dc,"Chat:",5,&r,DT_VCENTER|DT_SINGLELINE);

	GetTextExtentPoint(dc,"Chat:",5,&s);
	r.left+=s.cx+2;
	r.top+=(32-s.cy)/2-2;
	r.bottom-=(32-s.cy)/2-2;
	r.right-=2;
	SelectObject(dc,GetStockObject(BLACK_BRUSH));
	SelectObject(dc,GetStockObject(WHITE_PEN));

	Rectangle(dc,r.left,r.top,r.right,r.bottom);

	r.left+=2;
	r.right-=2;

	if (lpText!=NULL)DrawText(dc,lpText,strlen(lpText),&r,DT_RIGHT);

	SelectObject(dc,oldfont);
	DeleteObject(font);
}

void CChatWindow::Execute(float elapsed)
{
	const float maxvisibility=0.6f;
	const float speed=3.5f;

	if ((visible)&&(visibility<maxvisibility))
	{
		visibility+=(elapsed*speed);
		if (visibility>maxvisibility)visibility=maxvisibility;
	}
	if ((!visible)&&(visibility>0))
	{
		visibility-=(elapsed*speed);
		if (visibility<0.0f)
		{
			visibility=0.0f;
			SaveRelease(lpSurface);
		}
	}
}
