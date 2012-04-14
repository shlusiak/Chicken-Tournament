#ifndef _CURSOR_INCLUDED_
#define _CURSOR_INCLUDED_

#include "..\\resources/resources.h"


class CCursor
{
protected:
	BYTE refcount;
	int width,height;
	BYTE frames,curframe;
	float nextframe;

	LPDIRECTDRAWSURFACE7 lpSurface[10];
	LPDIRECTDRAW7 lpDD;
public:
	POINT CursorPos;

	CCursor(LPDIRECTDRAW7 lpDirectDraw);
	~CCursor();

	void AddRef() {refcount++;}
	const BOOLEAN Release() {return (--refcount)==0;}

	void Draw(LPDIRECTDRAWSURFACE7 lpSurf,const int w,const int h)const;
	void Clear();
	const BOOLEAN AddFrame(const HBITMAP bitmap);
	const BOOLEAN AddFrame(const PCHAR name) { const HBITMAP bitmap=LoadPicture(name); const BOOLEAN b=AddFrame(bitmap); DeleteObject(bitmap); return b; }

	void Execute(const float elapsed);
	void SetPos(const int vx,const int vy) { CursorPos.x=vx;CursorPos.y=vy;}
};





#endif