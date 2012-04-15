/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _SERVERWND_INCLUDED_
#define _SERVERWND_INCLUDED_


#include "helperlib\\wndobj.h"
#include "objects.h"



class CServerPage:public CPage
{
protected:
	float lasttime;
	BOOLEAN mustsend;

	void RemoveObject(const UID type)const;
public:
	CServerPage();

	virtual void Create();
	virtual void PerformCommand(int command,DWORD p1,int p2);
	virtual void Execute(float elapsed);
};






#endif