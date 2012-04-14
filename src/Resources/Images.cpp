#include "stdafx.h"
#include "resources.h"
#include "ocidl.h"
#include "olectl.h"



#define HIMETRIC_INCH   2540    // HIMETRIC units per inch





HGLOBAL LoadFromFile(const PCHAR name,PDWORD lpSize)
{
	HGLOBAL memory;
	const HANDLE f=CreateFile(name,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,0);
	if (f==INVALID_HANDLE_VALUE)return 0;

	*lpSize=GetFileSize(f,NULL);

	memory=GlobalAlloc(GMEM_MOVEABLE,*lpSize);

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

HGLOBAL LoadFromRessource(const HINSTANCE lib,const PCHAR name,PDWORD lpSize)
{
	const HRSRC rc=FindResource(lib,name,"JPEG");
	if (rc==0)return 0;
	*lpSize=SizeofResource(lib,rc);
	const HGLOBAL g1=LoadResource(lib,rc),g2=GlobalAlloc(GMEM_MOVEABLE,*lpSize);
	const LPVOID p1=LockResource(g1),p2=GlobalLock(g2);

	CopyMemory(p2,p1,*lpSize);

	GlobalUnlock(g2);
	return g2;
}

HBITMAP LoadJPEGFromMemory(const HGLOBAL memory,LPWORD width,LPWORD height)
{
	IPicture *picture;
	IStream *stream;
	WORD _cx,_cy;
	if (width==NULL)width=&_cx;
	if (height==NULL)height=&_cy;


//	Bild aus Stream laden, der memory-Speicher wird automatisch freigegeben
	CoInitialize(NULL);
	CreateStreamOnHGlobal(memory,TRUE,&stream);
	if (stream==NULL)
	{
		CoUninitialize();
		return 0;
	}

	HRESULT hr=OleLoadPicture(stream,0,FALSE,IID_IPicture,(void**)&picture);
	if (hr!=S_OK)
	{
		stream->Release();
		CoUninitialize();
		return 0;
	}

//	Bild geladen, jetzt Rendern =)
	HBITMAP bitmap;
	{
		OLE_XSIZE_HIMETRIC w,h;

		HDC memdc,desktopdc;

		hr=picture->get_Width(&w);
		hr=picture->get_Height(&h);

		desktopdc=GetDC(0);
		memdc=CreateCompatibleDC(desktopdc);

		{	// Convert units
			DWORD cxPerInch = GetDeviceCaps(memdc,LOGPIXELSX);
			DWORD cyPerInch = GetDeviceCaps(memdc,LOGPIXELSY);
			
			*width=MulDiv(w, cxPerInch, HIMETRIC_INCH);
			*height=MulDiv(h, cyPerInch, HIMETRIC_INCH);
		}

		bitmap=CreateCompatibleBitmap(desktopdc,*width,*height);
		ReleaseDC(0,desktopdc);

		SelectObject(memdc,bitmap);

		hr=picture->Render(memdc,0,0,*width,*height,0,h,w,-h,NULL);

		RestoreDC(memdc,-1);
		DeleteDC(memdc);
	}

//	Und aufräumen	
	picture->Release();
	stream->Release();

	CoUninitialize();
	return bitmap;
}

HBITMAP LoadJPEG(const HINSTANCE lib,const PCHAR name,LPWORD width,LPWORD height)
{
	WORD _cx,_cy;
	if (width==NULL)width=&_cx;
	if (height==NULL)height=&_cy;
 
	HGLOBAL memory=0;
	DWORD size=0;

//	Bild suchen (erst Datei, dann Ressource)
	if (memory==NULL)memory=LoadFromFile(name,&size);
	if (memory==NULL)memory=LoadFromRessource(lib,name,&size);
	if (memory==NULL)return 0;

//	Bild aus Speicher erstellen
	HBITMAP bitmap=LoadJPEGFromMemory(memory,width,height);
	return bitmap;
}





const HBITMAP LoadPicture(const PCHAR name)
{
	HBITMAP bmp=0;

#ifdef _DEBUG
	CHAR c[100];
	sprintf(&c[0],"Lade Bild \"%s\".",name);
	DebugOut(&c[0]);
#endif

	// JPEG erst als Datei, dann als Resource
	bmp=LoadJPEG(hInstance,name,NULL,NULL);
	if (bmp!=0)return bmp;

	// Bitmap als Datei
	char* t=(char*)malloc(strlen(name)+4);
	strcpy(t,".\\");
	strcat(t,name);
	bmp=(HBITMAP)LoadImage(0,t,IMAGE_BITMAP,0,0,LR_DEFAULTSIZE|LR_LOADFROMFILE);
	if (bmp!=0)return bmp;

	// Bitmap als Resource
	bmp=LoadBitmap(hInstance,name);
	if (bmp!=0)return bmp;

	// Nix gefunden; 0 zurückgeben
	return 0;
}

HBITMAP LoadBitmapFromMemory(const LPVOID memory)
{
	HDC deskdc=GetDC(0);
	
	BITMAPINFOHEADER* bih=(BITMAPINFOHEADER*)(LPBYTE(memory)+sizeof(BITMAPFILEHEADER));
	LPVOID data=LPBYTE(memory)+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFO);
	BITMAPINFO* bi=(BITMAPINFO*)bih;

	HBITMAP bmp=CreateDIBitmap(deskdc,bih,CBM_INIT,data,bi,DIB_RGB_COLORS);

	ReleaseDC(0,deskdc);

	return bmp;
}

HBITMAP LoadJPEGFromMemory(const LPVOID memory,const DWORD datasize)
{
	const HGLOBAL g=GlobalAlloc(0,datasize);
	if (g==0)return 0;

	const LPVOID p=GlobalLock(g);
	CopyMemory(p,memory,datasize);
	GlobalUnlock(g);

	WORD w,h;
	return LoadJPEGFromMemory(g,&w,&h);
}

const HBITMAP LoadPictureFromMemory(const LPVOID memory,const DWORD datasize)
{
	HBITMAP bmp=0;

	// Bitmap
	if (bmp==0)bmp=LoadBitmapFromMemory(memory);
	if (bmp==0)bmp=LoadJPEGFromMemory(memory,datasize);

	return bmp;
}












const BOOLEAN SaveBitmap(const HBITMAP bitmap,const HDC dc,const PCHAR filename)
{
    int         hFile;
    OFSTRUCT    ofReOpenBuff;
    HBITMAP     hTmpBmp, hBmpOld;
    BOOL        bSuccess;
    BITMAPFILEHEADER    bfh;
    PBITMAPINFO pbmi;
    PBYTE       pBits;
    BITMAPINFO  bmi;
    PBYTE pjTmp, pjTmpBmi;
    ULONG sizBMI;


    bSuccess = TRUE;
    if (!bitmap) return FALSE;

    //
    // Let the graphics engine to retrieve the dimension of the bitmap for us
    // GetDIBits uses the size to determine if it's BITMAPCOREINFO or BITMAPINFO
    // if BitCount != 0, color table will be retrieved
    //
    bmi.bmiHeader.biSize = 0x28;              // GDI need this to work
    bmi.bmiHeader.biBitCount = 0;             // don't get the color table
    if ((GetDIBits(dc, bitmap, 0, 0, (LPSTR)NULL, &bmi, DIB_RGB_COLORS)) == 0) {
       return FALSE;
    }

    //
    // Now that we know the size of the image, alloc enough memory to retrieve
    // the actual bits
    //
    if ((pBits = (PBYTE)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
                bmi.bmiHeader.biSizeImage)) == NULL) {
        return FALSE;
    }

    //
    // Note: 24 bits per pixel has no color table.  So, we don't have to
    // allocate memory for retrieving that.  Otherwise, we do.
    //
    pbmi = &bmi;                                      // assume no color table

    switch (bmi.bmiHeader.biBitCount) {
        case 24:                                      // has color table
            sizBMI = sizeof(BITMAPINFOHEADER);
            break;
        case 16:
        case 32:
            sizBMI = sizeof(BITMAPINFOHEADER)+sizeof(DWORD)*3;
            break;
        default:
            sizBMI = sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*(1<<bmi.bmiHeader.biBitCount);
            break;

    }

    //
    // Allocate memory for color table if it is not 24bpp...
    //
    if (sizBMI != sizeof(BITMAPINFOHEADER)) {
        ULONG       sizTmp;
        //
        // I need more memory for the color table
        //
        if ((pbmi = (PBITMAPINFO)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizBMI )) == NULL) {
            bSuccess = FALSE;
            goto ErrExit1;
        }
        //
        // Now that we've a bigger chunk of memory, let's copy the Bitmap
        // info header data over
        //
        pjTmp = (PBYTE)pbmi;
        pjTmpBmi = (PBYTE)&bmi;
        sizTmp = sizeof(BITMAPINFOHEADER);

        while(sizTmp--)
        {
            *(((PBYTE)pjTmp)++) = *((pjTmpBmi)++);
        }
    }

    //
    // Let's open the file and get ready for writing
    //
    if ((hFile = OpenFile(filename, (LPOFSTRUCT)&ofReOpenBuff,
                 OF_CREATE | OF_WRITE)) == -1) {
        goto ErrExit2;
    }

    //
    // But first, fill in the info for the BitmapFileHeader
    //
    bfh.bfType = 0x4D42;                            // 'BM'
    bfh.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+sizBMI+
        pbmi->bmiHeader.biSizeImage;
    bfh.bfReserved1 =
    bfh.bfReserved2 = 0;
    bfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizBMI;

    //
    // Write out the file header now
    //
    if (_lwrite(hFile, (LPSTR)&bfh, sizeof(BITMAPFILEHEADER)) == -1) {
        bSuccess = FALSE;
        goto ErrExit3;
    }

    //
    // Bitmap can't be selected into a DC when calling GetDIBits
    // Assume that the hDC is the DC where the bitmap would have been selected
    // if indeed it has been selected
    //
    if (hTmpBmp = CreateCompatibleBitmap(dc, pbmi->bmiHeader.biWidth, pbmi->bmiHeader.biHeight)) {
        hBmpOld = (HBITMAP)SelectObject(dc, hTmpBmp);
        if ((GetDIBits(dc, bitmap, 0, pbmi->bmiHeader.biHeight, (LPSTR)pBits, pbmi, DIB_RGB_COLORS))==0){
            bSuccess = FALSE;
            goto ErrExit4;
        }
    } else {
        bSuccess = FALSE;
        goto ErrExit3;
    }

    //
    // Now write out the BitmapInfoHeader and color table, if any
    //
    if (_lwrite(hFile, (LPSTR)pbmi, sizBMI) == -1) {
        bSuccess = FALSE;
        goto ErrExit4;
    }

    //
    // write the bits also
    //
    if (_lwrite(hFile, (LPSTR)pBits, pbmi->bmiHeader.biSizeImage) == -1) {
        bSuccess = FALSE;
        goto ErrExit4;
    }

ErrExit4:
    SelectObject(dc, hBmpOld);
    DeleteObject(hTmpBmp);
ErrExit3:
    _lclose(hFile);
ErrExit2:
    GlobalFree(pbmi);
ErrExit1:
    GlobalFree(pBits);
    return bSuccess;
}




