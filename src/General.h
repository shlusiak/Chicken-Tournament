/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifdef _DEBUG

inline void DebugOut(PCHAR x) {OutputDebugString(x);OutputDebugString("\n");}
#define ASSERT(x) if (!(x)){CHAR c[100]; sprintf(&c[0],"ASSERTION failed in file \"%s\" in line %d.",__FILE__,__LINE__); DebugOut(&c[0]);}

#else

inline void DebugOut(PCHAR x) {}
#define ASSERT(x)

#endif


#define SaveRelease(x) if (x){x->Release();x=NULL;}
#ifndef _GENERAL_INCLUDED_
#define _GENERAL_INCLUDED_

#define SaveDelete(x) if (x){delete x;x=NULL;}

#define debugvar(x) { CHAR ch[100]; sprintf(&ch[0],"%s: %f",#x,x); DebugOut(&ch[0]);}


extern HINSTANCE hInstance;

#ifndef VK_OEM_5
#define VK_OEM_5 0xDC
#endif
#ifndef VK_OEM_6
#define VK_OEM_6 0xDD
#endif

#ifndef UDM_SETPOS32
#define UDM_SETPOS32 (WM_USER+113)
#endif
#ifndef UDM_GETPOS32
#define UDM_GETPOS32 (WM_USER+114)
#endif

#endif

