/*
 * AutoFish.h
 *
 *  Created on: 2018-06-15
 *      Author: ChengZu  Email: 1351606745@qq.com
 */

#ifndef AUTOFISH_H_
#define  AUTOFISH_H_
#include <windows.h>
#include <string>
#include "Robot.h"
#include "FFTClass.h"
#include "audio/AudioManager.h"
#include "audio/Timer.h"

class CPlaybackEventHandler: public IPlaybackEvent
{
public:
    virtual VOID OnPlaybackEnd()
    {
        //StopPlay();
    }
};

class AutoFish
{
public:
    AutoFish();
    void start();
    void stop();
    void uoun();
    void fish();
    bool hasFish();
    void setfloatColor();
    virtual ~AutoFish();
    
    int fishKey;
    int doFishTime;
    int perFishTime;

    int uounKey;
    int doUounTime;
    int perUounTime;
    
    Color floatColor;
    float floatColorSimilar;
    float soundData[10];
    float soundSimilar;
    
    int curShape;
    bool recCurShap; 
    
    bool unStop;
    Robot robot;
    Timer timer;
    POINT lastFindPoint;

    DWORD threadID;
    HANDLE hThread;
private:
    int lastFishTime;
    int lastUounTime;

    int count;
    short *data;
    double *WaveR;
    double *WaveI;
    FFTClass FFT;
    CPlaybackEventHandler playbackEventHandler;
    CAudioManager audioMgr;

    std::string dec2bin(int val);
    long long bin2dec(std::string A);

};

#endif /*  AUTOFISH_H_ */
