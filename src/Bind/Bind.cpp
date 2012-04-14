#include "stdafx.h"
#include "iostream.h"
#include "stdio.h"
#include "stdlib.h"

struct TFileData
{
	CHAR filename[32];
	DWORD start,length;
};

int main(int argc, char* argv[])
{
	cout << "Suche nach Dateien...\n";
	cout.flush();

	CHAR path[MAX_PATH];
	strcpy(&path[0],"Musik\\KFC\\RuntimeFiles");
	if (argc>1)strcpy(&path[0],argv[1]);

	CHAR x[MAX_PATH];
	strcpy(&x[0],&path[0]);
	strcat(&x[0],"\\*.*");

	cout << "Suche Dateien in: " << &x[0] << "\n";
	WIN32_FIND_DATA wd;
	HANDLE find=FindFirstFile(&x[0],&wd);
	TFileData filedata;
	DWORD filepos=0;

	strcpy(&x[0],"Musik\\Music.dat");
	if (argc>2)strcpy(&x[0],argv[2]);
	cout << "Erstelle Packet: " << &x[0] << "\n\n";
	cout.flush();

	HANDLE dest=CreateFile(&x[0],GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,0);
	if (dest==INVALID_HANDLE_VALUE)
	{
		cout << "Zieldatei konnte nicht erstellt werden.\n";
		return -1;
	}

	if (find!=INVALID_HANDLE_VALUE)
	do
	{
		if ((wd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0)
		{
			ZeroMemory(&filedata,sizeof(filedata));
			CHAR c[100];
			sprintf(&c[0],"%s\\%s",&path[0],wd.cFileName);
			HANDLE src=CreateFile(&c[0],GENERIC_READ,0,NULL,OPEN_EXISTING,0,0);

			DWORD filesize=GetFileSize(src,NULL);

			strcpy(&filedata.filename[0],wd.cFileName);
			filedata.start=filepos+sizeof(filedata);
			filedata.length=filesize;
			

			DWORD written;
			WriteFile(dest,&filedata,sizeof(filedata),&written,NULL);

			LPVOID data=malloc(filesize);
			DWORD read;
			ReadFile(src,data,filesize,&read,NULL);

			WriteFile(dest,data,filesize,&written,NULL);

			filepos+=sizeof(filedata)+filesize;
			CloseHandle(src),
			cout << "Datei hinzugefuegt: " << wd.cFileName << " StartByte: " << filedata.start << ", Laenge: " << filedata.length << "\n";
			cout.flush();
		}
	}while (FindNextFile(find,&wd));
	FindClose(find);

	CloseHandle(dest);

	cout << "\nGesamtdateigroesse: " << filepos<< "\n";
	cout << "Beendet...\n";

	cin.get();
	return 0;
}
