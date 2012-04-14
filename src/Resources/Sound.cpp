#include "stdafx.h"
#include "resources.h"
#include "mmsystem.h"
#include "dsound.h"
#include "wave.h"



const LPDIRECTSOUNDBUFFER CreateSoundBuffer(const LPDIRECTSOUND lpDS,const PCHAR filename,const BOOLEAN is3d)
{	
	WAVEFORMATEX wfFormat;
	LPDIRECTSOUNDBUFFER DSBuffer;
	DWORD dwDataLen;
	DSBUFFERDESC dsbdDesc;
	CWave w;

	if (lpDS==NULL)return NULL;

	// Erst Datei, dann Ressource
	if (!w.IsValid()) w.Create(filename);
	if (!w.IsValid()) w.Create(filename,hInstance);

	if (!w.IsValid())return NULL;

	w.GetFormat(wfFormat);
	dwDataLen=w.GetDataLen();

	ZeroMemory(&dsbdDesc,sizeof(dsbdDesc));
	dsbdDesc.dwSize=sizeof(DSBUFFERDESC);
	dsbdDesc.dwFlags=DSBCAPS_CTRLPAN|DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLFREQUENCY|DSBCAPS_STATIC;
	if (is3d)dsbdDesc.dwFlags|=DSBCAPS_CTRL3D;

	dsbdDesc.dwBufferBytes=dwDataLen;
	dsbdDesc.lpwfxFormat=&wfFormat;

	if (FAILED(lpDS->CreateSoundBuffer(&dsbdDesc,&DSBuffer,NULL)))
		return NULL;

//	Puffer füttern
	BYTE *pDSBuffData;
	if (DSBuffer->Lock(0,dwDataLen,(void**)&pDSBuffData,&dwDataLen,NULL,0,0)!=DS_OK)
	{
		DSBuffer->Release();
		return NULL;
	}
	dwDataLen=w.GetData(pDSBuffData,dwDataLen);

	if (DSBuffer->Unlock(pDSBuffData,dwDataLen,NULL,0)!=DS_OK)
	{
		DSBuffer->Release();
		return NULL;
	}

	return DSBuffer;
}

const LPDIRECTSOUND3DBUFFER CreateSound3DBuffer(const LPDIRECTSOUND ds,const LPDIRECTSOUNDBUFFER buf)
{
	if ((buf==NULL)||(ds==NULL))return NULL;
	LPDIRECTSOUND3DBUFFER b3d;
	buf->QueryInterface(IID_IDirectSound3DBuffer,(void**)&b3d);
	return b3d;
}
