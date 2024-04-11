// XACT3Util.cpp : Implements the CXACT3Util class.
//
//
//

#include "XACT3Util.h"


//////////////////////////////////////////////////////////////////////
// Constructors

CXACT3Util::CXACT3Util()
{
    m_pXACT3Engine = NULL;
    m_pWaveBank = NULL;
    m_pSoundBank = NULL;
    m_pWaveBankData = NULL;
    m_pSoundBankData = NULL;
}

CXACT3Util::~CXACT3Util()
{
}


//////////////////////////////////////////////////////////////////////
// Methods

bool CXACT3Util::Initialize()
{
    // Initialize COM
    HRESULT hr = ::CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hr))
        return false;

    // Create XACT3 engine
    hr = ::XACT3CreateEngine(XACT_FLAG_API_AUDITION_MODE, &m_pXACT3Engine);
    if (FAILED(hr))
        return false;

    // Initialize XACT3 engine
    XACT_RUNTIME_PARAMETERS xactRTParams = { 0 };
    xactRTParams.lookAheadTime = XACT_ENGINE_LOOKAHEAD_DEFAULT;
    hr = m_pXACT3Engine->Initialize(&xactRTParams);
    if (FAILED(hr))
        return false;

    return true;
}

void CXACT3Util::Terminate()
{
    if (m_pSoundBank != NULL)
        m_pSoundBank->Destroy();
    m_pSoundBank = NULL;
    if (m_pWaveBank != NULL)
        m_pWaveBank->Destroy();
    m_pWaveBank = NULL;

    if (m_pXACT3Engine != NULL) {
        m_pXACT3Engine->ShutDown();
        m_pXACT3Engine->Release();
    }

    if (m_pSoundBankData != NULL) {
        ::UnmapViewOfFile(m_pSoundBankData);
        m_pSoundBankData = NULL;
    }

    if (m_pWaveBankData != NULL) {
        ::UnmapViewOfFile(m_pWaveBankData);
        m_pWaveBankData = NULL;
    }

    ::CoUninitialize();
}

bool CXACT3Util::LoadWaveBank(LPCWSTR szFilePath)
{
    // Load file
    DWORD dwFileSize = 0;
    m_pWaveBankData = LoadBankFile(szFilePath, &dwFileSize);

    // Create wave bank
    HRESULT hr = m_pXACT3Engine->CreateInMemoryWaveBank(
        m_pWaveBankData, dwFileSize, 0, 0, &m_pWaveBank);
    if (FAILED(hr))
        return false;

    return true;
}

bool CXACT3Util::LoadSoundBank(LPCWSTR szFilePath)
{
    // Load file
    DWORD dwFileSize = 0;
    m_pSoundBankData = LoadBankFile(szFilePath, &dwFileSize);

    // Create wave bank
    HRESULT hr = m_pXACT3Engine->CreateSoundBank(
        m_pSoundBankData, dwFileSize, 0, 0, &m_pSoundBank);
    if (FAILED(hr))
        return false;

    return true;
}

void CXACT3Util::DoWork()
{
    if ((m_pXACT3Engine != NULL) && (m_pSoundBank != NULL)) {
        unsigned long state = 0;
        m_pSoundBank->GetState(&state);
        if (state && XACT_CUESTATE_PLAYING)
            m_pXACT3Engine->DoWork();
    }
}


//////////////////////////////////////////////////////////////////////
// Implementation

void* CXACT3Util::LoadBankFile(LPCWSTR szFilePath, DWORD* pdwFileSize)
{
    // Open file
    HANDLE fh = ::CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (fh == INVALID_HANDLE_VALUE)
        return NULL;

    // Get file size
    if (pdwFileSize == NULL)
        return NULL; // error
    *pdwFileSize = ::GetFileSize(fh, 0);

    // Create file mapping
    HANDLE mapFile = ::CreateFileMapping(fh, 0, PAGE_READONLY, 0, *pdwFileSize, 0);
    if (mapFile == NULL) {
        ::CloseHandle(fh);
        return 0;
    }

    // Map view of file
    void* p = ::MapViewOfFile(mapFile, FILE_MAP_READ, 0, 0, 0);

    // Clean up
    ::CloseHandle(mapFile);
    ::CloseHandle(fh);

    return p;
}