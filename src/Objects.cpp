/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "objects.h"
#include "game.h"


static DWORD idcnt[256];


const UID MakeUID(const BYTE type)
{
	return UID(type)|((idcnt[type]++)<<8);
}

const PCHAR GetTypeName(const UID id)
{
	static const PCHAR names[]=
	{
		"???",
		"IDUnknown",
		"IDSound",
		"IDEffect",
		"IDGartenkralle",
		"IDChicken",
		"IDBauer",
		"IDDrescher",
		"IDGolfer",
		"IDWorld",
		"IDTree",
		"IDStall",
		"IDPlasmaCannon",
		"IDItem",
		"IDEi"
	};

	switch(GetType(id))
	{
	case IDUnknown:return names[1];
	case IDSound:return names[2];
	case IDEffect:return names[3];
	case IDGartenkralle:return names[4];
	case IDChicken:return names[5];
	case IDBauer:return names[6];
	case IDDrescher:return names[7];
	case IDGolfer:return names[8];
	case IDWorld:return names[9];
	case IDTree:return names[10];
	case IDStall:return names[11];
	case IDPlasmaCannon:return names[12];
	case IDItem:return names[13];
	case IDEi:return names[14];

	default: return names[0];
	}
}




