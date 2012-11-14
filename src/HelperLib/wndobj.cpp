#include "stdafx.h"
#include "wndobj.h"
#include "utils.h"
#include "..\\resource.h"



CCursor* CPage::lpCursor=NULL;




CWnd::CWnd(const LPDIRECTDRAW7 vlpDD,const int vx,const int vy,const int vw,const int vh,CWnd* vowner,const int vcommand)
:x(vx),y(vy),w(vw),h(vh),owner(vowner),childwindow(NULL),closed(FALSE),returnvalue(0),canfocus(TRUE),lpDD(vlpDD?vlpDD:vowner->lpDD),focused(FALSE),command(vcommand)
{
	title=PCHAR(malloc(3));
	title[0]='\0';

	for (int i=0;i<MAX_CHILD;i++)child[i]=NULL;

	if (owner!=NULL) if (owner->GetFocusedChild()==-1)focused=TRUE;
	Invalidate();

	surf=CreateSurface(lpDD,w,h);

}

CWnd::~CWnd()
{
	if (childwindow)delete childwindow;
	DeleteChilds();
	SaveRelease(surf);
	free(title);
}

void CWnd::Execute(float elapsed)
{
	if (childwindow)
	{
		childwindow->Execute(elapsed);
		if (childwindow->closed)
		{
			delete childwindow;
			childwindow=NULL;
			InvalidateAll();
		}
	}
	else for (int i=0;i<MAX_CHILD;i++)if (child[i]!=NULL)child[i]->Execute(elapsed);
}

void CWnd::AddChild(CWnd *o)
{
	for (int i=0;i<MAX_CHILD;i++)if (child[i]==NULL)
	{
		child[i]=o;
		return;
	}
	delete o;
}

void CWnd::PreBlt()
{
	if (surf==NULL)	surf=CreateSurface(lpDD,w,h);
	if (surf->IsLost()==DDERR_SURFACELOST)surf->Restore();
	if (Invalidated)
	{
		HDC dc;
		if (SUCCEEDED(surf->GetDC(&dc)))
		{
			Draw(dc);
			surf->ReleaseDC(dc);
		}

		for (int i=0;i<MAX_CHILD;i++)if (child[i]!=NULL)child[i]->Blt(surf);
		if (owner!=NULL)owner->Invalidate();
		Invalidated=FALSE;
	}
}

void CWnd::Blt(LPDIRECTDRAWSURFACE7 lpSurf)
{
	if (childwindow)
	{
		childwindow->Blt(lpSurf);
		return;
	}
	PreBlt();
	RECT r={0,0,w,h};
	lpSurf->BltFast(x,y,surf,&r,0);
}

void CWnd::Draw(HDC dc)
{
	RECT r={0,0,w,h};
	HBRUSH background=CreateSolidBrush(RGB(64,64+16,64+32));
	FillRect(dc,&r,background);
	DeleteObject(background);
}

void CWnd::SetTitle(const PCHAR t)
{
	free(title);
	title=(PCHAR)malloc(strlen(t)+3);
	strcpy(title,t);
	Invalidate();
}

const int CWnd::SearchChildPos(const int fx,const int fy,const int ox,const int oy)const
{
	if ((ox==0)&&(oy==0))return -1;
	int fav=-1;
	float abst=0.0f;

	for (int i=0;i<MAX_CHILD;i++) if ((child[i])&&(child[i]->canfocus))
	{
		int cx=child[i]->x+child[i]->w/2;
		int cy=child[i]->y+child[i]->h/2;
		if (((ox<0)&&(cx<fx))||
			((ox>0)&&(cx>fx))||
			((oy<0)&&(cy<fy))||
			((oy>0)&&(cy>fy)))
		{
			float a=sqrtf(sqr(fx-cx)+sqr(fy-cy));
			if (((a<abst)||(abst==0.0f))&&(a>0.0f))
			{
				fav=i;
				abst=a;
			}
		}
	}

	return fav;
}

const int CWnd::SearchChild(const int offsetx,const int offsety)const
{
	if (GetFocusedChild()==-1)return -1;
	CWnd* akt=child[GetFocusedChild()];

	int x=akt->x+akt->w/2;
	int y=akt->y+akt->h/2;

	int n=SearchChildPos(x,y,offsetx,offsety);
	if (n==-1)
	{
		if (offsetx>0)x=0;
		if (offsetx<0)x=w;
		if (offsety>0)y=0;
		if (offsety<0)y=h;
		n=SearchChildPos(x,y,offsetx,offsety);
		if (n==GetFocusedChild())return n;
	}

	if (n!=-1)
	{
		child[n]->focused=TRUE;
		akt->focused=FALSE;
		child[n]->Invalidate();
		akt->Invalidate();
	}
	return n;
}

BOOLEAN CWnd::PerformMessage(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	if (childwindow)if (childwindow->PerformMessage(hWnd,uMsg,wParam,lParam))return TRUE;
	const int c=GetFocusedChild();
	if (c!=-1)if (child[c]->PerformMessage(hWnd,uMsg,wParam,lParam))return TRUE;

	for (int i=0;i<MAX_CHILD;i++)if (child[i]!=NULL)
	{
		if (child[i]->PerformMessage(hWnd,uMsg,wParam,lParam))return TRUE;
	}

	return FALSE;
}

const int CWnd::GetFocusedChild()const
{
	for (int i=0;i<MAX_CHILD;i++)if ((child[i]!=NULL)&&(child[i]->focused))return i;
	return -1;
}

const int CWnd::GetPrevChild(const int nr)const
{
	for (int i=nr-1;i>=0;i--)if (child[i]!=NULL)return i;
	return -1;
}

const int CWnd::GetNextChild(const int nr)const
{
	for (int i=nr+1;i<MAX_CHILD;i++)if (child[i]!=NULL)return i;
	return -1;
}

void CWnd::Close(const int retval)
{
	returnvalue=retval;
	Invalidate();
	if (owner!=NULL)delete this;
	else closed=TRUE;
}

void CWnd::SetFocus(int nr)
{
	if (child[nr]==NULL)nr=GetNextChild(nr);
	if (nr!=-1)
	{
		for (int i=0;i<MAX_CHILD;i++)if ((child[i]!=NULL)&&(child[i]->focused))
		{
			child[i]->focused=FALSE;	
			child[i]->Invalidate();
		}
		child[nr]->focused=TRUE;
		child[nr]->Invalidate();
	}
}

void CWnd::CreateChildWindow(CWnd *wnd)
{
	focused=FALSE;
	childwindow=wnd;
	wnd->Create();
}

void CWnd::DeleteChilds()
{
	for (int i=0;i<MAX_CHILD;i++)if (child[i]!=NULL)
	{
		delete child[i];
		child[i]=NULL;
	}
}

const int CWnd::FindChild(CWnd* wnd)const
{
	for (int i=0;i<MAX_CHILD;i++)
		if (child[i]==wnd)
			return i;
	return -1;
}

const BOOLEAN CWnd::IsMouseClick(const UINT uMsg,const LPARAM lParam)const
{
	if (uMsg!=WM_LBUTTONDOWN)return FALSE;
	POINT p={LOWORD(lParam),HIWORD(lParam)};
	ScreenToClient(&p);
	return IsInside(&p);
}

CWnd* CWnd::FindChild(const int command)const
{
	for (int i=0;i<MAX_CHILD;i++)
		if (child[i]->command==command)return child[i];
	DebugOut("Child nicht gefunden!");
	return NULL;
}





CButton::CButton(const int vx,const int vy,const int vw,const int vh,CWnd* vowner,const PCHAR title,const int vcommand)
:CWnd(NULL,vx,vy,vw,vh,vowner,vcommand)
{
	SetTitle(title);
}

BOOLEAN CButton::PerformMessage(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	if (canfocus==FALSE)return FALSE;

	if (IsMouseClick(uMsg,lParam))
	{
		int i=owner->FindChild(this);
		if (i!=-1)owner->SetFocus(i);

		owner->PerformCommand(command,(DWORD)this,0);
		return TRUE;
	}

	if (focused==FALSE)return FALSE;

	if ((uMsg==WM_KEYDOWN)&&((wParam==VK_RETURN)||(wParam==VK_SPACE)))
	{
		owner->PerformCommand(command,(DWORD)this,0);
		return TRUE;
	}
	return FALSE;
}

void CButton::Draw(HDC dc)
{
	const COLORREF dark=RGB(96,96,96);
	const COLORREF light=RGB(192,192,192);

	HPEN pen=CreatePen(PS_SOLID,1,focused?light:dark),oldpen=(HPEN)SelectObject(dc,pen);
	SelectObject(dc,CreateSolidBrush(canfocus?(focused?RGB(64,192,255):RGB(64,64,128)):RGB(96,96,96)));
	Rectangle(dc,0,0,w,h);
	DeleteObject(SelectObject(dc,GetStockObject(BLACK_BRUSH)));

	Line(dc,w-1,0,w-1,h-1);
	Line(dc,0,h-1,w-1,h-1);

	SelectObject(dc,oldpen);
	DeleteObject(pen);

	pen=CreatePen(PS_SOLID,1,focused?dark:light);
	SelectObject(dc,pen);

	Line(dc,0,0,0,h-1);
	Line(dc,0,0,w-1,0);

	SelectObject(dc,oldpen);
	DeleteObject(pen);

	SetBkMode(dc,TRANSPARENT);
	SetTextColor(dc,canfocus?(focused?RGB(255,255,255):RGB(192,192,192)):RGB(128,128,128));
	RECT r={0,0,w,h};
	DrawText(dc,title,strlen(title),&r,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_NOCLIP);
}


CCheckBox::CCheckBox(const int vx,const int vy,const int vw,const int vh,CWnd* vowner,const PCHAR title,const int vcommand,const BOOLEAN vchecked)
:CWnd(NULL,vx,vy,vw,vh,vowner,vcommand),checked(vchecked)
{
	SetTitle(title);
}

BOOLEAN CCheckBox::PerformMessage(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	if (canfocus==FALSE)return FALSE;
	if (IsMouseClick(uMsg,lParam))
	{
		int i=owner->FindChild(this);
		if (i!=-1)
			owner->SetFocus(i);
		Invalidate();
		checked=!checked;
		owner->PerformCommand(command,(DWORD)this,checked);
		return TRUE;
	}
	if (!focused)return FALSE;
	if ((uMsg==WM_KEYDOWN)&&((wParam==VK_RETURN)||(wParam==VK_SPACE)))
	{
		Invalidate();
		checked=!checked;
		owner->PerformCommand(command,(DWORD)this,checked);
		return TRUE;
	}
	return FALSE;
}

void CCheckBox::Draw(HDC dc)
{
	CWnd::Draw(dc);

	SetBkMode(dc,TRANSPARENT);
	SetTextColor(dc,canfocus?(focused?RGB(255,255,255):RGB(0,0,0)):RGB(160,160,160));
	RECT r;
#define chsize 15
	r.left=chsize+2;
	r.top=0;
	r.right=w;
	r.bottom=h;
	DrawText(dc,title,strlen(title),&r,DT_VCENTER|DT_SINGLELINE);

	SelectObject(dc,GetStockObject(WHITE_PEN));
	HBRUSH brush,oldbrush;
	oldbrush=(HBRUSH)SelectObject(dc,brush=CreateSolidBrush(focused?RGB(0,64,128):RGB(0,0,0)));
	const HPEN pen=CreatePen(PS_SOLID,1,canfocus?RGB(255,255,255):RGB(128,128,128)),oldpen=(HPEN)SelectObject(dc,pen);
	Rectangle(dc,0,0+(h-chsize)/2,0+chsize,0+(h+chsize)/2);
	SelectObject(dc,oldbrush);
	DeleteObject(brush);

	if (checked)
	{
#define abstand 2
//		\			//
		MoveToEx(dc,abstand,(h-chsize)/2+abstand,NULL);
		LineTo(dc,chsize-abstand,(h+chsize)/2-abstand);

//		/			//
		MoveToEx(dc,chsize-abstand-1,(h-chsize)/2+abstand,NULL);
		LineTo(dc,abstand-1,(h+chsize)/2-abstand);
	}
	SelectObject(dc,oldpen);
	DeleteObject(pen);
}



CPage::CPage(const LPDIRECTDRAW7 lpDD,const PCHAR t,const int vw,const int vh,const HWND vhWnd)
:CWnd(lpDD,0,0,800,600,NULL),sx(vw),sy(vh),hWnd(vhWnd)
{
	SetTitle(t);
	if (!lpCursor)lpCursor=new CCursor(lpDD);
	lpCursor->AddRef();
}

CPage::~CPage()
{
	if (lpCursor->Release())SaveDelete(lpCursor);
}

void CPage::Draw(HDC dc)
{
	CWnd::Draw(dc);

	HFONT font=CreateFont(60,0,0,0,FW_BOLD,0,0,0,0,0,0,NONANTIALIASED_QUALITY,0,"ARIAL");
	HFONT oldf=(HFONT)SelectObject(dc,font);

	SetBkMode(dc,TRANSPARENT);
	SetTextColor(dc,RGB(255,255,255));

	RECT r={0,0,w,70};
	DrawText(dc,title,strlen(title),&r,DT_CENTER|DT_VCENTER);

	SelectObject(dc,oldf);
	DeleteObject(font);
}

void CPage::Blt(LPDIRECTDRAWSURFACE7 lpSurf)
{
	if (childwindow)
	{
		childwindow->Blt(lpSurf);
		return;
	}
	PreBlt();
	RECT d={0,0,sx,sy};
	RECT s={0,0,w,h};

	lpSurf->Blt(&d,surf,&s,DDBLT_WAIT,NULL);

	{	// Cursor malen
		POINT mp;
		GetCursorPos(&mp);
		if (WindowFromPoint(mp)==hWnd)
		{
			::ScreenToClient(hWnd,&mp);

			lpCursor->SetPos(mp.x,mp.y);
			lpCursor->Draw(lpSurf,sx,sy);
		}
	}
}

void CPage::Execute(float elapsed)
{
	CWnd::Execute(elapsed);
	if (!childwindow)lpCursor->Execute(elapsed);
}

BOOLEAN CPage::PerformMessage(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SIZE:
		{
			RECT r;
			GetClientRect(hWnd,&r);
			sx=r.right;
			sy=r.bottom;
		}
		break;
	}

	if (childwindow)
	{
		if (childwindow->PerformMessage(hWnd,uMsg,wParam,lParam))return TRUE;
		return FALSE;
	}

	if (CWnd::PerformMessage(hWnd,uMsg,wParam,lParam))return TRUE;

	int i;

	switch (uMsg)
	{
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			Close(0);
			closed=TRUE;
			break;
		case VK_LEFT:SearchChild(-1,0);
			break;
		case VK_RIGHT:SearchChild(1,0);
			break;
		case VK_UP:SearchChild(0,-1);
			break;
		case VK_DOWN:SearchChild(0,1);
			break;
		case VK_TAB:
			{
				i=GetFocusedChild();
				if (i!=-1)
				{
					int j=i;
					do
					{
						j=GetNextChild(j);
					}while ((j!=-1)&&(!child[j]->canfocus));
					if (j!=-1)
					{
						child[j]->focused=TRUE;
						child[i]->focused=FALSE;
						child[i]->Invalidate();
						child[j]->Invalidate();
					}else{
						j=0;
						while ((j!=-1)&&(!child[j]->canfocus))
						{
							j=GetNextChild(j);
						}
						if (j!=-1)
						{
							child[i]->focused=FALSE;
							child[j]->focused=TRUE;
							child[i]->Invalidate();
							child[j]->Invalidate();
						}
					}
				}
			}
			break;
		default: return TRUE;
		}
		break;
	default:return FALSE;
	}
	return TRUE;
}


CGroupBox::CGroupBox(const int vx,const int vy,const int vw,const int vh,const PCHAR vtitle,CWnd* vowner)
:CWnd(NULL,vx,vy,vw,vh,vowner)
{
	SetTitle(vtitle);
	canfocus=FALSE;
}

void CGroupBox::Draw(HDC dc)
{
	CWnd::Draw(dc);

	SIZE s;
	GetTextExtentPoint32(dc,title,strlen(title),&s);

	SelectObject(dc,GetStockObject(WHITE_PEN));

	// Links / oben
	Line(dc,0,s.cy/2,0,h-1);
	Line(dc,10+10+s.cx,s.cy/2,w-1,s.cy/2);
	Line(dc,0,s.cy/2,10,s.cy/2);

	// Rechts / unten
	SelectObject(dc,GetStockObject(BLACK_PEN));
	Line(dc,0,h-1,w,h-1);
	Line(dc,w-1,s.cy/2,w-1,h-1);


	SetTextColor(dc,RGB(255,255,255));
	SetBkMode(dc,TRANSPARENT);
	TextOut(dc,0+10+5,0,title,strlen(title));
}




CSlider::CSlider(int vx,int vy,int vw,int vh,CWnd* vowner,float vmin,float vmax,float pos,float vstepsize,int vcommand,BOOLEAN lockmin,BOOLEAN lockmax)
:CWnd(NULL,vx,vy,vw,vh,vowner,vcommand),iMin(vmin),iMax(vmax),Position(vmin),stepsize(vstepsize),activated(FALSE),MouseButtonDown(FALSE),iLeft(vmin),iRight(vmax)
{
	SetMinLocked(lockmin);
	SetMaxLocked(lockmax);
	SetPosition(pos); 
	SetTitle("");
}

const BOOLEAN CSlider::MoveTo(const float np)
{
	const BOOLEAN b=SetPosition(np);
	if ((b)&&(command!=-1))owner->PerformCommand(command,(DWORD)this,FloatToLong(Position));

	Invalidate();
	return b;
}

#define WM_MOUSEWHEEL (0x20A)
#define WHEEL_DELTA (120)

BOOLEAN CSlider::PerformMessage(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	if (canfocus==FALSE)return FALSE;
	if (uMsg==WM_LBUTTONDOWN)
	{
		POINT p={LOWORD(lParam),HIWORD(lParam)};
		ScreenToClient(&p);
		if (!IsInside(&p))return FALSE;
		int i=owner->FindChild(this);
		if (i!=-1)owner->SetFocus(i);

		OldMouseX=p.x;

		float value=(float)p.x-(float)x-float(h)/4.0f;
		value=value/float(w-h/2)*(iRight-iLeft)+iLeft;
		MoveTo(value);
		activated=TRUE;
		Invalidate();
		SetCapture(hWnd);
		MouseButtonDown=TRUE;
		return TRUE;
	}
	if (uMsg==WM_LBUTTONUP)
	{
		MouseButtonDown=FALSE;
		activated=FALSE;
		Invalidate();
		ReleaseCapture();
		return TRUE;
	}
	if (uMsg==WM_MOUSEMOVE)
	{
		if (!MouseButtonDown)return FALSE;
		POINT p={(short)LOWORD(lParam),(short)HIWORD(lParam)};
		ScreenToClient(&p);

//		float value=(float)p.x-(float)x-float(h)/4.0f;
//		value=value/float(w-h/2)*(iRight-iLeft)+iLeft;
		float value=float(p.x-OldMouseX)/float(w-h/2)*(iRight-iLeft);
		if (MoveTo(Position+value))
			OldMouseX=p.x;
		Invalidate();
		return TRUE;
	}
	if (uMsg==WM_MOUSEWHEEL)
	{
		POINT p={LOWORD(lParam),HIWORD(lParam)};
		::ScreenToClient(hWnd,&p);
		ScreenToClient(&p);
		if (!IsInside(&p))return FALSE;
		int i=owner->FindChild(this);
		if (i!=-1)owner->SetFocus(i);

		int delta=(short)HIWORD(wParam)/WHEEL_DELTA;
		MoveTo(Position-(float)delta);
		Invalidate();
		return TRUE;
	}


	if (focused==FALSE)return FALSE;
	if (uMsg==WM_CHAR)
	{
		switch(wParam)
		{
		case '+':
			MoveTo(Position+stepsize);
			break;
		case '-':
			MoveTo(Position-stepsize);
			break;
		default:return FALSE;
		}
		return TRUE;
	}
	if (uMsg==WM_KEYDOWN)
	{
		switch(wParam)
		{
		case VK_LEFT:
			if (!activated)return FALSE;
		case VK_PRIOR:
		case VK_SUBTRACT:
			MoveTo(Position-stepsize);
			break;
		case VK_RIGHT:
			if (!activated)return FALSE;
		case VK_NEXT:
		case VK_ADD:
			MoveTo(Position+stepsize);
			break;
		case VK_HOME:
			MoveTo(iMin);
			break;
		case VK_END:
			MoveTo(iMax);
			break;
		case VK_RETURN:
			activated=(activated==FALSE);
			Invalidate();
			break;
		case VK_ESCAPE:
			if (activated)
			{
				activated=FALSE;
				Invalidate();
			}else return FALSE;
			break;
		case VK_SPACE:
			{
				if (Position>iMax)MoveTo(iMin);
				else 
				{
					if ((MoveTo(Position+stepsize)==FALSE)||(Position>iMax))
						MoveTo(iMin);
				}
			}
			break;

		default:return FALSE;
		}
		return TRUE;
	}
	if (uMsg==WM_COMMAND)
	{
		switch (LOWORD(wParam))
		{
		case ID_CTRLRIGHT:
			MoveTo(Position+stepsize);
			break;
		case ID_CTRLLEFT:
			MoveTo(Position-stepsize);
			break;

		default:return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

void CSlider::Draw(HDC dc)
{
	CWnd::Draw(dc);

	activated&=focused;
	MouseButtonDown&=focused;

	int h2=h/2;

	// Hintergrund
	{
		HBRUSH brush=CreateSolidBrush(focused?RGB(0,64,92):(canfocus?RGB(0,0,0):RGB(64,64,64)))
			,oldbrush=(HBRUSH)SelectObject(dc,brush);
		SelectObject(dc,GetStockObject(NULL_PEN));

		Rectangle(dc,0,0,w,h2+1);

		SelectObject(dc,oldbrush);
		DeleteObject(brush);


		HPEN pen=CreatePen(PS_SOLID,1,RGB(64,64,64)),oldpen=(HPEN)SelectObject(dc,pen);

		Line(dc,0,0,w-1,0);
		Line(dc,0,0,0,h2-1);

		SelectObject(dc,oldpen);
		DeleteObject(pen);

		pen=CreatePen(PS_SOLID,1,RGB(192,192,192));
		SelectObject(dc,pen);

		Line(dc,w-1,0,w-1,h2-1);
		Line(dc,0,h2-1,w-1,h2-1);

		SelectObject(dc,oldpen);
		DeleteObject(pen);
	}


	// Beschriftung
	{
		HFONT font=CreateFont(h-h2-2,0,0,0,0,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"MS SansSerif"),oldfont=(HFONT)SelectObject(dc,font);
		
		SetTextColor(dc,focused?RGB(255,255,255):RGB(192,192,192));
		RECT r={0,0+h2-1,w-1,h-1};

		SetBkMode(dc,TRANSPARENT);

		CHAR c[10];
		sprintf(&c[0],"%.2f",iLeft);
		DrawText(dc,&c[0],strlen(&c[0]),&r,DT_LEFT);

		sprintf(&c[0],"%.2f",iRight);
		DrawText(dc,&c[0],strlen(&c[0]),&r,DT_RIGHT);

		SetTextColor(dc,focused?RGB(64,192,255):RGB(192,192,192));
		sprintf(&c[0],"%.2f",Position);
		DrawText(dc,&c[0],strlen(&c[0]),&r,DT_CENTER|DT_SINGLELINE);

		SelectObject(dc,oldfont);
		DeleteObject(font);
	}

	// Knopf
	{
		HPEN pen=CreatePen(PS_SOLID,1,focused?RGB(255,255,255):canfocus?RGB(0,192,255):RGB(160,160,160)),oldpen=(HPEN)SelectObject(dc,pen);
		HBRUSH brush=CreateSolidBrush(canfocus?focused?activated?RGB(0,255,192):RGB(0,192,255):RGB(0,128,192):RGB(192,192,192)),oldbrush=(HBRUSH)SelectObject(dc,brush);

		const int breite=h2-2;
		const int hoehe=h2-2;

		int links=int(2+((w-2-breite)*(Position-iLeft))/(iRight-iLeft));

		Rectangle(dc,links-1,1,links+breite-1,hoehe+1);

		if (activated)
		{
			const int x1=links+1;
			const int x2=links+breite-4;
			const int xm=(x2-x1)/2+x1;
			const int y1=3+1;
			const int y2=hoehe-2-1;
			const int ym=(y2-y1)/2+y1;
			HPEN pen=CreatePen(PS_SOLID,2,RGB(255,0,0)),oldpen=(HPEN)SelectObject(dc,pen);

			Line(dc,xm-1,y1,x1,ym);
			Line(dc,x1,ym,xm-1,y2);

			Line(dc,xm+1,y1,x2,ym);
			Line(dc,x2,ym,xm+1,y2);

			SelectObject(dc,oldpen);
			DeleteObject(pen);
		}

		SelectObject(dc,oldbrush);
		SelectObject(dc,oldpen);
		DeleteObject(brush);
		DeleteObject(pen);
	}
}

const BOOLEAN CSlider::SetPosition(const float pos)
{
	if (Position==pos)return FALSE;
	const float op=Position;
	Position=pos;
	if ((pos<iMin)&&(LockMin))Position=iMin;
	if ((pos>iMax)&&(LockMax))Position=iMax;

	if (Position<iLeft)
	{
		iRight-=(iLeft-Position);
		iLeft=Position;
	}
	if (Position>iRight)
	{
		iLeft+=(Position-iRight);
		iRight=Position;
	}

	return (Position!=op);
}



CStaticText::CStaticText(const int vx,const int vy,const int vw,const int vh,CWnd* vowner,const PCHAR text)
:CWnd(NULL,vx,vy,vw,vh,vowner)
{
	SetTitle(text);
	canfocus=FALSE;
}

void CStaticText::Draw(HDC dc)
{
	CWnd::Draw(dc);

	RECT r={0,0,w-1,h-1};

	SetBkMode(dc,TRANSPARENT);
	SetTextColor(dc,RGB(214,214,214));
	DrawText(dc,title,strlen(title),&r,DT_SINGLELINE|DT_LEFT|DT_VCENTER);
}







const int listitem_height=20;
const int listscrollbar_width=20;

CList::CList(const int vx,const int vy,const int vw,const int vh,CWnd* vowner,const int vcommand)
:CWnd(NULL,vx,vy,vw,vh,vowner,vcommand),numstrings(0),strings(NULL),selected(-1),activated(FALSE),starty(0),scrolling(FALSE)
{ }

CList::~CList()
{
	if (strings!=NULL)
	{
		for (int i=0;i<numstrings;i++)
			free(strings[i]);
		delete[] strings;
	}
}


BOOLEAN CList::PerformMessage(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
		{
			POINT p={LOWORD(lParam),HIWORD(lParam)};
			ScreenToClient(&p);
			if (!IsInside(&p))return FALSE;
			int i=owner->FindChild(this);
			if (i!=-1)owner->SetFocus(i);
			
			if (p.x-x<w-listscrollbar_width)
			{	// Eintrag ausgewählt
				int s=(p.y-y-5)/listitem_height+starty;
				if ((s>=0)&&(s<numstrings))
					SetSel(s);
			}else{
				// Auf Scrollbar geklickt
				POINT p={(short)LOWORD(lParam),(short)HIWORD(lParam)};
				ScreenToClient(&p);

				float hoehe=(h-2.0f)*(float)VisibleLines()/(float)numstrings;
				if (VisibleLines()>numstrings)
					hoehe=(h-2.0f);

				if (numstrings!=VisibleLines()+1)
					starty=int((p.y-y-hoehe/2.0f)/(float(h-hoehe-2.0f)/float(numstrings-VisibleLines()-1)));

				if (starty+VisibleLines()>=numstrings)
					starty=numstrings-VisibleLines()-1;
				if (starty<0)starty=0;
	
				SetCapture(hWnd);
				scrolling=TRUE;
				Invalidate();
			}
			return TRUE;
		}
		break;
	case WM_MOUSEMOVE:if (scrolling)
		{
			POINT p={(short)LOWORD(lParam),(short)HIWORD(lParam)};
			ScreenToClient(&p);

			float hoehe=(h-2.0f)*(float)VisibleLines()/(float)numstrings;
			if (VisibleLines()>numstrings)
				hoehe=(h-2.0f);


			if (numstrings!=VisibleLines()+1)
				starty=int((p.y-y-hoehe/2.0f)/(float(h-hoehe-2.0f)/float(numstrings-VisibleLines()-1)));

			if (starty+VisibleLines()>=numstrings)
				starty=numstrings-VisibleLines()-1;
			if (starty<0)starty=0;

			Invalidate();
		}
		break;
	case WM_LBUTTONUP:
		{
			scrolling=FALSE;
			ReleaseCapture();
			Invalidate();
		}
		break;
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_RETURN:
			activated=!activated;
			Invalidate();
			break;
		case VK_ESCAPE:
			if (activated)
			{
				activated=FALSE;
				Invalidate();
			}else return FALSE;
			break;
		case VK_UP:
			if (activated)
			{
				SetSel(selected-1);
			}else return FALSE;
			break;
		case VK_DOWN:
			if (activated)
			{
				SetSel(selected+1);
			}else return FALSE;
			break;
		case VK_PRIOR:
			if (activated)
			{
				SetSel(selected-10);
			}else return FALSE;
			break;
		case VK_NEXT:
			if (activated)
			{
				SetSel(selected+10);
			}else return FALSE;
			break;
		case VK_HOME:
			if (activated)
			{
				SetSel(0);
			}else return FALSE;
			break;
		case VK_END:
			if (activated)
			{
				SetSel(numstrings-1);
			}else return FALSE;
			break;

		default:return FALSE;
		}
		break;
	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case ID_CTRLDOWN:
			case ID_CTRLRIGHT:
				SetSel(selected+1);
				break;
			case ID_CTRLUP:
			case ID_CTRLLEFT:
				SetSel(selected-1);
				break;

			default:return FALSE;
			}
		}
		break;
		
	default:return FALSE;
	}
	return TRUE;
}

void CList::Draw(HDC dc)
{
	activated&=focused;
	{	// Rahmen malen
		HPEN pen=CreatePen(PS_SOLID,1,focused?RGB(255,255,255):RGB(128,128,128)),oldpen;
		oldpen=(HPEN)SelectObject(dc,pen);

		HBRUSH brush=CreateSolidBrush(focused?RGB(32,32+12,32+24):RGB(8,16,32)),oldbrush;

		oldbrush=(HBRUSH)SelectObject(dc,brush);
	
		Rectangle(dc,0,0,w,h);

		SelectObject(dc,GetStockObject(BLACK_BRUSH));
		Rectangle(dc,w-listscrollbar_width,0,w,h);
		Line(dc,w-listscrollbar_width,0,w-listscrollbar_width,h);

		SelectObject(dc,oldpen);
		DeleteObject(pen);
		SelectObject(dc,oldbrush);
		DeleteObject(brush);
	}
	if (numstrings>0)
	{	// Scrollbar malen
		SelectObject(dc,GetStockObject(WHITE_PEN));
		HBRUSH brush=CreateSolidBrush((activated||scrolling)?RGB(64,192,214):RGB(64,92,128)),oldbrush;
		oldbrush=(HBRUSH)SelectObject(dc,brush);

		float hoehe=(h-2.0f)*(float)VisibleLines()/(float)numstrings;
		if (VisibleLines()>numstrings)
			hoehe=(h-2.0f);

		float y=1.0f;
		if (numstrings!=VisibleLines()+1)y=1.0f+starty*(float(h-hoehe-2.0f)/float(numstrings-VisibleLines()-1));


		Rectangle(dc,w-listscrollbar_width+1,(int)y,w-1,int(y+hoehe));

		SelectObject(dc,oldbrush);
		DeleteObject(brush);
	}

	if (strings)
	{	// Listenelemente anzeigen
		int y=5;

		HFONT font=CreateFont(listitem_height-1,0,0,0,FW_THIN,0,0,0,0,0,0,0,0,"Arial"),oldfont;

		oldfont=(HFONT)SelectObject(dc,font);
		SetBkMode(dc,TRANSPARENT);

		HBRUSH brush=CreateSolidBrush(activated?RGB(64,192,214):RGB(64,92,128));
		SelectObject(dc,GetStockObject(NULL_PEN));

		SetTextColor(dc,RGB(0,192,255));

		for (int i=starty;i<numstrings;i++)
		{
			RECT r={1,y,w-listscrollbar_width,y+listitem_height};
			if (i==selected)
			{	// Aktueller Eintrag ist der Auserwählte
				SetTextColor(dc,RGB(255,255,255));
				FillRect(dc,&r,brush);
				Sleep(1);
			}
			r.left+=5;

			DrawText(dc,strings[i],strlen(strings[i]),&r,DT_VCENTER);

			if (i==selected)
			{
				SetTextColor(dc,RGB(0,192,255));
			}

			y+=listitem_height;
			if (i>=starty+VisibleLines())break;
		}

		SelectObject(dc,oldfont);
		DeleteObject(brush);
		DeleteObject(font);
	}
}

const int CList::VisibleLines()const
{
	return (int)floorf((h-10.0f)/(float)listitem_height)-1;
}

const int CList::Add(const PCHAR string)
{
	if (selected==-1)selected=0;
	PCHAR *ns=new PCHAR[numstrings+1];
	if (strings!=NULL)
	{	// Alte Listenelemente übernehmen
		for (int i=0;i<numstrings;i++)
			ns[i]=strings[i];

		delete[] strings;
	}

	ns[numstrings]=(PCHAR)malloc(strlen(string)+5);
	strcpy(ns[numstrings],string);

	strings=ns;
	numstrings++;

	Invalidate();
	return numstrings-1;
}

void CList::SetSel(int n)
{
	if (n<0)n=0;
	if (n>numstrings-1)n=numstrings-1;
	if (selected==n)return;
	selected=n;

	Invalidate();
	if (command!=-1)owner->PerformCommand(command,(DWORD)this,selected);
	if (n-1<starty)
		starty=n-1;
	if (n+1>starty+VisibleLines())
		starty=n-VisibleLines()+1;

	if (starty+VisibleLines()>=numstrings)
		starty=numstrings-VisibleLines()-1;
	if (starty<0)starty=0;
}

void CList::Optimize()
{
//	const int n=selected;
	if (selected-1<starty)
		starty=selected-1;
	if (selected+1>starty+VisibleLines())
		starty=selected-VisibleLines()+1;

	if (starty+VisibleLines()>=numstrings)
		starty=numstrings-VisibleLines()-1;
	if (starty<0)starty=0;
}