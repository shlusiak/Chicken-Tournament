#ifndef _RESOURCES_INCLUDED_
#define _RESOURCES_INCLUDED_


const HBITMAP LoadPicture(const PCHAR name);
const HBITMAP LoadPictureFromMemory(const LPVOID memory,const DWORD datasize);
const BOOLEAN SaveBitmap(const HBITMAP bitmap,const HDC dc,const PCHAR filename);
const HCURSOR LoadAniCursor(const HINSTANCE hmodule,const DWORD res);

const HINSTANCE GetDllInstance();


#ifdef __DSOUND_INCLUDED__
	const LPDIRECTSOUNDBUFFER CreateSoundBuffer(const LPDIRECTSOUND lpDS,const PCHAR filename,const BOOLEAN is3d=TRUE);
	const LPDIRECTSOUND3DBUFFER CreateSound3DBuffer(const LPDIRECTSOUND ds,const LPDIRECTSOUNDBUFFER buf);
#endif



struct HPACKAGE
{
	HGLOBAL glob;
	LPVOID p;
	DWORD size;
};



const HPACKAGE LoadPackage(const PCHAR package);
void FreePackage(const HPACKAGE *package);
const LPVOID LoadPackageData(const HPACKAGE *package,const PCHAR ident,const LPDWORD datasize);
const LPVOID LoadNextPackageData(const HPACKAGE *package,const LPVOID current,const LPDWORD datasize);
const BOOLEAN HasUTSounds();


#endif