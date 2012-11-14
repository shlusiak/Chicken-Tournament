/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _UI_INCLUDED_
#define _UI_INCLUDED_

#include "objects.h"



const int MAXTICKER=10;



class CFPS:public CObject
{
private:
	struct TFPS
	{
		float zhl,temp;
		LONGLONG start;
		int nums;
	}FPS[5];
	float time;
public:
	BOOLEAN full;

	BOOLEAN Init();
	virtual void DrawUI(LPDIRECT3DDEVICE7 lpDevice);

	void SetStartTime(const int index);
	void SetEndTime(const int index);

	virtual void Execute(float elapsed);
};


class CTicker:public CObject
{
private:
	float akttime;
	struct DATA
	{
		PCHAR text;
		float times,duration;
		DWORD color;
	}data[MAXTICKER];

	void MoveUp();
public:
	CTicker();
	virtual void DrawUI(LPDIRECT3DDEVICE7 lpDevice);
	virtual void Execute(float elapsed);
	void Add(const PCHAR s,const DWORD color=D3DRGB(1,1,1),const float vduration=15.0f);
};


class CPlayerInfo:public CObject
{
private:
	HBITMAP Bitmap;
	float time;
	CObjChain* chain;
	LPDIRECTDRAWSURFACE7 surf;
	DWORD width,height;
	int send,received;
	BOOLEAN Invalidated;
public:
	class CPlayerInfoPlayer:public CObject
	{
	private:
		int ping;
		PCHAR name;
		DPID dpid;
		UID pid;
		BOOLEAN IsServer;

	public:
		BOOLEAN updated;
		BOOLEAN ispinging;

		CPlayerInfoPlayer();
		virtual ~CPlayerInfoPlayer();
//		virtual BOOLEAN Init();
		void SetInfo(const DPID vid,const PCHAR name,const UID vpid,const BOOLEAN vIsServer);
		void SetPing(const int p) {ping=p;}

		virtual void Draw(HDC dc);
	};

	BOOLEAN visible;

	CPlayerInfo();
	virtual ~CPlayerInfo();
	virtual void DrawUI(LPDIRECT3DDEVICE7 lpDevice);
	void MyDraw();
	void Redraw() { Invalidated=TRUE; }
	virtual void Execute(float elapsed);

	void Refresh();
	void RefreshAll();
	void ReceivePing(const PNETWORKDATA data);
	void AddSendNum();
	void AddReceivedNum();

	virtual void Invalidate();
	virtual void Restore();
};

class CBillboardText:public CObject
{
protected:
	LPDIRECTDRAWSURFACE7 lpSurf;
	BOOLEAN withalpha;
	DWORD w,h;
	BYTE length;
	float time;
public:
	CBillboardText():time(0.0f),w(256),h(256),withalpha(TRUE),lpSurf(NULL),length(0) {}

	virtual ~CBillboardText();
	virtual BOOLEAN Init();

	void InitSurface();

	virtual void Execute(float elapsed);
	virtual void DrawUI(LPDIRECT3DDEVICE7 lpDevice);
	void SetText(const PCHAR text,const COLORREF color);
	virtual void Invalidate();
	virtual void Restore();
};

class CFadeText:public CBillboardText
{
protected:
	float y;
public:
	CFadeText(const float vy);
	CFadeText();

	void SetY(const float vy) { y=vy; }
	virtual void Execute(float elapsed);
	virtual void DrawUI(LPDIRECT3DDEVICE7 lpDevice);
};

#endif