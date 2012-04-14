#ifndef _WNDOBJ_INCLUDED_
#define _WNDOBJ_INCLUDED_

#include "cursor.h"


const MAX_CHILD=100;


class CWnd
{
protected:
	LPDIRECTDRAWSURFACE7 surf;
	const LPDIRECTDRAW7 lpDD;

	CWnd* child[MAX_CHILD];
	CWnd* childwindow;
	BOOLEAN Invalidated;
	const int command;

	const int SearchChildPos(const int fx,const int fy,const int ox,const int oy)const;
	const int SearchChild(const int offsetx,const int offsety)const;

	void DeleteChilds();
	const BOOLEAN IsMouseClick(const UINT uMsg,const LPARAM lParam)const;
public:
	int x,y,w,h;
	CWnd* owner;
	PCHAR title;
	BOOLEAN closed;
	int returnvalue;
	BOOLEAN canfocus,focused;

	CWnd(const LPDIRECTDRAW7 lpDD,const int vx,const int vy,const int vw,const int vh,CWnd* vowner,const int vcommand=0);
	virtual ~CWnd();

	void PreBlt();
	virtual void Blt(LPDIRECTDRAWSURFACE7 lpSurf);
	virtual void Draw(HDC dc);
	virtual void Execute(float elapsed);
	virtual BOOLEAN PerformMessage(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	virtual void PerformCommand(int command,DWORD p1,int p2) {}
	virtual void Create() {}

	void AddChild(CWnd* o);
	void Invalidate() {Invalidated=TRUE; if (owner)owner->Invalidated=TRUE;}
	void InvalidateAll() { if (childwindow!=NULL) { childwindow->InvalidateAll(); return; } Invalidated=TRUE; for (int i=0;i<MAX_CHILD;i++)if (child[i]!=NULL) child[i]->InvalidateAll(); }
	void SetTitle(const PCHAR t);
	void CreateChildWindow(CWnd *wnd);
	const int GetFocusedChild()const;
	const int GetPrevChild(const int nr)const;
	const int GetNextChild(const int nr)const;
	const int FindChild(CWnd* wnd)const;
	void SetFocus(const int nr);
	void Close(const int retval=0);
	virtual void ScreenToClient(LPPOINT lpPoint)const { if (owner!=NULL)owner->ScreenToClient(lpPoint); }
	const BOOLEAN IsInside(const LPPOINT p)const { return !((p->x<x)||(p->x>x+w)||(p->y<y)||(p->y>y+h)); }
	CWnd* FindChild(const int command)const;
};

class CButton:public CWnd
{
private:
//	const int command;
public:
	CButton(const int vx,const int vy,const int vw,const int vh,CWnd* vowner,const PCHAR title,const int vcommand);
	virtual BOOLEAN PerformMessage(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	virtual void Draw(HDC dc);
};

class CCheckBox:public CWnd
{
private:
//	const int command;
	BOOLEAN checked;
public:
	CCheckBox(const int vx,const int vy,const int vw,const int vh,CWnd* vowner,const PCHAR title,const int vcommand,const BOOLEAN checked);
	virtual BOOLEAN PerformMessage(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	virtual void Draw(HDC dc);
	void SetCheck(const BOOLEAN c) {checked=c;}
	const BOOLEAN GetCheck()const {return checked;}
};

class CPage:public CWnd
{
protected:
	static CCursor* lpCursor;
private:
	int sx,sy;
public:
	const HWND hWnd;

	CPage(const LPDIRECTDRAW7 lpDD,const PCHAR title,const int vw,const int vh,const HWND vhWnd);
	virtual ~CPage();
	virtual void Draw(HDC dc);
	virtual BOOLEAN PerformMessage(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	virtual void Blt(LPDIRECTDRAWSURFACE7 lpSurf);
	virtual void ScreenToClient(LPPOINT lpPoint)const { lpPoint->x=int(lpPoint->x*(800.0f/sx)); lpPoint->y=int(lpPoint->y*(600.0f/sy)); }
	virtual void Execute(float elapsed);
	virtual void SetCursor() { }
};

class CGroupBox:public CWnd
{
public:
	CGroupBox(const int vx,const int vy,const int vw,const int vh,const PCHAR title,CWnd* vowner);
	virtual void Draw(HDC dc);
};

class CStaticText:public CWnd
{
public:
	CStaticText(const int vx,const int vy,const int vw,const int vh,CWnd* vowner,const PCHAR text);
	virtual void Draw(HDC dc);
};

class CSlider:public CWnd
{
private:
	const float iMin,iMax,stepsize;
	float iLeft,iRight;
	float Position;
	BOOLEAN LockMax,LockMin;
//	const int command;
	BOOLEAN activated,MouseButtonDown;
	int OldMouseX;

	const BOOLEAN MoveTo(const float np);
public:
	CSlider(const int vx,const int vy,const int vw,const int vh,CWnd* vowner,const float vmin,const float vmax,const float pos,const float vstepsize,const int vcommand,const BOOLEAN lockmin,const BOOLEAN lockmax);	
	virtual BOOLEAN PerformMessage(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	virtual void Draw(HDC dc);
	const float GetPosition()const {return Position;}
	const BOOLEAN SetPosition(const float pos);
	void SetMaxLocked(const BOOLEAN l) { LockMax=l;}
	void SetMinLocked(const BOOLEAN l) { LockMin=l;}
};

class CList:public CWnd
{
private:
//	const int command;
	PCHAR *strings;
	int numstrings;
	int selected;
	int activated;
	int starty;
	BOOLEAN scrolling;

	const int VisibleLines()const;
public:
	CList(const int vx,const int vy,const int vw,const int vh,CWnd* vowner,const int vcommand);
	virtual ~CList();

	virtual BOOLEAN PerformMessage(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	virtual void Draw(HDC dc);

	const int Add(const PCHAR string);
	void SetSel(const int n);
	const int GetSel()const { return selected; }
	const PCHAR GetString(const int index)const { return strings[index]; }
	void Optimize();
};



#endif