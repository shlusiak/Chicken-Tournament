/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "game.h"
#include "resources\\resources.h"
#include "music.h"
#include "text.h"




MUSIC_TIME time=0;

#define DEFAULT_TEMPO 60


CMusic::CMusic()
:lpDirectMusic(NULL),lpDM(NULL),lpLoader(NULL),lpComposer(NULL),lpSegment(NULL),
 lpTemplate(NULL),lpStyle(NULL),lpChordMap(NULL),playing(FALSE)
{
}

CMusic::~CMusic()
{
	if (lpLoader)
	{
		lpLoader->ClearCache(GUID_DirectMusicAllTypes);
	}
	if (lpDM)
	{
        lpDM->Stop( NULL, NULL, 0, 0 );
		lpDM->CloseDown();
	}
	SaveRelease(lpStyle);
	SaveRelease(lpSegment);
	SaveRelease(lpTemplate);
	SaveRelease(lpComposer);
	SaveRelease(lpLoader);
	SaveRelease(lpDM);
}

const BOOLEAN CMusic::LoadMusic()
{
	DWORD size;
	LPVOID data;
	HPACKAGE p=LoadPackage("music.dat");
	
	data=LoadPackageData(&p,"Template.tpl",&size);

	DMUS_OBJECTDESC Desc;
	HRESULT r;

	lpLoader->EnableCache(GUID_DirectMusicAllTypes,TRUE);
	ZeroMemory(&Desc,sizeof(Desc));
	Desc.guidClass=CLSID_DirectMusicSegment;
	Desc.dwSize=sizeof(DMUS_OBJECTDESC);
	Desc.dwValidData=DMUS_OBJ_CLASS|DMUS_OBJ_NAME|DMUS_OBJ_MEMORY|DMUS_OBJ_LOADED;
	wcscpy(Desc.wszName,L"template.tpl");
	Desc.llMemLength=size;
	Desc.pbMemData=(PBYTE)data;

	r=lpLoader->GetObject(&Desc,IID_IDirectMusicSegment,(LPVOID*)&lpTemplate);
	if (!lpTemplate)goto failed;

	Desc.guidClass=CLSID_DirectMusicStyle;
	data=LoadPackageData(&p,"style.sty",&size);
	wcscpy(Desc.wszName,L"style.sty");
	Desc.llMemLength=size;
	Desc.pbMemData=(PBYTE)data;
	r=lpLoader->GetObject(&Desc,IID_IDirectMusicStyle,(LPVOID*)&lpStyle);
	if (!lpStyle)goto failed;

	Desc.guidClass=CLSID_DirectMusicChordMap;
	data=LoadPackageData(&p,"chordMap.cdm",&size);
	wcscpy(Desc.wszName,L"chordMap.cdm");
	Desc.llMemLength=size;
	Desc.pbMemData=(PBYTE)data;
	r=lpLoader->GetObject(&Desc,IID_IDirectMusicChordMap,(LPVOID*)&lpChordMap);
	if (!lpChordMap)goto failed;

//	Desc.guidClass=CLSID_DirectMusicBand;
//	data=LoadPackageData(p,"band.bnd",&size);
//	wcscpy(Desc.wszName,L"band.bnd");
//	Desc.llMemLength=size;
//	Desc.pbMemData=(PBYTE)data;
//	r=lpLoader->GetObject(&Desc,IID_IDirectMusicBand,(LPVOID*)&lpBand);
//	if (!lpBand)goto failed;

//	lpBand->CreateSegment(&lpBandSegment);

	r=lpComposer->ComposeSegmentFromTemplate(lpStyle,lpTemplate,1,lpChordMap,&lpSegment);

	if (r!=S_OK)goto failed;

	lpSegment->SetRepeats(999);

	return TRUE;

failed:

	FreePackage(&p);
	return FALSE;
}

const BOOLEAN CMusic::SetPort()
{
    IDirectMusicPort* pPort = NULL;
    DMUS_PORTPARAMS dmos;
    DMUS_PORTCAPS dmpc;
    GUID guidSynthGUID;
    HRESULT hr = S_OK;
    
    if( !SUCCEEDED(lpDirectMusic->GetDefaultPort(&guidSynthGUID)))
    {
        return FALSE;
    }
    
    ZeroMemory(&dmos, sizeof(dmos));
    dmos.dwSize = sizeof(DMUS_PORTPARAMS);
    dmos.dwChannelGroups = 1;
    dmos.dwValidParams = DMUS_PORTPARAMS_CHANNELGROUPS;
    
    if( !SUCCEEDED(lpDirectMusic->CreatePort(guidSynthGUID,
        &dmos,
        &pPort,
        NULL)))
    {
        return FALSE;
    }
    
    ZeroMemory(&dmpc, sizeof(dmpc));
    dmpc.dwSize = sizeof(DMUS_PORTCAPS);
    
    if( !SUCCEEDED(pPort->GetCaps(&dmpc)))
    {
        if (pPort) pPort->Release();
        return FALSE;
    }

    
#define _SOFTWARE_SYNTH_
#ifdef _SOFTWARE_SYNTH_
    if ((dmpc.dwClass != DMUS_PC_OUTPUTCLASS) || !(dmpc.dwFlags & DMUS_PC_DLS))
    {
        pPort->Release();
        pPort = NULL;
    }
    
    if (!pPort)
    {
        for(DWORD index = 0; hr == S_OK; index++)
        {
            ZeroMemory(&dmpc, sizeof(dmpc));
            dmpc.dwSize = sizeof(DMUS_PORTCAPS);
            
            hr = lpDirectMusic->EnumPort(index, &dmpc);
            if(hr == S_OK)
            {
                if ( (dmpc.dwClass == DMUS_PC_OUTPUTCLASS) && 
                    (dmpc.dwFlags & DMUS_PC_DLS) )
                {
                    CopyMemory(&guidSynthGUID, &dmpc.guidPort, sizeof(GUID));
                    
                    ZeroMemory(&dmos, sizeof(dmos));
                    dmos.dwSize = sizeof(DMUS_PORTPARAMS);
                    dmos.dwChannelGroups = 1;
                    dmos.dwValidParams = DMUS_PORTPARAMS_CHANNELGROUPS;
                    
                    hr = lpDirectMusic->CreatePort(guidSynthGUID, &dmos, &pPort, NULL);
                    break;
                }
            }
        }
        if (hr != S_OK)
        {
            if (pPort) pPort->Release();
            return FALSE;
        }
    }
#else
    if( 0 != _wcsicmp( dmpc.wszDescription, L"MIDI Mapper" ) )
    {
        pPort->Release();
        pPort = NULL;
    }
    
    if (!pPort)
    {
        for(DWORD index = 0; hr == S_OK; index++)
        {
            ZeroMemory(&dmpc, sizeof(dmpc));
            dmpc.dwSize = sizeof(DMUS_PORTCAPS);
            
            hr = lpDirectMusic->EnumPort(index, &dmpc);
            if(hr == S_OK)
            {
                if( 0 == _wcsicmp( dmpc.wszDescription, L"MIDI Mapper" ) )
                {
                    CopyMemory(&guidSynthGUID, &dmpc.guidPort, sizeof(GUID));
                    
                    ZeroMemory(&dmos, sizeof(dmos));
                    dmos.dwSize = sizeof(DMUS_PORTPARAMS);
                    dmos.dwChannelGroups = 1;
                    dmos.dwValidParams = DMUS_PORTPARAMS_CHANNELGROUPS;
                    
                    hr = lpDirectMusic->CreatePort(guidSynthGUID, &dmos, &pPort, NULL);
                    break;
                }
            }
        }
        if (hr != S_OK)
        {
            if (pPort) pPort->Release();
            return FALSE;
        }
    }
#endif
    
    pPort->Activate(TRUE);
    lpDM->AddPort(pPort );
    lpDM->AssignPChannelBlock(0, pPort, 1);
    if (pPort) pPort->Release();

	return TRUE;
}

const BOOLEAN CMusic::InitMusic(const BOOLEAN play)
{
	if (play==FALSE)
	{
		DebugOut("Musik nicht initialisiert, weil momentan nicht gebraucht.");
		return TRUE;
	}

	HRESULT h;
	h=CoCreateInstance(CLSID_DirectMusicPerformance,NULL,CLSCTX_INPROC_SERVER,IID_IDirectMusicPerformance,(LPVOID*)&lpDM);
	if (FAILED(h))return FALSE;

	h=CoCreateInstance(CLSID_DirectMusicLoader,NULL,CLSCTX_INPROC,IID_IDirectMusicLoader,(LPVOID*)&lpLoader);
	if (FAILED(h))return FALSE;

	h=CoCreateInstance(CLSID_DirectMusicComposer,NULL,CLSCTX_INPROC,IID_IDirectMusicComposer,(LPVOID*)&lpComposer);
	if (FAILED(h))return FALSE;


	lpDM->Init(&lpDirectMusic,game->lpDS,NULL);
	BOOL AutoDownload=TRUE;
	lpDM->SetGlobalParam(GUID_PerfAutoDownload,&AutoDownload,sizeof(BOOL));
	lpDM->AddPort(NULL);
//	if (!SetPort())DebugOut("Port konnte nicht gesetzt werden.");

	if (!LoadMusic())return FALSE;

//	lpBand->Download(lpDM);
//	lpDM->PlaySegment(lpBandSegment,DMUS_SEGF_SECONDARY,0,NULL);
//	lpDM->PlaySegment(lpBandSegment,0,0,NULL);
	if (play)lpDM->PlaySegment(lpSegment,0,time,NULL);
	playing=play;

	DebugOut("Musik initialisiert.");

	return TRUE;
}

const BOOLEAN CMusic::ToggleMusic()
{
	if ((lpDM)&&(lpDM->IsPlaying(lpSegment,NULL)==S_OK))
	{
		lpDM->GetTime(NULL,&time);
		lpDM->Stop(NULL,NULL,0,0);
		time=0;
		game->ticker->Add(T_MUSICOFF);
		playing=FALSE;
		return FALSE;
	}
	else 
	{
		if (!lpDM)
			InitMusic(TRUE);
		else lpDM->PlaySegment(lpSegment,0,time,NULL);

		game->ticker->Add(T_MUSICON);
		playing=TRUE;
		return TRUE;
	}
}

const BOOLEAN CMusic::IsMusic()const
{
	if (lpDM==NULL)return FALSE;
	if (lpSegment==NULL)return FALSE;
	return playing;
}

void CMusic::PlayModif(const PCHAR name)
{
	if (lpDM==NULL)return;
    IDirectMusicSegment* pSeg;
    HRESULT              hr;
	WCHAR *wc=(WCHAR*)malloc(strlen(name)*2+5);

	MultiByteToWideChar(0,0,name,strlen(name)+1,wc,strlen(name)+5);

    hr = lpStyle->GetMotif( wc, &pSeg );

    if( S_OK == hr )
    {
        lpDM->PlaySegment( pSeg,     DMUS_SEGF_BEAT | DMUS_SEGF_SECONDARY, 
                                     0, 
                                     NULL );
        pSeg->Release();
    }
	free(wc);
}

const BOOLEAN CGame::InitMusic(const BOOLEAN playing)
{
	CZeitMesser z("Musik initialisiert in : %d ms");
	lpMusic=new CMusic();

	if (lpMusic->InitMusic(playing)==FALSE)return FALSE;
	return TRUE;
}