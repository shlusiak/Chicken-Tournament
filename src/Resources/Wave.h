//-----------------------------------------------------------------
// Wave Object
// C++ Header - Wave.h
//-----------------------------------------------------------------

#ifndef __WAVE_H__
#define __WAVE_H__

//-----------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------
#include <MMSystem.h>

//-----------------------------------------------------------------
// CWave Class - Wave Object
//-----------------------------------------------------------------
class CWave {
    // Public Constructor(s)/Destructor
public:
            CWave();
            CWave(const char* pszFileName);
            CWave(const char* uiResID, HMODULE hmod);
    virtual ~CWave();

  // Public Methods
public:
    BOOL    Create(const char* pszFileName);
    BOOL    Create(const char* uiResID, HMODULE hmod);
    BOOL    IsValid() const { return (m_pImageData ? TRUE :
                FALSE); };
//    BOOL    Play(BOOL bAsync = TRUE, BOOL bLooped = FALSE) const;
    BOOL    GetFormat(WAVEFORMATEX& wfFormat) const;
    DWORD   GetDataLen() const;
    DWORD   GetData(BYTE*& pWaveData, DWORD dwMaxToCopy) const;

    // Protected Methods
protected:
    BOOL    Free();

    // Private Data
private:
    BYTE* m_pImageData;
    DWORD m_dwImageLen;
    BOOL  m_bResource;
	HGLOBAL hgmemWave;
};

#endif
