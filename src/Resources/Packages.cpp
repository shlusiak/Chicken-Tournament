#include "stdafx.h"
#include "resources.h"


extern HINSTANCE hInstance;


struct TFileData
{
	CHAR filename[32];
	DWORD start,length;
};


const HGLOBAL LoadFilePackage(const PCHAR filename,const LPDWORD lpSize)
{
	const HANDLE f=CreateFile(filename,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,0);
	if (f==INVALID_HANDLE_VALUE)return 0;

	*lpSize=GetFileSize(f,NULL);

	const HGLOBAL memory=GlobalAlloc(GMEM_MOVEABLE,*lpSize);

	const LPVOID buf=GlobalLock(memory);

	DWORD read;
	ReadFile(f,buf,*lpSize,&read,NULL);

	GlobalUnlock(memory);

	CloseHandle(f);
	if (read!=*lpSize)
	{
		GlobalFree(memory);
		return 0;
	}
	return memory;
}

const HGLOBAL LoadResPackage(const PCHAR resname,const LPDWORD lpSize)
{
	const HRSRC rc=FindResource(hInstance,resname,"PACK");
	if (rc==0)return 0;
	*lpSize=SizeofResource(hInstance,rc);
	const HGLOBAL g2=GlobalAlloc(GMEM_MOVEABLE,*lpSize);
	const LPVOID p1=LockResource(LoadResource(hInstance,rc)),p2=GlobalLock(g2);

	CopyMemory(p2,p1,*lpSize);

	GlobalUnlock(g2);
	return g2;
}


const HPACKAGE LoadPackage(const PCHAR package)
{
	HPACKAGE p;
	ZeroMemory(&p,sizeof(p));
	DWORD size;

	HGLOBAL g=LoadFilePackage(package,&size);
	if (g==0)g=LoadResPackage(package,&size);
	if (g==0)return p;

	p.glob=g;
	p.size=size;
	p.p=GlobalLock(p.glob);

	return p;
}

void FreePackage(const HPACKAGE *package)
{
	GlobalUnlock(package->glob);
	GlobalFree(package->glob);
}

const LPVOID LoadPackageData(const HPACKAGE *package,const PCHAR ident,const LPDWORD datasize)
{
	LPBYTE p=(LPBYTE)package->p;
#define filedata ((TFileData*)p)


	while (DWORD(p-PBYTE(package->p))<package->size)
	{
		if (_stricmp(ident,&filedata->filename[0])==0)
		{
			if (datasize!=NULL)*datasize=filedata->length;
			return p+sizeof(TFileData);
		}

//	Nächstes Element finden
		p+=sizeof(TFileData)+filedata->length;
	}

#undef filedata

	return NULL;
}

const LPVOID LoadNextPackageData(const HPACKAGE *package,const LPVOID current,const LPDWORD datasize)
{
	LPBYTE p=(LPBYTE)package->p;
#define filedata ((TFileData*)p)

	if (current==NULL)
	{
		if (datasize!=NULL)*datasize=filedata->length;
		return p+sizeof(TFileData);
	}else p=(LPBYTE)current;
	p-=sizeof(TFileData);


	while (DWORD(p-PBYTE(package->p))<package->size)
	{
//	Nächstes Element finden
		p+=sizeof(TFileData)+filedata->length;

		if (DWORD(p-PBYTE(package->p))>=package->size)return NULL;

		if (datasize!=NULL)*datasize=filedata->length;
		return p+sizeof(TFileData);
	}

#undef filedata

	return NULL;
}
