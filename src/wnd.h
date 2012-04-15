/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _WND_INCLUDED_
#define _WND_INCLUDED_


#include "helperlib\\wndobj.h"


class CMainPage:public CPage
{
private:
public:
	CMainPage();
	virtual ~CMainPage();

	virtual void Create();
	virtual void PerformCommand(int command,DWORD p1,int p2);
	virtual void Draw(HDC dc);
	virtual void SetCursor();
};


#endif