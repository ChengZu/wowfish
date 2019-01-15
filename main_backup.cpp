#include <string>
#include <cmath>
using namespace std;
#include "audio/AudioManager.h"
#include "resource.h"
#include "HotKey.h"
#include "AutoFish.h"

#include "FFTClass.h"

AutoFish autoFish;
Robot robot;
FFTClass FFT;

#define MAX_LOADSTRING 100
#define START_CAPTURE_ID 200
#define STOP_CAPTURE_ID 201
#define START_PLAY_ID 202
#define STOP_PLAY_ID 203
#define GEN_ID 204
#define MY_ICON_MESSAGE (WM_USER+1)
float soundData[10] = {272.84, 321.172, 262.046, 176.161, 133.748, 85.7818, 96.2307, 72.4137, 72.8278, 55.8367}; 

void myDraw2(HWND hWnd,HDC hdc);

int           	count1=2048;
short           *data;
double          *WaveR;
double          *WaveI;


NOTIFYICONDATA g_nid;
//g_nid.uCallbackMessage = MY_ICON_MESSAGE;


#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include<algorithm>

string dec2bin(int val)
{
    string wyn;

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

long long bin2dec(string A)
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

// 全局变量:
HINSTANCE hInst; // 当前实例
TCHAR szTitle[MAX_LOADSTRING]; // 标题栏文本
TCHAR szHello[MAX_LOADSTRING]; // 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING]; // 主窗口类名
HINSTANCE g_hInstance = NULL;
HWND g_hWndMain = NULL;

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

VOID StartRecord();
VOID StopRecord();
VOID StartPlay();
VOID StopPlay();



HWND g_hPrzycisk;

vector<int> Trans;



CPlaybackEventHandler g_playbackEventHandler;
CAudioManager g_audioMgr( &g_playbackEventHandler);

VOID GetData();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    g_hInstance = hInstance;

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。
    MSG msg;
    HACCEL hAccelTable;

    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WOWFISH, szWindowClass, MAX_LOADSTRING);
	
	CoInitialize(NULL);

    // 初始化全局字符串
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_WOWFISH);
    // 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

    CoUninitialize();

    return msg.wParam;
}

//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, (LPCSTR)IDI_BIG_ICON);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance,(LPCSTR)IDI_SMALL_ICON);

    return RegisterClassEx( & wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;

    hInst = hInstance; // 将实例句柄存储在全局变量中
    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

VOID CreateControlButtons(HWND hWndParent)
{
    RegisterAllHotKey(hWndParent);
    const INT nButtonWidth = 100;
    const INT nButtonHeight = 30;
    SetTimer(hWndParent, 2200, 500, NULL);//设定时器

    const DWORD dwButtonStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_TEXT;

    HWND hWndStartRecord = CreateWindow(TEXT("BUTTON"), TEXT("Start Capture"), dwButtonStyle, 20, 20, nButtonWidth, nButtonHeight, hWndParent, (HMENU) START_CAPTURE_ID, hInst, NULL);

    HWND hWndStopRecord = CreateWindow(TEXT("BUTTON"), TEXT("Stop Capture"), dwButtonStyle, 150, 20, nButtonWidth, nButtonHeight, hWndParent, (HMENU) STOP_CAPTURE_ID, hInst, NULL);

    HWND hWndStartPlay = CreateWindow(TEXT("BUTTON"), TEXT("Start Play"), dwButtonStyle, 20, 80, nButtonWidth, nButtonHeight, hWndParent, (HMENU) START_PLAY_ID, hInst, NULL);

    HWND hWndStopPlay = CreateWindow(TEXT("BUTTON"), TEXT("Stop Play"), dwButtonStyle, 150, 80, nButtonWidth, nButtonHeight, hWndParent, (HMENU) STOP_PLAY_ID, hInst, NULL);


    g_hPrzycisk = CreateWindowEx( 0, "BUTTON", "Generuj", WS_CHILD | WS_VISIBLE, 20, 520, 180, 25, hWndParent, (HMENU) GEN_ID, hInst, NULL );

}

VOID UpdateButtonStatus(BOOL bEnableStartCapture, BOOL bEnableStopCapture, BOOL bEnableStartPlay, BOOL bEnableStopPlay)
{
    HWND hWndStartRecord = GetDlgItem(g_hWndMain, START_CAPTURE_ID);
    HWND hWndStopRecord = GetDlgItem(g_hWndMain, STOP_CAPTURE_ID);
    HWND hWndStartPlay = GetDlgItem(g_hWndMain, START_PLAY_ID);
    HWND hWndStopPlay = GetDlgItem(g_hWndMain, STOP_PLAY_ID);

    EnableWindow(hWndStartRecord, bEnableStartCapture);
    EnableWindow(hWndStopRecord, bEnableStopCapture);
    EnableWindow(hWndStartPlay, bEnableStartPlay);
    EnableWindow(hWndStopPlay, bEnableStopPlay);
}

VOID StartRecord()
{
    if (g_audioMgr.StartCapture())
    {
        UpdateButtonStatus(FALSE, TRUE, FALSE, FALSE);
    }
}

VOID StopRecord()
{
    g_audioMgr.StopCapture();
    UpdateButtonStatus(TRUE, FALSE, TRUE, TRUE);
}

VOID StartPlay()
{
    if (g_audioMgr.CanPlay())
    {
        if (g_audioMgr.StartPlayback())
            UpdateButtonStatus(FALSE, FALSE, FALSE, TRUE);
    }
}

VOID StopPlay()
{
    g_audioMgr.StopPlayback();
    UpdateButtonStatus(TRUE, FALSE, TRUE, FALSE);
}

VOID GetData()
{
	Trans.clear();
	//Trans = g_audioMgr.GetData();
	//*
    std::vector<BYTE> data = g_audioMgr.GetData();
    int ss = data.size();   
    
    int min = ss;
    if(min > 2048 * 4) min = 2048 * 4 ;

    for (int j = 0; j < min ; j+=2)
    {
        string WYN;
        vector<int> BYTES;

        BYTES.push_back(data[j]);
        BYTES.push_back(data[j+1]);

        for(int i = BYTES.size() - 1; i >= 0; i--)
            WYN += dec2bin(BYTES[i]);
        Trans.push_back((double)bin2dec(WYN));
    }
   // */
}



//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
char gd = 'a';
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    int wmId = LOWORD(wParam);
    int wmEvent = HIWORD(wParam);
    int x = LOWORD(lParam);
    int y = HIWORD(lParam);
    RECT rect;
    GetClientRect(hWnd, &rect);

    rect.left=250;
    rect.top=0;

    Color color;
    switch (message)
    {
    case WM_CREATE:
    {
        g_hWndMain = hWnd;
        CreateControlButtons(hWnd);
        UpdateButtonStatus(TRUE, FALSE, FALSE, FALSE);
    }
    break;

    case WM_COMMAND:
        // 分析菜单选择:
        switch (wmId)
        {
        case GEN_ID:
			InvalidateRect(hWnd,&rect,1);

            break;
        case START_CAPTURE_ID:
        	autoFish.start();
            StartRecord();
            break;

        case STOP_CAPTURE_ID:
			autoFish.stop();
            StopRecord();
            break;

        case START_PLAY_ID:
            StartPlay();
            break;

        case STOP_PLAY_ID:
            StopPlay();
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_KEYDOWN:
        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        // TODO: 在此添加任意绘图代码...
 
		//myDraw2(hWnd,hdc);
        
        EndPaint(hWnd, &ps);
        break;
    //case WM_ERASEBKGND:
		//break;
    case WM_TIMER:

        InvalidateRect(hWnd,&rect,1);
        break;
    case WM_HOTKEY:
    	UnregisterAllHotKey(hWnd);
        switch (wmId)
        {
        case HK_ID_F9:
        	do{
        		
        		POINT p=robot.getCursorPos();
        		robot.captureScreen();
        		autoFish.floatColor=robot.getPixelColor(p.x,p.y);
            	
			}while(false);
        	break;
        case HK_ID_F10:
        	//autoFish.start();
            StartRecord();

            break;
        case HK_ID_F11:
        	//autoFish.stop();
            StopRecord();
            break;
        default:
            break;
        }
        

        gd++;
        SetWindowText(hWnd, TEXT(&gd));

        RegisterAllHotKey(hWnd);
        
        break;
    case WM_DESTROY:
        UnregisterAllHotKey(hWnd);
        if (g_audioMgr.IsCapturing()) g_audioMgr.StopCapture();
        if (g_audioMgr.IsPlaybacking()) g_audioMgr.StopPlayback();
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


void myDraw2(HWND hWnd,HDC hdc)
{
	GetData();
	if(Trans.size()<(count1*2))return;
    data=new short[count1 * 2];
    WaveR=new double[count1];
    WaveI=new double[count1];
    for(int ii=0;ii<count1*2;ii++){
    	data[ii]=Trans[ii];
	}
	
	
    RECT rect;
    HDC memDC;
    HBITMAP memBitmap;
    HBITMAP hOldBitmap;
    GetClientRect(hWnd,&rect);
    memDC = CreateCompatibleDC(hdc);
    memBitmap = CreateCompatibleBitmap(hdc, rect.right,rect.bottom);   
    hOldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);


	// the whole client

    HBRUSH hBrush=CreateSolidBrush(RGB(255,255,255));
    SelectObject(memDC,hBrush);
    FillRect(memDC, &rect, hBrush);
    HPEN pen = CreatePen(PS_SOLID,1,RGB(0,0,255));
    SelectObject(memDC,pen);


	
	
	int width=800;
	int height=500;
    long    zerolevel= height- 30;
    int     i, dx, freq;
    double  ts=(double)width / count1 * 2;

    Rectangle(memDC, 0, 0, width, height);
    //pen = CreatePen(PS_SOLID,1,RGB(255,255,0));
    MoveToEx(memDC, 0, zerolevel,NULL);
	LineTo(memDC, width, zerolevel);
 

    memset(WaveI, 0, count1 * sizeof(double));

    for(i=0; i < count1; i++)
    {
        WaveR[i]=data[i * 2];
    }
/*
    FFT.FFT(WaveR, WaveI, count1, 1);
    //pen = CreatePen(PS_SOLID,1,RGB(186,186,186));
    MoveToEx(memDC, 0, zerolevel - sqrt(WaveR[0] * WaveR[0] + WaveI[0] * WaveI[0]) / count1 * 2,NULL);
    for(i=1; i < count1 / 2; i++)
    {
        LineTo(memDC, i * ts, zerolevel - sqrt(WaveR[i] * WaveR[i] + WaveI[i] * WaveI[i]) / count1 * 2);
        
    }

    memset(WaveI, 0, count1 * sizeof(double));
    */

    for(i=0; i < count1; i++)
    {
        WaveR[i]=data[i * 2 + 1];
    }

    FFT.FFT(WaveR, WaveI, count1, 1);
    //pen = CreatePen(PS_SOLID,1,RGB(0,186,186));
    float data11[300];
    
    MoveToEx(memDC, 0, zerolevel - sqrt(WaveR[0] * WaveR[0] + WaveI[0] * WaveI[0]) / count1 * 2,NULL);
    for(i=1; i < count1 / 2; i++)
    {
    	if(i<300)data11[i]=sqrt(WaveR[i] * WaveR[i] + WaveI[i] * WaveI[i]) / count1 * 2;
        LineTo(memDC, i * ts, zerolevel - sqrt(WaveR[i] * WaveR[i] + WaveI[i] * WaveI[i]) / count1 * 2);
    }
    float data12[10];
    for(int i=0;i<10;i++){
    	float num=0;
    	for(int j=30*i;j<30*(i+1);j++){
    		num+=data11[j];
		}
		data12[i]=num;
	}
	float ttt=0;
	for(int i=0;i<10;i++){
		float ds=abs(soundData[i]-data12[i]);
		float ss=ds*2/(soundData[i]+data12[i]);
		ttt+=(1-ss);
	}
if(ttt>0.6)SetWindowText(hWnd, TEXT("识别到声音"));
Log::out("ttttttttttt");
Log::out(ttt);

    DeleteObject(hBrush);
    DeleteObject(pen);

    BitBlt(hdc, 300, 0, rect.right,rect.bottom,memDC,0,0,SRCCOPY);

    SelectObject(memDC, hOldBitmap);  
    DeleteObject(memBitmap);  
    DeleteDC(memDC); 
    delete[] data;
    delete[] WaveR;
    delete[] WaveI;
}

