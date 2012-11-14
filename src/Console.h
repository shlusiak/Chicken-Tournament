/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _CONSOLE_INCLUDED_
#define _CONSOLE_INCLUDED_

#include "objects.h"

//const console_width=512;
//const console_textheight=20;
//const maxlines=console_width/(console_textheight)-1;
const int maxlines=20;


class CConsole:public CObject
{
private:
	int maxconsolelength;
	HBITMAP Bitmap;
	BOOLEAN visible;
	PCHAR akt,prev;
	struct LINES
	{
		PCHAR text;
		COLORREF color;
	}lines[maxlines];

	DWORD width,height;

	LPDIRECTDRAWSURFACE7 surf;
	BOOLEAN bRedraw;

	void Info();
	void HandleVariable(const PCHAR name,float* var,const BOOLEAN ServerOnly,const int max,const PCHAR p1);
	const int PrintLine(HDC dc,const PCHAR text,const int pos,int *xpos)const;
	void AddChar(const PCHAR a,const CHAR c);
public:

	CConsole();
	virtual ~CConsole();
	BOOLEAN Init();

	void Toggle();
	const BOOLEAN IsVisible()const {return visible;}
	virtual void DrawUI(LPDIRECT3DDEVICE7 lpDevice);
	void MyDraw();
	void WMKEYDOWN(const DWORD Key);
	void WMCHAR(const WORD Key);
	void Exec();
	void Add(const PCHAR s,const COLORREF color=RGB(0,214,255));
	void Redraw() { bRedraw=TRUE; }

	virtual void Invalidate();
	virtual void Restore();
};





#endif