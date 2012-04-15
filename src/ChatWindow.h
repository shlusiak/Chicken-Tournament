/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _CHATWINDOW_INCLUDED_
#define _CHATWINDOW_INCLUDED_

#include "objects.h"


class CChatWindow:public CObject
{
private:
	BOOLEAN visible;
	float visibility;
	LPDIRECTDRAWSURFACE7 lpSurface;
	HBITMAP bitmap;
	BOOLEAN mustupdate;
	DWORD width,height;
	PCHAR lpText;

	void MyDraw(const HDC dc) const;
	void MyInvalidate() {mustupdate=TRUE;}
public:
	CChatWindow():visible(FALSE),visibility(0),lpSurface(NULL),mustupdate(FALSE),width(1),height(1),lpText(NULL) {}
	
	virtual ~CChatWindow();

	const BOOLEAN IsVisible() const { return visible; }
	void Toggle();
	void WMCHAR(const WORD wParam);
	void WMKEYDOWN(const DWORD Key);

	virtual void DrawUI(LPDIRECT3DDEVICE7 lpDevice);
	void UpdateSurface();
	virtual void Execute(float elapsed);

	virtual void Invalidate() {SaveRelease(lpSurface);}
};









#endif