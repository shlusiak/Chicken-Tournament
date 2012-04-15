/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "wnd.h"
#include "serverwnd.h"
#include "game.h"
#include "config.h"
#include "text.h"
#include "resources/resource.h"
#include "player.h"
#include "bauer.h"
#include "chickenplayer.h"
#include "shellapi.h"



class COptionen:public CPage
{
private:
public:
	COptionen();

	virtual void Create();
	virtual void PerformCommand(int command,DWORD p1,int p2);
	virtual void Draw(HDC dc);
};

class CInfo:public CPage
{
private:
public:
	CInfo();

	virtual void Create();
	virtual void PerformCommand(int command,DWORD p1,int p2);
};

class CResolution:public CPage
{
private:
	CList* list;
	CCheckBox* fullscreen;
public:
	CResolution();

	virtual void Create();
	virtual void PerformCommand(int command,DWORD p1,int p2);
};





#define mitte (w/2)
const breite=160;
const hoehe=40;
#define links (mitte-(breite/2))
#define ypos(x) (150+(x)*(hoehe+10))


CMainPage::CMainPage()
:CPage(game->lpDD,M_MAINMENU,game->width,game->height,game->wnd)
{
//	HCURSOR c=0;
//	c=LoadAniCursor(GetDllInstance(),IDC_ANI1);
//	if (!c)c=LoadCursor(GetDllInstance(),MAKEINTRESOURCE((GetType(player->id)==IDChicken)?IDC_HUHN:IDC_BAUER));
//	SetClassLong(game->wnd,GCL_HCURSOR,(LONG)c);

	SetCursor();

	ShowCursor(TRUE);
	game->lpMouse->Unacquire();

	RECT r;
	GetClientRect(game->wnd,&r);
	ClientToScreen(game->wnd,&r);
	SetCursorPos(r.left+(r.right-r.left)/2,r.top+(r.bottom-r.top)/2);
}

CMainPage::~CMainPage()
{
	ShowCursor(FALSE);
	game->lpMouse->Acquire();
}

void CMainPage::SetCursor()
{
	lpCursor->Clear();
	lpCursor->AddFrame("cursor1.bmp");
	lpCursor->AddFrame("cursor2.bmp");
	lpCursor->AddFrame("cursor3.bmp");
	lpCursor->AddFrame("cursor4.bmp");
	lpCursor->AddFrame("cursor5.bmp");
}

void CMainPage::Create()
{
	AddChild(new CButton(links,ypos(0),breite,hoehe,this,M_CONTINUE,1));
	AddChild(new CButton(links,ypos(1),breite,hoehe,this,M_OPTIONS,3));
	AddChild(new CButton(links,ypos(2),breite,hoehe,this,M_SERVEROPTIONS,5));
	AddChild(new CButton(links,ypos(3),breite,hoehe,this,M_INFO,4));
	AddChild(new CButton(links,ypos(4),breite,hoehe,this,M_EXIT,2));



	AddChild(new CGroupBox(550,420,230,140,T_PLAYER,this));
	CList* list;
	AddChild(list=new CList(570,450,190,50,this,10));
	list->Add(T_PLAYERTYPES[0]);
	list->Add(T_PLAYERTYPES[1]);

	if (player->IsType(IDChicken))list->SetSel(1);else list->SetSel(0);

	list->Optimize();

	AddChild(new CButton(600,510,130,35,this,M_APPLY,11));
}

void CMainPage::PerformCommand(int command,DWORD p1,int p2)
{
	switch (command)
	{
	case 1:Close(1);
		return;
	case 2:DestroyWindow(game->wnd);
		Close(0);
		return;
	case 3:CreateChildWindow(new COptionen());
		return;
	case 4:CreateChildWindow(new CInfo());
		return;
	case 5:CreateChildWindow(new CServerPage());
		return;
	case 11:
		{
			CList* list=(CList*)FindChild(10);
			int i=list->GetSel()+1;

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
			Close(1);
			return;
		}
	}
}

void CMainPage::Draw(HDC dc)
{
	CPage::Draw(dc);

	SetTextColor(dc,RGB(255,255,255));
//	const PCHAR text="\"Sind die Hühner platt wie Teller,\n"
//		"Hat der Traktor kein Profil gehabt (sonst wären sie gerillt).\" (G.H.)";
	const PCHAR text=
		"\"Ist der Bauer noch nicht satt,\n"
		"fährt er sich ein Hühnchen platt.\"";

	RECT r={20,h-60,w-20,h};
	DrawText(dc,text,strlen(text),&r,DT_CENTER|DT_WORDBREAK);
}



COptionen::COptionen()
:CPage(game->lpDD,M_OPTIONS,game->width,game->height,game->wnd)
{ }

const reihe=240;

void COptionen::Create()
{
	const spalte=25;
	CWnd* obj=NULL;

	DWORD rs;
#define GetX(nr) (50+reihe*(nr))
#define GetY(nr) (105+spalte*(nr))

	AddChild(new CButton(links,h-hoehe-20,breite,hoehe,this,M_BACK,1));	
	AddChild(new CButton(GetX(0),h-hoehe-20,breite,hoehe,this,M_RESOLUTION,100));

	// Grafik
	AddChild(new CCheckBox(GetX(0),GetY(0),reihe,spalte,this,M_ALPHABLENDING,5,Config.alpha));
	game->lpDevice->GetRenderState(D3DRENDERSTATE_FOGENABLE,&rs);
	AddChild(new CCheckBox(GetX(0),GetY(1),reihe,spalte,this,M_FOG,7,BOOLEAN(rs)));
	game->lpDevice->GetRenderState(D3DRENDERSTATE_SPECULARENABLE,&rs);
	AddChild(new CCheckBox(GetX(0),GetY(2),reihe,spalte,this,M_SPECULAR,8,BOOLEAN(rs)));
	AddChild(new CCheckBox(GetX(0),GetY(3),reihe,spalte,this,M_VSYNC,11,Config.vsync));
	AddChild(obj=new CCheckBox(GetX(0),GetY(4),reihe,spalte,this,M_REFLECTIONS,30,Config.reflections));

	AddChild(new CCheckBox(GetX(0),GetY(5),reihe,spalte,this,M_CROSSHAIR,27,Config.Crosshair));
	AddChild(new CCheckBox(GetX(0),GetY(6),reihe,spalte,this,M_ANIMATIONS,28,Config.Animations));
	AddChild(new CCheckBox(GetX(0),GetY(7),reihe,spalte,this,M_GRASS,29,Config.Gras));
	AddChild(new CCheckBox(GetX(0),GetY(8),reihe,spalte,this,M_SHOWPLAYERNAMES,31,Config.ShowPlayernames));
 
	AddChild(new CCheckBox(GetX(0),GetY(10),reihe,spalte,this,M_BLOOD,2,Config.blut));
	AddChild(new CCheckBox(GetX(0),GetY(11),reihe,spalte,this,M_FEDERN,10,Config.federn));
	AddChild(new CCheckBox(GetX(0),GetY(12),reihe,spalte,this,M_GIBS,12,Config.gibs));
	AddChild(new CCheckBox(GetX(0)+20,GetY(13),reihe,spalte,this,M_FIRE,85,Config.fire));
	if (!Config.gibs)FindChild(85)->canfocus=FALSE;

	AddChild(new CStaticText(GetX(0),GetY(14),reihe,spalte,this,M_BLOODAMOUNT));
	AddChild(new CSlider(GetX(0),GetY(15),reihe,35,this,0.1f,10.0f,game->gorelevel,0.1f,20,TRUE,FALSE));

	// Sounds
	int i;
	AddChild(new CCheckBox(GetX(1),GetY(0),reihe,spalte,this,M_SOUND,4,Config.soundon));
	if (HasUTSounds())
	{
		AddChild(new CCheckBox(GetX(1)+20,GetY(1),reihe,spalte,this,M_UNREALSOUND,13,Config.unrealext));
		i=2;
	}else i=1;
	AddChild(new CCheckBox(GetX(1)+20,GetY(i),reihe,spalte,this,M_GACKERN,23,Config.gackern));
	if (!Config.soundon)
	{
		if (HasUTSounds())FindChild(13)->canfocus=FALSE;
		FindChild(23)->canfocus=FALSE;
	}
	AddChild(new CCheckBox(GetX(1),GetY(i+1),reihe,spalte,this,M_MUSIC,3,game->lpMusic->IsMusic()));

	// Sonstiges
	AddChild(new CCheckBox(GetX(2),GetY(0),reihe,spalte,this,M_REVERSEMOUSE,6,Config.reversemouse));
	AddChild(new CCheckBox(GetX(2),GetY(1),reihe,spalte,this,M_REVERSEFLIGHT,15,Config.reverseflight));
	AddChild(new CCheckBox(GetX(2),GetY(2),reihe,spalte,this,M_SMOOTHMOUSE,9,Config.smoothmouse));

	AddChild(new CStaticText(GetX(2),GetY(3),reihe,spalte,this,M_MOUSESENSITIVITY));
	AddChild(new CSlider(GetX(2),GetY(4),reihe,35,this,10.0f,50.0f,float(Config.sensitivity),5.0f,21,TRUE,FALSE));

	AddChild(new CCheckBox(GetX(2),GetY(6),reihe,spalte,this,M_GOLFERGEBNISSE,14,Config.golfergebnisse));
}

void COptionen::Draw(HDC dc)
{
	CPage::Draw(dc);

	HPEN pen=CreatePen(PS_SOLID,1,RGB(0,128,192)),oldpen=HPEN(SelectObject(dc,pen));

	Line(dc,GetX(0),96,GetX(3)-10,96);

	SelectObject(dc,oldpen);
	DeleteObject(pen);

	SetTextColor(dc,RGB(96,208,255));
	SetBkMode(dc,TRANSPARENT);
	TextOut(dc,GetX(0),75,M_GRAFIK,strlen(M_GRAFIK));
	TextOut(dc,GetX(1),75,M_SOUND,strlen(M_SOUND));
	TextOut(dc,GetX(2),75,M_SONSTIGES,strlen(M_SONSTIGES));
}

void COptionen::PerformCommand(int command,DWORD p1,int p2)
{
	switch (command)
	{
	case 1:Close();
		break;
	case 100:
		CreateChildWindow(new CResolution());
		break;

	case 2:Config.blut=p2;
		break;
	case 3:Config.Musik=game->lpMusic->ToggleMusic();
		break;
	case 4:game->ToggleSound();
		if (HasUTSounds())FindChild(13)->canfocus=p2;
		if (HasUTSounds())FindChild(13)->Invalidate();
		FindChild(23)->canfocus=p2;
		FindChild(23)->Invalidate();
		break;
	case 5:Config.alpha=p2;
		break;
	case 6:Config.reversemouse=p2;
		break;
	case 7:game->lpDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE,p2);
		break;
	case 8:game->lpDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE,p2);
		break;
	case 9:Config.smoothmouse=p2;
		break;
	case 10:Config.federn=p2;
		break;
	case 11:Config.vsync=p2;
		break;
	case 30:Config.reflections=p2;
		break;
	case 12:Config.gibs=p2;
		FindChild(85)->canfocus=p2;
		FindChild(85)->Invalidate();

		break;
	case 85:Config.fire=p2;
		break;
	case 13:Config.unrealext=p2;
		break;
	case 14:Config.golfergebnisse=p2;
		break;
	case 15:Config.reverseflight=p2;
		break;

	case 20:game->gorelevel=LongToFloat(p2);
		break;
	case 21:Config.sensitivity=int(LongToFloat(p2));
		break;
	case 23:Config.gackern=p2;
		break;

	case 27:Config.Crosshair=p2;
		break;
	case 28:Config.Animations=p2;
		break;
	case 29:Config.Gras=p2;
		break;
	case 31:Config.ShowPlayernames=p2;
		break;
	}
}





HRESULT WINAPI MyDisplayModesEnumCallback( LPDDSURFACEDESC2 sd,
                                          VOID* pParentInfo )
{
	int bpp=sd->ddpfPixelFormat.dwRGBBitCount;
	int width=sd->dwWidth,height=sd->dwHeight;


	if (bpp>8)
	{
		CHAR c[100];
		sprintf(&c[0],"%dx%dx%d",width,height,bpp);
		int i=((CList*)pParentInfo)->Add(&c[0]);

		if ((width==Config.resx)&&(height==Config.resy)&&(Config.bpp==bpp))
		{
			((CList*)pParentInfo)->SetSel(i);
		}
	}

    return DDENUMRET_OK;
}


CResolution::CResolution()
:CPage(game->lpDD,M_RESOLUTION,game->width,game->height,game->wnd)
{
}

void CResolution::Create()
{
	AddChild(new CButton(mitte-5-breite,h-hoehe-20,breite,hoehe,this,M_BACK,1));
	AddChild(new CButton(mitte+5,h-hoehe-20,breite,hoehe,this,M_APPLY,4));
	AddChild(fullscreen=new CCheckBox(mitte-200,480,130,25,this,M_FULLSCREEN,2,Config.fullscreen));

	AddChild(list=new CList(mitte-200,100,400,370,this,3));

	game->lpDD->EnumDisplayModes(0,NULL,(LPVOID)list,MyDisplayModesEnumCallback);

	list->Optimize();
}

void CResolution::PerformCommand(int command,DWORD p1,int p2)
{
	switch(command)
	{
	case 1:
		Close();
		break;
	case 4:	// Alle Einstellungen übernehmen
		{
			PCHAR w;
			{
				PCHAR tmp=list->GetString(list->GetSel());
				w=(PCHAR)malloc(strlen(tmp)+1);
				strcpy(w,tmp);
			}
			BOOLEAN fs=fullscreen->GetCheck();
			
			PCHAR h=w;
			while (*h!='x')h++;
			*h='\0';
			h++;

			PCHAR bpp=h;
			while (*bpp!='x')bpp++;
			*bpp='\0';
			bpp++;

			if ((Config.fullscreen==FALSE)&&(fs==FALSE))
			{
				Config.resx=atoi(w);
				Config.resy=atoi(h);
				Config.bpp=atoi(bpp);
			}else{
				game->ChangeResolution(atoi(w),atoi(h),atoi(bpp),fs);
			}
			
			free(w);
		}
		break;
	}
}






class CInfoControl:public CWnd
{
private:
	float pos;
	BOOLEAN buttondown;
	int my;
public:
	CInfoControl(int vx,int vy,int vw,int vh,CWnd* vowner);
	virtual void Draw(HDC dc);
	virtual void Execute(float elapsed);
	virtual BOOLEAN PerformMessage(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
};


CInfoControl::CInfoControl(int vx,int vy,int vw,int vh,CWnd* vowner)
:CWnd(NULL,vx,vy,vw,vh,vowner)
{
	canfocus=FALSE;
	pos=-float(h);
}

void CInfoControl::Execute(float elapsed)
{
	if (!buttondown)
	{
		const float speed=30.0f;
		pos+=elapsed*speed;
		Invalidate();
	}
}

void CInfoControl::Draw(HDC dc)
{
	CWnd::Draw(dc);

	SetBkMode(dc,TRANSPARENT);

#define th 20
#define tw 8
#define NORMAL TRUE,0,0
#define LAST TRUE,1,0
#define HEAD FALSE,0,1

	const PCHAR shl="Sascha Hlusiak";

	struct
	{
		PCHAR text;
		BOOLEAN center;
		int next;
		int font;
	}text[]=
	{
		"CT",TRUE,0,1,
		"(Chicken Tournament)",TRUE,0,1,
		"Version 1.2",TRUE,0,0,
		"("__DATE__", "__TIME__")",TRUE,5,0,

		M_HAUPTIDEE,HEAD,
		"Andreas Kempe",NORMAL,
		"Johannes Kruse",LAST,

		M_VORSCHLAEGE,HEAD,
		"Steffen Malar",NORMAL,
		"Matthias Henche",NORMAL,
		"Matthias Heming",NORMAL,
		"Benjamin Lang",NORMAL,
		"Johannes Kruse",NORMAL,
		"Christoph Kruse",NORMAL,
		"Gerhard Hlusiak",NORMAL,
		"Sebastian Schulte",NORMAL,
		"Andreas Kempe",NORMAL,
		"Jens Herbst",NORMAL,
		"Stefan Hedtfeld",NORMAL,
		"Björn Ribniger",NORMAL,
		shl,LAST,

		M_TEXTUREN,HEAD,
		shl,NORMAL,
		"Jens Herbst",NORMAL,
		"Benjamin Lang",LAST,

		M_GRAFIKER,HEAD,
		shl,LAST,
//		"Benjamin Lang",LAST,
//		"GuitarmanESP",LAST,

		M_PROGRAMMIERER,HEAD,
		shl,LAST,

		M_SPRITES,HEAD,
		shl,LAST,

		M_MODELLE,HEAD,
		shl,LAST,

		M_SOUNDS,HEAD,
		shl,LAST,

		M_MUSIK,HEAD,
		shl,LAST,

		M_NETWORK,HEAD,
		shl,LAST,

		M_AI,HEAD,
		shl,LAST,


		M_BETATESTER,HEAD,
		"Steffen Malar",NORMAL,
		"Matthias Henche",NORMAL,
		"Matthias Heming",NORMAL,
		"Michael Heming",NORMAL,
		"Benjamin Lang",NORMAL,
		"Jens Herbst",NORMAL,
		"Sebastian Schulte",NORMAL,
		"Andreas Kempe",NORMAL,
		"Christoph Kruse",NORMAL,
		"Frank Elseberg",NORMAL,
		"Christoph Klee",LAST,

		M_THANKS,HEAD,
		"Dire Straits",NORMAL,
		M_DIRESTRAITS,TRUE,1,0,

		"Wolfgang Dick",TRUE,3,0,

		M_ENDE,TRUE,4,1,
		M_EMAILS,TRUE,1,0
	};

	const MAX_TEXT=sizeof(text)/sizeof(text[0]);

	const HFONT font1=CreateFont(th,tw,0,0,FW_BLACK,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,NONANTIALIASED_QUALITY,FF_DONTCARE,"ARIAL");
	const HFONT font2=CreateFont(th,tw,0,0,FW_ULTRALIGHT,TRUE,FALSE,FALSE,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,NONANTIALIASED_QUALITY,FF_DONTCARE,"ARIAL");
	const HFONT oldfont=(HFONT)SelectObject(dc,font1);

#define bh h

	const HRGN rgn=CreateRoundRectRgn(0,0,w,bh,40,25),orgn=(HRGN)SelectObject(dc,rgn);
	FillRgn(dc,rgn,(HBRUSH)GetStockObject(BLACK_BRUSH));


	RECT r;
	int y=20-int(pos);
	// Draw shadow
	SetTextColor(dc,RGB(64,64,64));
	r.left=12;
	r.right=w-18;

	for (int i=0;i<MAX_TEXT;i++)
	{
		r.top=y+4+int((y+i)*0.015);
		r.bottom=r.top+th;

		SelectObject(dc,(text[i].font==1)?font1:font2);
		DrawText(dc,text[i].text,strlen(text[i].text),&r,(text[i].center?DT_CENTER:0));
		y+=th+(th*text[i].next);
	}

	// Real text
	SetTextColor(dc,RGB(255,255,255));
	y=20-int(pos);

	r.left=20;
	r.right=w-20;

	for (i=0;i<MAX_TEXT;i++)
	{
		r.top=y;
		r.bottom=r.top+th;

		SelectObject(dc,(text[i].font==1)?font1:font2);
		DrawText(dc,text[i].text,strlen(text[i].text),&r,(text[i].center?DT_CENTER:0));
		y+=th+(th*text[i].next);
	}
	FrameRgn(dc,rgn,(HBRUSH)GetStockObject(LTGRAY_BRUSH),1,1);
	SelectObject(dc,oldfont);
	SelectObject(dc,orgn);
	DeleteObject(rgn);
	DeleteObject(font1);
	DeleteObject(font2);
	if (y<0)pos=-float(h);
}

BOOLEAN CInfoControl::PerformMessage(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	if (uMsg==WM_LBUTTONDOWN)
	{
		POINT p={LOWORD(lParam),HIWORD(lParam)};
		ScreenToClient(&p);
		if (!IsInside(&p))return FALSE;
		buttondown=TRUE;
		my=p.y;

		SetCapture(hWnd);
		return TRUE;
	}
	if (uMsg==WM_LBUTTONUP)
	{
		buttondown=FALSE;
		ReleaseCapture();
		return TRUE;
	}
	if (uMsg==WM_MOUSEMOVE)
	{
		if (!buttondown)return FALSE;
		POINT p={(short)LOWORD(lParam),(short)HIWORD(lParam)};
		ScreenToClient(&p);

		pos-=p.y-my;

		my=p.y;

		Invalidate();
		return TRUE;
	}

	return FALSE;
}






CInfo::CInfo()
:CPage(game->lpDD,M_INFOABOUT,game->width,game->height,game->wnd)
{
}

void CInfo::Create()
{
	const breite=325;
	AddChild(new CButton(links,h-hoehe-20,breite,hoehe,this,M_BACK,1));
	AddChild(new CButton(mitte-250/2,h-3*hoehe-10,250,30,this,"email: mail@saschahlusiak.de",2));
	AddChild(new CButton(mitte-250/2,h-2*hoehe-17,250,30,this,"www.saschahlusiak.de",3));
	AddChild(new CInfoControl(w/2-breite/2,80,breite,h-80-135,this));
}

void CInfo::PerformCommand(int command,DWORD p1,int p2)
{
	switch(command)
	{
	case 1:Close();
		break;
	case 2:
		CloseWindow(game->wnd);
		ShellExecute(game->wnd,NULL,"mailto:Spam84@gmx.de?subject=Chicken Tournament",NULL,NULL,SW_SHOW);
		break;
	case 3:
		CloseWindow(game->wnd);
		ShellExecute(game->wnd,NULL,"http://www.sascha.hlusiak.de",NULL,NULL,SW_SHOW);
		break;
	}
}

