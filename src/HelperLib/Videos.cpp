#include "stdafx.h"
#include "videos.h"
//#include "uuids.h"
//#include "control.h"


/*
BOOLEAN closed=FALSE;
CDShow *show=NULL;




BOOLEAN init_show(const PCHAR filename)
{
	closed=FALSE;
	show=NULL;
	show=new CDShow();
	if (show==NULL) 
	{
		return FALSE;
	}
    if (!show->Open(filename,game->lpDD))
	{
		delete show;
		show=NULL;
		return FALSE;
	}

	if (show->Start()==FALSE)
	{
		show->Stop();
		delete show;
		closed=TRUE;
		ShowCursor(TRUE);
		MessageBox(game->wnd,"Video konnte irgendwie nicht korrekt abgespielt werden.","Nanu!?",MB_OK|MB_ICONEXCLAMATION);
		ShowCursor(FALSE);

		return FALSE;
	}
	return TRUE;
}

typedef BOOLEAN EXTRACTVIDEOFILE(PCHAR resname,PCHAR lpFilename);

BOOLEAN ShowVideo(const HINSTANCE hInstance,const PCHAR filename)
{
	CHAR fn[MAX_PATH];
	strcpy(&fn[0],filename);

	if (!init_show(&fn[0]))
	{
		return TRUE;
	}

	LPDIRECTDRAWSURFACE4 lpSurface4;
	if (FAILED(game->lpDDSBack->QueryInterface(IID_IDirectDrawSurface4,(LPVOID*)&lpSurface4)))
		return TRUE;


	BOOL bGotMsg;
	MSG msg;
	PeekMessage(&msg,NULL,0,0,PM_NOREMOVE);

	while (!closed)
	{
		if (!IsWindow(game->wnd))
		{
			PostQuitMessage(0);
			closed=TRUE;
			if (show)show->Stop();
			break;
		}

		bGotMsg=PeekMessage(&msg,NULL,0,0,PM_REMOVE);
		if (bGotMsg)
		{
			TranslateMessage(&msg);
			if (((msg.message==WM_KEYDOWN)&&(msg.wParam==VK_ESCAPE))||
				(msg.message==WM_LBUTTONDOWN)||(msg.message==WM_RBUTTONDOWN))
			{
				if (show)show->Stop();
				closed=TRUE;
			}

			DispatchMessage(&msg);
		}else{
			if (show->Draw(lpSurface4,NULL)==FALSE)
			{
				show->Stop();
				closed=TRUE;
			}
			if (!show->IsPlaying()) 
			{
				closed=TRUE;
				break;
			}
	
			game->Flip();
		}
	}
	if (show)
	{
		show->Stop();
		delete show;
		show=NULL;
	}
	lpSurface4->Release();

	if (!IsWindow(game->wnd))return FALSE;
	return TRUE;
}

*/


#include "dshow.h"
//#include "atlbase.h"


const int WM_GRAPHNOTIFY=WM_USER+31;



BOOLEAN ShowVideo(const HINSTANCE hInstance,const HWND wnd,const BOOLEAN fullscreen,const PCHAR filename)
{
	IGraphBuilder * pGB=NULL;
	IMediaControl *pMC=NULL;
	IVideoWindow *pVW=NULL;
	IBasicVideo *pBV=NULL;
	IBasicAudio *pBA=NULL;
	IMediaEventEx *pME=NULL;
	BOOLEAN closed=FALSE;
	MSG msg;
	HRESULT hr;

	CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC_SERVER,IID_IGraphBuilder,(void**)&pGB);

	WCHAR wPath[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, filename, -1, wPath, sizeof(wPath)/sizeof(wPath[0]));
	hr=pGB->RenderFile(wPath,NULL);
	if (hr!=0)goto error;

	pGB->QueryInterface(IID_IMediaControl,(void**)&pMC);
	pGB->QueryInterface(IID_IMediaEventEx,(void**)&pME);
	pGB->QueryInterface(IID_IVideoWindow,(void**)&pVW);
	pGB->QueryInterface(IID_IBasicVideo,(void**)&pBV);
	pGB->QueryInterface(IID_IBasicAudio,(void**)&pBA);

	if (!(pMC && pME && pVW && pBV && pBA))goto error;

	pME->SetNotifyWindow((OAHWND)wnd,WM_GRAPHNOTIFY,0);
	pVW->put_Owner((OAHWND)wnd);
	pVW->put_WindowStyle(WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN);

	RECT rect;
	GetClientRect(wnd,&rect);
//	pVW->SetWindowPosition(rect.left,rect.top,rect.right,rect.bottom);

	ShowWindow(wnd,SW_SHOWNORMAL);
	UpdateWindow(wnd);
	SetForegroundWindow(wnd);
	SetFocus(wnd);

	pMC->Run();

	pVW->SetWindowPosition(rect.left,rect.top,rect.right,rect.bottom);

	SetForegroundWindow(wnd);
	SetFocus(wnd);

	OAHWND hDrain;
	if (fullscreen)
	{
		pVW->get_MessageDrain(&hDrain);
		pVW->put_MessageDrain((OAHWND)wnd);
		pVW->put_FullScreenMode(OATRUE);
	}

	while (closed==FALSE)
	{
		if (!IsWindow(wnd))break;

		if (GetMessage(&msg,0,0,0)==FALSE)
		{
			closed=TRUE;
			break;
		}
//		while (!PeekMessage(&msg,0,0,0,PM_REMOVE)) Sleep(300);


		TranslateMessage(&msg);
		switch(msg.message)
		{
		case WM_QUIT:
			closed=TRUE;
			break;
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			closed=TRUE;
			break;
		case WM_KEYDOWN:
			if (msg.wParam==VK_ESCAPE)
			{
				closed=TRUE;
			}
			break;
		case WM_GRAPHNOTIFY:
			{
				LONG code,param1,param2;

				while (SUCCEEDED(pME->GetEvent(&code,&param1,&param2,0)))
				{
					pME->FreeEventParams(code,param1,param2);
					switch(code)
					{
					case EC_WINDOW_DESTROYED:
					case EC_USERABORT:
						DestroyWindow(wnd);
					case EC_COMPLETE:
					case EC_FULLSCREEN_LOST:
						closed=TRUE;
						break;
					}
				}
			}
			break;
		default:
			DispatchMessage(&msg);
//			DefWindowProc(msg.hwnd,msg.message,msg.wParam,msg.lParam);
			break;
		}

		if (pVW)pVW->NotifyOwnerMessage((long)msg.hwnd,(long)msg.message,(long)msg.wParam,(long)msg.lParam);
	}

error:
	if (pMC)pMC->Stop();

	if ((fullscreen)&&(pVW))
	{
		pVW->put_FullScreenMode(OAFALSE);
		pVW->put_MessageDrain(hDrain);
	}

	if (pVW)
	{
		pVW->put_Visible(OAFALSE);
		pVW->put_Owner(NULL);
	}

	if (pME)pME->SetNotifyWindow((OAHWND)NULL,0,0);

	SaveRelease(pME);
	SaveRelease(pMC);
	SaveRelease(pBA);
	SaveRelease(pBV);
	SaveRelease(pVW);
	SaveRelease(pGB);
	
	if (!IsWindow(wnd))return FALSE;
	return TRUE;
}