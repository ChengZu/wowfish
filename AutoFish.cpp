#include "AutoFish.h"
#include <cmath>
#include <vector>
#include<algorithm> 
#include "audio/Log.h"


DWORD WINAPI AutoFishThreadProc(LPVOID lpParam)
{
    AutoFish *autoFish = (AutoFish *)lpParam;
    while(true)
    {
        autoFish->fish();
        Sleep(10);
    }
    return 0;
}

AutoFish::AutoFish()
{
	fishKey = 50;
	doFishTime = 1500;
    perFishTime = 20000; 
    lastFishTime = 0;

    uounKey = 49;
    doUounTime = 5000;
    perUounTime = 10 * 60 * 1000;
    lastUounTime = 0;

    floatColor = Color(190, 55, 35);
    floatColorSimilar = 0.95;
    
    curShape = 301892;
    recCurShap = true; 
    
	float soundDatat[10] = {272.84, 321.172, 262.046, 176.161, 133.748, 85.7818, 96.2307, 72.4137, 72.8278, 55.8367};
	for(int i=0;i<10;i++)
		soundData[i] = soundDatat[i];
	soundSimilar = 0.6;
	
    count = 2048;
    data = new short[count * 2];
    WaveR = new double[count];
    WaveI = new double[count];


    hThread = CreateThread(NULL, 0, AutoFishThreadProc, this, 0, &threadID);
}


void AutoFish::start()
{
    unStop = true;
    lastUounTime = -999999999;
    timer.start();
    audioMgr.StartCapture();
}

void AutoFish::stop()
{
    unStop = false;
    audioMgr.StopCapture();
}

void AutoFish::uoun()
{
    if((timer.getElapsedTimeInMilliSec() - lastUounTime) > perUounTime)
    {
        robot.keyPress(uounKey);
        robot.keyRelease(uounKey);
        Sleep(doUounTime);
        lastUounTime = timer.getElapsedTimeInMilliSec();
    }
}
void AutoFish::fish()
{
    if(!unStop) return;
    uoun();
    robot.keyPress(fishKey);
    robot.keyRelease(fishKey);
    lastFishTime = timer.getElapsedTimeInMilliSec();
    Sleep(doFishTime);
    robot.captureScreen();
    POINT p = robot.findColor(floatColor, floatColorSimilar, lastFindPoint);
    //POINT p = robot.findColor(floatColor, floatColorSimilar);

	if(p.x > 0 && p.y > 0)
    {
        lastFindPoint = p;
        robot.mouseMove(p.x, p.y);
    	if(recCurShap) {
    		Sleep(1000);
    		if(robot.getCursorShap() != curShape){
    			lastFindPoint.x = robot.getImageW()/2;	
    			lastFindPoint.y = robot.getImageH()/2;	
    			return;
			}
		}
        while((timer.getElapsedTimeInMilliSec() - lastFishTime) < perFishTime && unStop)
        {
            if(hasFish() && unStop)
            {
                robot.mouseRightClick();
                Sleep(1000);
                break;
            }
            Sleep(100);
        }
    }
}
bool AutoFish::hasFish()
{

    std::vector<BYTE> bytedata = audioMgr.GetData();
    int size = bytedata.size();

    if(size < count * 4)
    {
        int asbsaas = 1;
        return false;
    }

    //for (int j = size - count * 4; j < size ; j+=2)
    for (int j = 0 ; j < count * 4 ; j += 2)
    {
        std::string WYN;
        std::vector<int> BYTES;

        BYTES.push_back(bytedata[j]);
        BYTES.push_back(bytedata[j + 1]);

        for(int i = BYTES.size() - 1; i >= 0; i--)
            WYN += dec2bin(BYTES[i]);
        data[j / 2] = bin2dec(WYN);
    }

    for(int i = 0; i < count; i++)
    {
        WaveR[i] = data[i * 2];
    }


    for(int i = 0; i < count; i++)
    {
        WaveR[i] = data[i * 2 + 1];
    }

    FFT.FFT(WaveR, WaveI, count, 1);

    float data11[300];

    for(int i = 1; i < 300; i++)
    {
        data11[i] = sqrt(WaveR[i] * WaveR[i] + WaveI[i] * WaveI[i]) / count * 2;
    }

    float data12[10];
    for(int i = 0; i < 10; i++)
    {
        float num = 0;
        for(int j = 30 * i; j < 30 * (i + 1); j++)
        {
            num += data11[j];
        }
        data12[i] = num;
    }
    float ttt = 0;
    for(int i = 0; i < 10; i++)
    {
        float min = soundData[i] < data12[i] ? soundData[i] : data12[i];
        float max = soundData[i] > data12[i] ? soundData[i] : data12[i];
        float ss = min / max;
        ttt += ss;
    }

    return ttt > (10 * soundSimilar);
}

void AutoFish::setfloatColor()
{
    POINT p = robot.getCursorPos();
    lastFindPoint = p;
    robot.captureScreen();
    floatColor = robot.getPixelColor(p.x, p.y);
}

std::string AutoFish::dec2bin(int val)
{
    std::string wyn;

    while(val > 0)
    {
        if(val % 2 == 0)wyn += "0";
        else wyn += "1";
        val /= 2;
    }

    while(wyn.size() < 8)wyn += "0";

    reverse(wyn.begin(), wyn.end());

    return wyn;
}

long long AutoFish::bin2dec(std::string A)
{
    long long d = 1;
    long long wyn = 0;
    for(int i = A.size() - 1; i >= 0; i--)
    {
        if(A[i] == '1')
        {
            wyn += d;
        }

        d *= 2;
    }

    return wyn;
}




AutoFish::~AutoFish()
{
    CloseHandle(hThread); // 关闭内核对象
    audioMgr.StopCapture();
    delete[] data;
    delete[] WaveR;
    delete[] WaveI;
}
