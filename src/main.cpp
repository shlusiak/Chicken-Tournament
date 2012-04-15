/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "game.h"
#include "text.h"


CHAR errormessage[200]="\0";


const PCHAR TryToRun(CGame* g)
{
#ifndef _DEBUG
	LPEXCEPTION_POINTERS exception;
	__try
#endif

	{
//		*(int*)0=5;
		if (g->Run()==FALSE)
			return g->GetErrorMessage();
	}

#ifndef _DEBUG
	__except (UnhandledExceptionFilter(exception=GetExceptionInformation()))
	{
		sprintf(&errormessage[0],E_CRITICAL,exception->ExceptionRecord->ExceptionCode,exception->ExceptionRecord->ExceptionAddress);

		return &errormessage[0];
	}
#endif
	return NULL;
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow )
{
	::hInstance=hInstance;

	// Wird für die XP-Controls gebraucht.   (???)
	HINSTANCE shelldll=LoadLibrary("shell32.dll");

	srand(GetTickCount());
    CoInitialize( NULL );

	PCHAR error;
	{
		CGame g;
		error=TryToRun(&g);
	}
	if (error!=NULL)MessageBox(0,error,E_TITLE,MB_OK|MB_ICONHAND|MB_SETFOREGROUND|MB_TOPMOST|MB_APPLMODAL);

    CoUninitialize();
	
	FreeLibrary(shelldll);
	return 0;
}



