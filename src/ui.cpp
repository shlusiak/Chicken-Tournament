/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "ui.h"
#include "objchain.h"
#include "game.h"
#include "player.h"
#include "config.h"
#include "text.h"



BOOLEAN CFPS::Init()
{

	for (BYTE i=0;i<sizeof(FPS)/sizeof(FPS[0]);i++)
	{
		FPS[i].zhl=FPS[i].temp=0;
		FPS[i].start=0;
		FPS[i].nums=0;
	}
	time=0.0f;
	full=FALSE;
	return TRUE;
}

void CFPS::DrawUI(LPDIRECT3DDEVICE7 lpDevice)
{
	static CHAR c[100];
	sprintf(&c[0],"%.2f fps",1.0f/FPS[2].zhl);
	game->lpD3DFont->DrawText(20,20,D3DRGB(1,1,1),&c[0],0);
	
	if (full)
	{
		sprintf(&c[0],"e=%.2f ms  /  r=%.2f ms",FPS[1].zhl*1000.0f,FPS[0].zhl*1000.0f);
		game->lpD3DFont->DrawText(20,40,D3DRGB(1,1,1),&c[0],0);
	}

	for (int i=3;i<5;i++) if (FPS[i].zhl>0.0f)
	{
		sprintf(&c[0],"FPS[%d]:  %.4f ms",i,FPS[i].zhl*1000.0f);
		game->lpD3DFont->DrawText(20,(float)60+(i-3)*20,D3DRGB(1,1,1),&c[0],0);
	}
}

void CFPS::Execute(float elapsed)
{
	if ((time+=elapsed)>0.5f)
	{
		time=0.0f;
		for (int i=0;i<sizeof(FPS)/sizeof(FPS[0]);i++)
		{
			FPS[i].zhl=FPS[i].temp/float(FPS[i].nums);
			FPS[i].temp=0;
			FPS[i].nums=0;
		}
	}
}

void CFPS::SetStartTime(const int index)
{
	QueryPerformanceCounter((LARGE_INTEGER*)&FPS[index].start);
}

void CFPS::SetEndTime(const int index)
{
	LONGLONG t;
	QueryPerformanceCounter((LARGE_INTEGER*)&t);
	FPS[index].temp+=float((t-FPS[index].start)*game->time_scale);
	FPS[index].nums++;
}




CTicker::CTicker()
:akttime(0)
{
	for (int i=0;i<MAXTICKER;i++)
	{
		data[i].text=NULL;
		data[i].times=0.0f;
	}
}

void CTicker::DrawUI(LPDIRECT3DDEVICE7 lpDevice)
{
	const float top=180.0f;
	const float left=20.0f;
	for (BYTE i=0;i<MAXTICKER;i++)if (data[i].text!=NULL)
	{
		game->lpD3DFont->DrawText(left,top+(i*19),data[i].color,data[i].text,0);
	}
}

void CTicker::Execute(float elapsed)
{
	akttime+=elapsed;
	if ((data[0].text!=NULL)&&(data[0].times<akttime-data[0].duration))
	{
		MoveUp();
	}
}

void CTicker::Add(const PCHAR s,const DWORD color,const float vduration)
{
	const maxtickerlength=80;
search:
#ifdef _DEBUG
	CHAR c[500];
	sprintf(&c[0],"Ticker << %s",s);
	DebugOut(&c[0]);
#endif
	for (BYTE i=0;i<MAXTICKER;i++)if (data[i].text==NULL)
	{
		data[i].text=(PCHAR)malloc(strlen(s)+2);
		strcpy(data[i].text,s);
		data[i].times=akttime;
		data[i].color=color;
		data[i].duration=vduration;

		// Text brav nach 80 Zeichen abschnibbeln und neue Zeile beginnen
		if (strlen(data[i].text)>maxtickerlength)
		{
			data[i].text[maxtickerlength]='\0';
			Add(s+maxtickerlength,color);
		}
		return;
	}
	MoveUp();
	goto search;
}

void CTicker::MoveUp()
{
	if (data[0].text!=NULL)free(data[0].text);
	for (BYTE i=0;i<MAXTICKER-1;i++)
	{
		data[i].text=data[i+1].text;
		data[i].times=data[i+1].times;
		data[i].color=data[i+1].color;
	}
	data[MAXTICKER-1].text=NULL;
}




CPlayerInfo::CPlayerInfoPlayer::CPlayerInfoPlayer()
:pid(0),dpid(0),ping(-1),name(NULL),IsServer(FALSE),updated(TRUE)
{
}

CPlayerInfo::CPlayerInfoPlayer::~CPlayerInfoPlayer()
{
	if (name!=NULL)free(name);
}

void CPlayerInfo::CPlayerInfoPlayer::Draw(HDC dc)
{
	int index=0;
	CObject* o=prev;
	while (o)
	{
		o=o->prev;
		index++;
	}

	const t2=70;

	SetTextColor(dc,RGB(0,0,0));
	if (dpid==LocalPlayerID)SetTextColor(dc,RGB(255,255,255));
	static CHAR c[200];

	// Name
	sprintf(&c[0],"%s%s",name,IsServer?T_SERVERCAT:"");
	TextOut(dc,30,t2+index*20,&c[0],strlen(&c[0]));
	
	// ID
	sprintf(&c[0],"%d",pid);
	TextOut(dc,170,t2+index*20,&c[0],strlen(&c[0]));

	// Kills
	CPlayer* p=(CPlayer*)game->chain->Find(pid);
	if (p!=NULL)
	{
		sprintf(&c[0],"%d",(int)floorf(p->hit));
		TextOut(dc,240,t2+index*20,&c[0],strlen(&c[0]));
	}

	// PING
	sprintf(&c[0],"%d",ping);
	TextOut(dc,320,t2+index*20,&c[0],strlen(&c[0]));

	// Type
	PCHAR s=NULL;
	if (GetType(pid)==IDBauer)s=T_PLAYERTYPES[0];
	if (GetType(pid)==IDChicken)s=T_PLAYERTYPES[1];
	if (s!=NULL)
	{
		sprintf(&c[0],"%s",s);
		TextOut(dc,400,t2+index*20,&c[0],strlen(&c[0]));
	}
}

void CPlayerInfo::CPlayerInfoPlayer::SetInfo(const DPID vid,const PCHAR vname,const UID vpid,const BOOLEAN vIsServer)
{
	dpid=vid;
	if (name)free(name);
	name=(PCHAR)malloc(strlen(vname)+2);
	strcpy(name,vname);
	IsServer=vIsServer;
	pid=id=vpid;
}




const float refreshperiod=2.0f;


CPlayerInfo::CPlayerInfo()
:time(0),visible(FALSE),send(0),received(0),width(512),height(512),Invalidated(FALSE),Bitmap(0)
{
	chain=new CObjChain;
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

	Redraw();
}

CPlayerInfo::~CPlayerInfo()
{
	if (Bitmap)DeleteObject(Bitmap);
	SaveRelease(surf);
	delete chain;
}

void CPlayerInfo::MyDraw()
{
	HDC dc,surfdc;
	if (FAILED(surf->GetDC(&dc)))return;
	if (Bitmap)
	{
		surfdc=dc;
		dc=CreateCompatibleDC(surfdc);
		SelectObject(dc,Bitmap);
	}

	SelectObject(dc,GetStockObject(DKGRAY_BRUSH));

	SetBkMode(dc,TRANSPARENT);

	SetTextColor(dc,RGB(0,192,255));
	Rectangle(dc,0,0,512-1,512-1);
	SelectObject(dc,GetStockObject(BLACK_PEN));
	Line(dc,30,60,512-30,60);

	TextOut(dc,20,20,T_PLAYER,strlen(T_PLAYER));

	TextOut(dc,30,40,T_NAME,strlen(T_NAME));
	TextOut(dc,170,40,T_ID,strlen(T_ID));
	TextOut(dc,240,40,T_KILLS,strlen(T_KILLS));
	TextOut(dc,320,40,T_PING,strlen(T_PING));
	TextOut(dc,400,40,T_TYPE,strlen(T_TYPE));


#ifdef _DEBUG
	SetTextColor(dc,RGB(255,0,128));
	Line(dc,30,512-150,512-30,512-150);
	TextOut(dc,30,512-130,"DEBUGINFO:",10);

	static CHAR c[100];
	sprintf(&c[0],"Nachrichten gesendet:  %d / sec.",int(float(send)/time));
	TextOut(dc,30,512-110,&c[0],strlen(&c[0]));
	sprintf(&c[0],"Nachrichten empfangen: %d / sec.",int(float(received)/time));
	TextOut(dc,30,512-90,&c[0],strlen(&c[0]));

	{	// Grafikinfos anzeigen
		D3DDEVINFO_TEXTUREMANAGER info;

		game->lpDevice->GetInfo(D3DDEVINFOID_TEXTUREMANAGER,&info,sizeof(info));

		sprintf(&c[0],"Texturen in Graka: %d KB",info.dwWorkingSetBytes/1024);
		TextOut(dc,30,512-60,&c[0],strlen(&c[0]));
	}

#endif


	chain->Draw(dc);

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

void CPlayerInfo::DrawUI(LPDIRECT3DDEVICE7 lpDevice)
{
	if (!visible)return;

	if (Invalidated)
	{
		MyDraw();
		Invalidated=FALSE;
	}


	float g=game->width-100.0f;
	if (game->height-100.0f<g)g=game->height-100.0f;
	const float left=(game->width-g)*0.5f;
	const float top=(game->height-g)*0.5f;

	const float faktor1=0.95f;
	const float faktor2=0.35f;
	D3DTLVERTEX v[4]=
	{
		D3DTLVERTEX(D3DVECTOR(left,top,0),0.5f,D3DRGBA(1,1,1,faktor1),0,0,0),
		D3DTLVERTEX(D3DVECTOR(left+g,top,0),0.5f,D3DRGBA(1,1,1,faktor1),0,1,0),
		D3DTLVERTEX(D3DVECTOR(left,top+g,0),0.5f,D3DRGBA(1,1,1,faktor2),0,0,1),
		D3DTLVERTEX(D3DVECTOR(left+g,top+g,0),0.5f,D3DRGBA(1,1,1,faktor2),0,1,1)
	};

	lpDevice->SetTexture(0,surf);

	lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,0);
	if (Config.alpha)
	{
		lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
		lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,1);
		lpDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
	}

	lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,v,4,0);

	if (Config.alpha)
	{
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,0);
	}
	lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,1);
}

void CPlayerInfo::Execute(float elapsed)
{
	if (!visible)return;

	time+=elapsed;
	if (time>refreshperiod)
	{
		Refresh();
#ifdef _DEBUG
		send=received=0;
#endif
		time=0.0f;
	}
}

BOOL FAR PASCAL EnumPlayersCallback2(DPID dpId,DWORD dwPlayerType,LPCDPNAME lpName,DWORD dwFlags,LPVOID lpContext)
{
	CObjChain* chain=(CObjChain*)lpContext;
	CPlayerInfo::CPlayerInfoPlayer* p;

	UID id=0;
	DWORD size=sizeof(UID);
	game->lpDirectPlay->GetPlayerData(dpId,&id,&size,DPGET_REMOTE);

	p=(CPlayerInfo::CPlayerInfoPlayer*)chain->Find(id);
	if (p==NULL)
	{
		p=new CPlayerInfo::CPlayerInfoPlayer();
		chain->Add(p);
		p->ispinging=FALSE;
	}

	p->SetInfo(dpId,lpName->lpszLongNameA,id,dwFlags==DPENUMPLAYERS_SERVERPLAYER);

	if (dpId==LocalPlayerID)
		p->SetPing(0);
	else 
	{
		if (p->ispinging==FALSE)
			Send(dpId,NM_PING,p->id,GetTickCount(),0,0,NULL,0,TRUE);
		p->ispinging=TRUE;
	}

	p->deleting=FALSE;

	return TRUE;
}

void CPlayerInfo::Refresh()
{
	CObject* o=chain->GetFirst();
	while (o)
	{
		o->Remove();
		o=o->next;
	}

	if (game->lpDirectPlay)game->lpDirectPlay->EnumPlayers(NULL,EnumPlayersCallback2,chain,DPENUMPLAYERS_ALL);

	chain->CleanChain();
	Redraw();
}

void CPlayerInfo::RefreshAll()
{
	CObject* o=chain->GetFirst();
	while (o)
	{
		o->Remove();
		o=o->next;
	}
	chain->CleanChain();

	Refresh();
}

void CPlayerInfo::ReceivePing(const PNETWORKDATA data)
{
	CPlayerInfo::CPlayerInfoPlayer* pip=
		(CPlayerInfo::CPlayerInfoPlayer*)chain->Find(data->id);

	if (pip==NULL)return;
	pip->SetPing(GetTickCount()-data->p1);
	pip->ispinging=FALSE;
	Redraw();
}

void CPlayerInfo::AddSendNum()
{
#ifdef _DEBUG
	send++;
#endif
}

void CPlayerInfo::AddReceivedNum()
{
#ifdef _DEBUG
	received++;
#endif
}

void CPlayerInfo::Invalidate()
{
	SaveRelease(surf);
}

void CPlayerInfo::Restore()
{
	width=height=512;
	surf=CreateTexture(game->lpDevice,&width,&height,0,FALSE,FALSE,FALSE);
}





const float maxtime=2.0f;



CBillboardText::~CBillboardText()
{
	SaveRelease(lpSurf);
}

BOOLEAN CBillboardText::Init()
{
	InitSurface();
	return TRUE;
}

void CBillboardText::InitSurface()
{
	if (lpSurf!=NULL)return;
	lpSurf=CreateTexture(game->lpDevice,&w,&h,0,withalpha,FALSE,FALSE);
}

void CBillboardText::SetText(const PCHAR text,const COLORREF color)
{
	if (lpSurf==NULL)
	{
		InitSurface();
		if (lpSurf==NULL)return;
	}
	time=0.0f;
	HDC dc;
	if (FAILED(lpSurf->GetDC(&dc)))
	{
		SaveRelease(lpSurf);
		return;
	}
	PatBlt(dc,-1,-1,w+1,h+1,BLACKNESS);

	if (strlen(text)!=0)
	{
		HFONT font=CreateFont(h,w/strlen(text),0,0,FW_BOLD,0,0,0,0,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,NONANTIALIASED_QUALITY,0,"COURIER NEW"),oldfont=(HFONT)SelectObject(dc,font);

		RECT r={0,0,w,h};

		SetBkMode(dc,TRANSPARENT);

		SetTextColor(dc,color);
		DrawText(dc,text,strlen(text),&r,DT_SINGLELINE|DT_CENTER|DT_VCENTER);

		SelectObject(dc,oldfont);
		DeleteObject(font);
	}

	length=strlen(text);
	lpSurf->ReleaseDC(dc);

	MakeTransparent(lpSurf,FALSE);
}

void CBillboardText::DrawUI(LPDIRECT3DDEVICE7 lpDevice)
{
	const maxx=game->width*3/5;
	const maxy=game->height*3/5;

	float faktor=sinf((time/maxtime)*g_PI);

	float w=maxx*faktor;
	float h=maxy*faktor;
	const float left=(game->width-w)*0.5f;
	const float top=(game->height-h)*0.5f;

	const DWORD color=D3DRGB(1,1,1);
	const float rhw=0.5f;
	D3DTLVERTEX v[4]=
	{
		D3DTLVERTEX(D3DVECTOR(left,top,0),rhw,color,0,0,0),
		D3DTLVERTEX(D3DVECTOR(left+w,top,0),rhw,color,0,1,0),
		D3DTLVERTEX(D3DVECTOR(left,top+h,0),rhw,color,0,0,1),
		D3DTLVERTEX(D3DVECTOR(left+w,top+h,0),rhw,color,0,1,1)
	};

	lpDevice->SetTexture(0,lpSurf);

	lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,0);
	if (Config.alpha)
	{
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
		lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
		lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);
	}

	lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,v,4,0);

	if (Config.alpha)
	{
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,0);
		lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,TRUE);
	}
}

void CBillboardText::Execute(float elapsed)
{
	time+=elapsed;
	if (time>maxtime)Remove();
}

void CBillboardText::Invalidate()
{
	SaveRelease(lpSurf);
}

void CBillboardText::Restore()
{
	InitSurface();
	SetText("",RGB(255,0,0));
}



const float maxfadetime=2.5f;
#define screenratio (1.0f/19.0f)

CFadeText::CFadeText()
:y(game->height*0.5f)
{ 
	withalpha=FALSE;
}

CFadeText::CFadeText(const float vy)
:y(game->height*0.5f+(vy*game->height*screenratio))
{ 
	withalpha=FALSE;
}

void CFadeText::Execute(float elapsed)
{
	if (length==0)return;
	time+=elapsed;
	if (time>maxfadetime)length=0;
}

void CFadeText::DrawUI(LPDIRECT3DDEVICE7 lpDevice)
{
	if (length==0)return;

	const float h=game->height*screenratio;
	const float w=length*h/2.15f;
	const float left=game->width*0.5f-w*0.5f;
	const float top=y-h*0.5f;

	float a=1.0f;
	if (time>maxfadetime*0.5f)
	{
		a=cosf((time-maxfadetime*0.5f)/(maxfadetime*0.5f)*(g_PI*0.5f));
	}

	DWORD color=D3DRGB(a,a,a);

	D3DTLVERTEX v[4]=
	{
		D3DTLVERTEX(D3DVECTOR(left,top,0),0.5f,color,0,0,0),
		D3DTLVERTEX(D3DVECTOR(left+w,top,0),0.5f,color,0,1,0),
		D3DTLVERTEX(D3DVECTOR(left,top+h,0),0.5f,color,0,0,1),
		D3DTLVERTEX(D3DVECTOR(left+w,top+h,0),0.5f,color,0,1,1)
	};

	lpDevice->SetTexture(0,lpSurf);

	lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,0);
	if (Config.alpha)
	{
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
		lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCCOLOR);
		lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCCOLOR);
	}

	lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,v,4,0);

	if (Config.alpha)
	{
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,0);
		lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,TRUE);
	}
}

