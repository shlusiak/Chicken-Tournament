#include "stdafx.h"
#include "utils.h"
#include "..\\resource.h"
#include "cursor.h"



CCursor::CCursor(LPDIRECTDRAW7 lpDirectDraw)
:refcount(0),width(0),height(0),lpDD(lpDirectDraw),frames(0),curframe(0),nextframe(0)
{
	for (int i=0;i<sizeof(lpSurface)/sizeof(lpSurface[0]);i++)
		lpSurface[i]=NULL;
}

CCursor::~CCursor()
{
	if (refcount)DebugOut("CCursor::~CCursor(); Refcount NICHT 0!!!!!");
	for (int i=0;i<sizeof(lpSurface)/sizeof(lpSurface[0]);i++)
		SaveRelease(lpSurface[i]);
}

void CCursor::Clear()
{
	for (int i=0;i<sizeof(lpSurface)/sizeof(lpSurface[0]);i++)
		SaveRelease(lpSurface[i]);
	frames=curframe=0;
	nextframe=0.0f;
}

void CCursor::Draw(LPDIRECTDRAWSURFACE7 lpSurf,const int w,const int h)const
{
//#define STRETCHCURSOR
	if (lpSurface[curframe]==NULL)return;
	if ((CursorPos.x<0)||(CursorPos.y<0)||(CursorPos.x>w)||(CursorPos.y>w))return;

	const hotspotx=10;
	const hotspoty=5;

#ifdef STRETCHCURSOR
	const int cw=(width*w)/800;
	const int ch=(height*h)/600;

	RECT s={0,0,width,height};
	RECT d={x,y,x+cw,y+ch};
	if (d.right>w)
	{
		s.right-=((d.right-w)*800)/w;
		d.right=w;
	}
	if (d.bottom>h)
	{
		s.bottom-=((d.bottom-h)*600)/h;
		d.bottom=h;
	}

	lpSurf->Blt(&d,lpSurface[curframe],&s,DDBLT_WAIT|DDBLT_KEYSRC,NULL);
#else
	RECT src={0,0,width,height};
	int mx=CursorPos.x-hotspotx;
	int my=CursorPos.y-hotspoty;

	if (mx<0)
	{
		src.left-=mx;
		mx=0;
	}
	if (my<0)
	{
		src.top-=my;
		my=0;
	}
	if (mx+src.right>w)
		src.right=w-mx;
	if (my+src.bottom>h)
		src.bottom=h-my;

	lpSurf->BltFast(mx,my,lpSurface[curframe],&src,DDBLTFAST_SRCCOLORKEY);
#endif
}

const BOOLEAN CCursor::AddFrame(const HBITMAP bitmap)
{
	SaveRelease(lpSurface[frames]);
	if (bitmap==0)return FALSE;
	BITMAP bmp;
	GetObject(bitmap,sizeof(bmp),&bmp);

	width=bmp.bmWidth;
	height=bmp.bmHeight;

	lpSurface[frames]=CreateSurfaceFromBitmap(lpDD,bitmap);
	if (lpSurface[frames]==NULL)return FALSE;

	DDCOLORKEY key;
	key.dwColorSpaceLowValue=key.dwColorSpaceHighValue=0x00000000;

	lpSurface[frames]->SetColorKey(DDCKEY_SRCBLT,&key);

	frames++;
	return TRUE;
}	

void CCursor::Execute(const float elapsed)
{
	nextframe-=elapsed;
	if (nextframe<0)
	{
		nextframe=0.12f;
		curframe++;
		if (curframe>=frames)curframe=0;
	}
}