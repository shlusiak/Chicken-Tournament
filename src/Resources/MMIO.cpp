//-----------------------------------------------------------------
// MMIO Objects
// C++ Source - MMIO.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------
#include "MMIO.h"

//-----------------------------------------------------------------
// MFC Debugging Support
//-----------------------------------------------------------------
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//-----------------------------------------------------------------
// CMMIO Public Methods
//-----------------------------------------------------------------
void CMMIO::Open(const char* pszFileName, DWORD dwOpenFlags)
{
    m_hmmio = mmioOpen((char*)pszFileName, NULL, dwOpenFlags);
}

void CMMIO::Open(CMMMemoryIOInfo &mmioinfo)
{
    m_hmmio = mmioOpen(NULL, &mmioinfo, MMIO_READWRITE);
}
