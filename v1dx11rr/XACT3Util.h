// XACT3Util.h : Defines the CXACT3Util class.
//
//
//

#ifndef _XACT3UTIL_H_
#define _XACT3UTIL_H_

#include <xact3.h>


class CXACT3Util
{
    // Constructors
public:
    CXACT3Util();
    virtual ~CXACT3Util();

    // Methods
public:
    bool Initialize();
    void Terminate();
    bool LoadWaveBank(LPCWSTR szFilePath);
    bool LoadSoundBank(LPCWSTR szFilePath);
    void DoWork();

    // Implementation
protected:
    void* LoadBankFile(LPCWSTR szFilePath, DWORD* pdwFileSize);

    // Attributes
public:
    IXACT3Engine* m_pXACT3Engine;
    IXACT3WaveBank* m_pWaveBank;
    IXACT3SoundBank* m_pSoundBank;
    void* m_pWaveBankData;
    void* m_pSoundBankData;
};


#endif // _XACT3UTIL_H_