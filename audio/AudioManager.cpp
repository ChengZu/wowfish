#include "AudioManager.h"
#include "Log.h"

#ifndef _min
#define _min(a,b)            (((a) < (b)) ? (a) : (b))
#endif //_min

CAudioManager::CAudioManager(IPlaybackEvent *pEvent)
    : m_pPlaybackEventHandler(pEvent)
{
    m_pFormat = NULL;
    m_nOffset = 0;
}

CAudioManager::~CAudioManager()
{
    ClearData();
}

BOOL CAudioManager::StartCapture()
{
    BOOL bRet = m_capture.IsCapturing();
    if(!bRet)
    {
        if(m_capture.Initialize(this))
        {
            bRet = m_capture.Start();
        }
    }

    return bRet;
}

VOID CAudioManager::StopCapture()
{
    if(m_capture.IsCapturing())
    {
        m_capture.Stop();
        m_capture.Destroy();
    }
}

BOOL CAudioManager::IsCapturing() const
{
    return m_capture.IsCapturing();
}

BOOL CAudioManager::StartPlayback()
{
    BOOL bRet = m_render.IsRendering();

    if(!bRet)
    {
        if(m_render.Initialize(this))
        {
            bRet = m_render.Start();
        }
    }

    return bRet;
}

VOID CAudioManager::StopPlayback()
{
    if(m_render.IsRendering())
    {
        m_render.Stop();
        m_render.Destroy();
    }
}

BOOL CAudioManager::IsPlaybacking() const
{
    return m_render.IsRendering();
}

BOOL CAudioManager::CanPlay() const
{
    return !m_dataList.empty();
}

VOID CAudioManager::ClearData()
{
    LPBYTE p = (LPBYTE)m_pFormat;
    delete []p;
    m_pFormat = NULL;

    std::list<Audio_Data>::iterator itr = m_dataList.begin();
    while(itr != m_dataList.end())
    {
        Audio_Data &item = *itr;
        delete []item.pData;
        ++itr;
    }
    m_dataList.clear();
}


std::vector<BYTE> CAudioManager::GetData()
{

    std::vector<BYTE> data;
    if(m_dataList.size() <= 0)
        return data;
    std::list<Audio_Data>::iterator itor;
    itor = m_dataList.begin();
    while(itor != m_dataList.end())
    {
        Audio_Data &item = *itor;
        for (int i = 0; i < item.nDataLen; i++)
        {
            data.push_back(item.pData[i]);
        }
        itor++;
    }

    /*
    std::ofstream file("wave.wav", std::ios_base::binary | std::ios_base::trunc);
    if (file.is_open())
    {

        DWORD WaveHeaderSize = 36; // RIFF 12个字节, 格式块26个字节
        DWORD WaveFormatSize = 16; // 格式块内容长度
        file.write("RIFF", 4);
        // RIFF + 格式块 +数据块长度[8+dwSize]
        int fileLen = (data.size() + WaveHeaderSize);
        char *tmp = (char *)&fileLen;
        file.write((char *)&fileLen, 4);

        file.write("WAVE", 4);
        file.write("fmt ", 4);
        short int format = 1;
        file.write((char *)&WaveFormatSize, 4);
        file.write((char *)&format, 2);
        file.write((char *)&m_pFormat->nChannels, 2);
        file.write((char *)&m_pFormat->nSamplesPerSec, 4);
        file.write((char *)&m_pFormat->nAvgBytesPerSec, 4);
        file.write((char *)&m_pFormat->nBlockAlign, 2);
        file.write((char *)&m_pFormat->wBitsPerSample, 2);
        file.write("data", 4);
        int size = data.size();
        file.write((char *)&size, 4);
        file.write((char *)data.data(), data.size());
        file.close();
    }

    */
    return data;
}

VOID CAudioManager::OnCatpureStart(DWORD dwInterval)
{

}

VOID CAudioManager::OnCaptureStop()
{

}

VOID CAudioManager::OnAdjustCaptureFormat(WAVEFORMATEX *pFormat)
{
    ClearData();

    INT nDataLen = sizeof(WAVEFORMATEX) + pFormat->cbSize;
    LPBYTE pData = new BYTE[nDataLen];
    if(pData != NULL)
    {
        memcpy(pData, pFormat, nDataLen);
        m_pFormat = (WAVEFORMATEX *)pData;
    }
}

VOID CAudioManager::OnCatpureData(LPBYTE pData, INT nDataLen)
{
    //*
    //for my program, onlyCature 1s
    long int size = 0;
    std::list<Audio_Data>::iterator itor;
    itor = m_dataList.begin();
    while(itor != m_dataList.end())
    {
        Audio_Data &item = *itor;
        size += item.nDataLen;
        itor++;
    }
    size += nDataLen;
    while(size > m_pFormat->nAvgBytesPerSec)
    {
        Audio_Data &item = m_dataList.front();
        delete[] item.pData;
        size -= item.nDataLen;
        m_dataList.pop_front();
    }
    //end for my program

    //*/
    Audio_Data item;
    item.nDataLen = nDataLen;
    item.pData = new BYTE[nDataLen];
    if(item.pData != NULL)
    {
        memcpy(item.pData, pData, nDataLen);
        m_dataList.push_back(item);
    }
}


VOID CAudioManager::OnRenderStart()
{
    m_nOffset = 0;
    m_itrCurrent = m_dataList.begin();
}

VOID CAudioManager::OnRenderStop()
{

}

VOID CAudioManager::OnAdjustRenderFormat(WAVEFORMATEX *pFormat)
{
    if(pFormat == NULL) return;

    INT nDataLen1 = sizeof(WAVEFORMATEX) + pFormat->cbSize;
    INT nDataLen2 = sizeof(WAVEFORMATEX) + m_pFormat->cbSize;
    if(nDataLen1 == nDataLen2)
    {
        memcpy(pFormat, m_pFormat, nDataLen2);
    }
    else
    {
        memcpy(pFormat, m_pFormat, sizeof(WAVEFORMATEX));
    }
}

VOID CAudioManager::OnGetRenderData(LPBYTE pData, INT nDataLen)
{
    INT nCopyed = 0;
    INT nNeedCopy = nDataLen;
    BOOL bEnd(FALSE);
    if(m_itrCurrent == m_dataList.end())
    {
        m_itrCurrent = m_dataList.begin();
        m_nOffset = 0;
    }

    while(nCopyed < nDataLen )
    {
        Audio_Data &item = *m_itrCurrent;
        INT nItemLeftDataLen = item.nDataLen - m_nOffset;
        INT nToCopy = _min(nNeedCopy, nItemLeftDataLen);
        if(nToCopy > 0)
        {
            memcpy(pData + nCopyed, item.pData + m_nOffset, nToCopy);
        }
        m_nOffset += nToCopy;
        nCopyed += nToCopy;
        nNeedCopy -= nToCopy;

        if(m_nOffset >= item.nDataLen)
        {
            ++m_itrCurrent;
            m_nOffset = 0;

            if(m_itrCurrent == m_dataList.end())
            {
                bEnd = TRUE;
                break;
            }
        }
    }

    if(bEnd)
    {
        if(m_pPlaybackEventHandler != NULL)
        {
            m_pPlaybackEventHandler->OnPlaybackEnd();
        }
    }
}
