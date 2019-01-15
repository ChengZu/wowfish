#include "resource.h"
#include <string>
#include <cmath>
#include <time.h>
#include "HotKey.h"
#include "AutoFish.h"
#include "Config.h"
#include "tool/GetMac.h"
#include "tool/md5.h"
#include "qrencode/qrencode.h"
#include <winhttp.h>
using namespace std;



// 全局变量:
HINSTANCE hInst; // 当前实例
TCHAR szTitle[MAX_LOADSTRING]; // 标题栏文本
TCHAR szHello[MAX_LOADSTRING]; // 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING]; // 主窗口类名
HINSTANCE g_hInstance = NULL;
HWND g_hWndMain = NULL;
NOTIFYICONDATA nid;     //托盘属性
HMENU hMenu;            //托盘菜单
HBITMAP QrcodeBmp; 
string MacId;

AutoFish autoFish;
Config config((char*)"wowfish.ini");
INT RegTimeDs=0;
INT RegTime=0;
BOOL HasReadReg=FALSE;
BOOL HasOnConnect=FALSE;
INT ReTryConnect=0;
BOOL CanUse=FALSE;


ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
VOID StartRun();
VOID StopRun();
VOID UpdateButtonStatus(BOOL bEnableStartRun, BOOL bEnableStopRun);
VOID DrawQrcode(HWND hWnd);
VOID DrawFloat(HWND hWnd);
VOID IintQrCodeBitmap(HWND hWnd);
DWORD WINAPI GetRegInfoThreadProc(LPVOID lpParam);
VOID InitTray(HINSTANCE hInstance, HWND hWnd);
VOID ShowTrayMsg(string msg);

VOID SetStateMsg(string msg){
	string str = "状态：";
	str += msg; 
	HWND txt=GetDlgItem(g_hWndMain, STATE_TEXT_ID);
	SetWindowText(txt, str.data());
}

VOID MyTimer(){
	BOOL canUset=CanUse;
	time_t timep;
	time (&timep);
	timep += RegTimeDs;
	if(timep<RegTime){
		CanUse=true;
		if(canUset) return;//激活状态 确认激活 
		//非激活状态，确认激活 
		char tmp[64];
    	time_t time2=RegTime;
    	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S",localtime(&time2) );
    	string str = tmp;
    	SetStateMsg("激活，有效期至：" + str);
    	UpdateButtonStatus(TRUE, FALSE);
		ReTryConnect = 0; 
		return; 
	}
	CanUse=FALSE;
    StopRun();
    
    if(HasReadReg){
		SetStateMsg("过期，支付宝扫码激活，点击二维码刷新");
		return;
	}
    
	if(ReTryConnect < 20){
		SetStateMsg("连接服务器中...");
		if(HasOnConnect)return;
		DWORD threadID;
		HANDLE hThread = CreateThread(NULL, 0, GetRegInfoThreadProc, NULL, 0, &threadID);
		CloseHandle(hThread); // 关闭内核对象
		ReTryConnect++; 
		return;
	}else if(ReTryConnect >= 20){
		SetStateMsg("连接服务器失败，点击二维码重试");
		return;
	}

	SetStateMsg("出错，点击二维码刷新状态");
} 



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
    wcex.hIcon = LoadIcon(wcex.hInstance, (LPCSTR)IDI_BIG_ICON);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(240,240,240));
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCSTR)IDI_SMALL_ICON);

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
    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX, CW_USEDEFAULT, 0, 360, 320, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}
VOID UpdateButtonStatus(BOOL bEnableStartRun, BOOL bEnableStopRun)
{
    EnableWindow(GetDlgItem(g_hWndMain, START_RUN_ID), bEnableStartRun);
    EnableWindow(GetDlgItem(g_hWndMain, STOP_RUN_ID), bEnableStopRun);
    EnableMenuItem(hMenu, ID_TRAY_START, !bEnableStartRun);  //MF_ENABLED,MF_GRAYED
    EnableMenuItem(hMenu, ID_TRAY_STOP, !bEnableStopRun);
}

VOID OnWindowsCreate(HWND hWndParent)
{
    RegisterAllHotKey(hWndParent);
    IintQrCodeBitmap(hWndParent);
    InitTray(hInst, hWndParent);
    config.read(autoFish);
    const INT nButtonWidth = 100;
    const INT nButtonHeight = 35;
    SetTimer(hWndParent, 2200, 500, NULL);//设定时器
    

    const DWORD dwButtonStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_TEXT;

    HWND hWndStartRun = CreateWindow(TEXT("BUTTON"), TEXT("启动(F9)"), dwButtonStyle, 240, 20, nButtonWidth, nButtonHeight, hWndParent, (HMENU) START_RUN_ID, hInst, NULL);

    HWND hWndStopRun  = CreateWindow(TEXT("BUTTON"), TEXT("停止(F11)"), dwButtonStyle, 240, 75, nButtonWidth, nButtonHeight, hWndParent, (HMENU) STOP_RUN_ID, hInst, NULL);

    HWND hWndSet	  = CreateWindow(TEXT("BUTTON"), TEXT("设置"), dwButtonStyle, 240, 130, nButtonWidth, nButtonHeight, hWndParent, (HMENU) PGM_SET_ID, hInst, NULL);
 
    HWND hState 	  = CreateWindow("static", TEXT("状态：未激活，使用支付宝扫码免费激活"), WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 230, 360, 25, hWndParent, (HMENU)STATE_TEXT_ID,hInst,NULL);  

    HWND hReadme	  = CreateWindow("static", TEXT("使用说明：Ctrl+A取鱼漂色，F9启动，F11停止"), WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 260, 360, 25, hWndParent, (HMENU)README_TEXT_ID, hInst, NULL);  
 
 
 
	RECT rect;  
	//获取窗体尺寸  
	GetWindowRect(hWndParent,&rect);
	rect.left=(GetSystemMetrics(SM_CXSCREEN)-rect.right)/2;  
	rect.top=(GetSystemMetrics(SM_CYSCREEN)-rect.bottom)/2;  
	//设置窗体位置  
	SetWindowPos(hWndParent, HWND_TOP, rect.left, rect.top, rect.right, rect.bottom, SWP_NOSIZE);  
	StopRun();

}



VOID StartRun()
{
	if(CanUse){
		autoFish.start();
    	UpdateButtonStatus(FALSE, TRUE);
	}
}

VOID StopRun()
{
	autoFish.stop();
	if(CanUse){
		UpdateButtonStatus(TRUE, FALSE);
	}else{
		UpdateButtonStatus(FALSE, FALSE);
	}
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

    rect.left = 250;
    rect.top = 0;

    Color color;
    switch (message)
    {
    case WM_CREATE:
        g_hWndMain = hWnd;
        OnWindowsCreate(hWnd);
    break;

    case WM_COMMAND:
        // 分析菜单选择:
        switch (wmId)
        {

        case START_RUN_ID:
            StartRun();
            break;
        case STOP_RUN_ID:
            StopRun();
            break;
        case PGM_SET_ID:
            MessageBox(hWnd, "请手动修改配置文件wowfish.ini", "UI开发中", 0);
        break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_LBUTTONDOWN:
    	if(x>20 && x< 220 && y>20 && y<220){
    		
    		ReTryConnect=0;
    		HasReadReg=FALSE; 
		}
		
    	break;
    case WM_KEYDOWN:
        break;
    
    case WM_TIMER:
        MyTimer();
        //KillTimer(hWnd, wParam);
        break;
    case WM_HOTKEY:
    	PlaySound((LPCTSTR)IDR_TIP_WAVE,hInst,SND_RESOURCE|SND_ASYNC);
        UnregisterAllHotKey(hWnd);
        switch (wmId)
        {
        case HK_ID_CQ:
            autoFish.robot.keyPress(VK_CONTROL);
            autoFish.robot.keyPress(65);
            autoFish.robot.keyRelease(65);
            autoFish.robot.keyRelease(VK_CONTROL);

            autoFish.setfloatColor();
            config.save(autoFish);
			DrawFloat(hWnd);

            break;
        case HK_ID_F9:
            autoFish.robot.keyDownUp(VK_F10);
            StartRun();

            break;
        case HK_ID_F11:
            autoFish.robot.keyDownUp(VK_F11);
            StopRun();
            break;
        default:
            break;
        }

        RegisterAllHotKey(hWnd);

        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);

		DrawFloat(hWnd);
		DrawQrcode(hWnd);
		
        EndPaint(hWnd, &ps);
        break;
    //case WM_ERASEBKGND:
    //break;
    case WM_TRAY:
        switch(lParam)
        {
        case WM_RBUTTONDOWN:
        {
            //获取鼠标坐标
            POINT pt;
            GetCursorPos(&pt);
            //解决在菜单外单击左键菜单不消失的问题
            SetForegroundWindow(hWnd);

            //显示并获取选中的菜单

            int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD, pt.x, pt.y, 0, hWnd,  NULL);
            if(cmd == ID_TRAY_SHOW)
                ShowWindow(hWnd, SW_SHOW);
            if(cmd == ID_TRAY_START)
                StartRun();
            if(cmd == ID_TRAY_STOP)
                StopRun();
            if(cmd == ID_TRAY_EXIT)
                PostMessage(hWnd, WM_DESTROY, wParam, lParam);
        }
        break;
        case WM_LBUTTONDOWN:

            break;
        case WM_LBUTTONDBLCLK:
            ShowWindow(hWnd, SW_SHOW);
            break;
        }
        break;
    case WM_CLOSE:
        ShowWindow(hWnd, SW_HIDE);
        break;
    case WM_DESTROY:
        Shell_NotifyIcon(NIM_DELETE, &nid);
        UnregisterAllHotKey(hWnd);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    if (message == WM_TASKBAR_CREATED)
    {
        //系统Explorer崩溃重启时，重新加载托盘
        Shell_NotifyIcon(NIM_ADD, &nid);
    }
    return 0;
}


VOID DrawFloat(HWND hWnd){
	HDC hdcOkno = GetDC( hWnd );
    HBRUSH PedzelZiel, Pudelko;
    HPEN OlowekCzerw, Piornik;
    PedzelZiel = CreateSolidBrush( RGB(autoFish.floatColor.R, autoFish.floatColor.G, autoFish.floatColor.B) );
    OlowekCzerw = CreatePen( PS_SOLID, 1, RGB(57, 178, 255) );
    Pudelko = ( HBRUSH ) SelectObject( hdcOkno, PedzelZiel );
    Piornik = ( HPEN ) SelectObject( hdcOkno, OlowekCzerw );
    Rectangle( hdcOkno, 240, 185, 340, 220 );
    SelectObject( hdcOkno, Pudelko );
    SelectObject( hdcOkno, Piornik );
    DeleteObject( OlowekCzerw );
    DeleteObject( PedzelZiel );
    ReleaseDC( hWnd, hdcOkno );
}
VOID DrawQrcode(HWND hWnd){
	HDC hdc = GetDC( hWnd );
	HDC memDC = CreateCompatibleDC(hdc);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(memDC, QrcodeBmp);
	BitBlt(hdc, 20, 20, 200, 200, memDC, 0, 0, SRCCOPY);
	SelectObject(memDC, hOldBitmap);
	DeleteObject(memDC);
	ReleaseDC(hWnd, hdc);
}


VOID IintQrCodeBitmap(HWND hWnd){
	string          url = "https://www.czios.com/wowfish/pay.php?code=";
	char			lpszMac[128] = {0};
    if(GetMacByCmd(lpszMac, 128)){
   		MD5 md5(lpszMac);
   		MacId = md5.md5();
   		url+=MacId;
	}
	unsigned int    unWidth, x, y, unWidthAdjusted, unDataBytes;
	unsigned char*  pRGBData, *pSourceData, *pDestData;
	QRcode*         pQRC;
	if (pQRC = QRcode_encodeString(url.data(), 1, QR_ECLEVEL_M, QR_MODE_8, 1))
	{
		unWidth = pQRC->width;
		unWidthAdjusted = unWidth * 3;//一行字节数 
		if (unWidthAdjusted % 4)
			unWidthAdjusted = (unWidthAdjusted / 4 + 1) * 4;
		unDataBytes = unWidthAdjusted * unWidth;
		// Allocate pixels buffer
		pRGBData = (unsigned char*)malloc(unDataBytes);

		// Preset to white
		memset(pRGBData, 0xFF, unDataBytes);
		// Prepare bmp headers
		BITMAPFILEHEADER kFileHeader;
		kFileHeader.bfType = 0x4d42;  // "BM"
		kFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + unDataBytes;
		kFileHeader.bfReserved1 = 0;
		kFileHeader.bfReserved2 = 0;
		kFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		BITMAPINFOHEADER kInfoHeader;
		kInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
		kInfoHeader.biWidth = unWidth ;
		kInfoHeader.biHeight = unWidth;
		kInfoHeader.biPlanes = 1;
		kInfoHeader.biBitCount = 24;
		kInfoHeader.biCompression = BI_RGB;
		kInfoHeader.biSizeImage = 0;
		kInfoHeader.biXPelsPerMeter = 0;
		kInfoHeader.biYPelsPerMeter = 0;
		kInfoHeader.biClrUsed = 0;
		kInfoHeader.biClrImportant = 0;
			
		
		// Convert QrCode bits to bmp pixels
		pSourceData = pQRC->data;
		for(y = unWidth ; y > 0; y--)
		{
			pDestData = pRGBData + unWidthAdjusted * (y - 1);
			for (x = 0; x < unWidth; x++)
			{
				if (*pSourceData & 1)
				{
					//this is qrcode color default black
					*(pDestData + 0) = 0x00;
					*(pDestData + 1) = 0x00;
					*(pDestData + 2) = 0x00;
				}
				pDestData += 3;
				pSourceData++;
			}
		}
		HDC hdc = GetDC( hWnd );
		DeleteObject(QrcodeBmp);
		HDC hCompatibleDC = CreateCompatibleDC(hdc);
		QrcodeBmp = CreateCompatibleBitmap(GetDC( hWnd ), 200, 200);
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hCompatibleDC, QrcodeBmp);
	
		StretchDIBits(hCompatibleDC, 0, 0, 200, 200, 0, 0, kInfoHeader.biWidth, kInfoHeader.biHeight, pRGBData, (BITMAPINFO*)&kInfoHeader, DIB_RGB_COLORS, SRCCOPY);

		HBITMAP hAliPaybmp = LoadBitmap(hInst,MAKEINTRESOURCE(IDB_ALIPAY_BITMAP)); 
		HDC hmemDC = CreateCompatibleDC(hdc);
		SelectObject(hmemDC, hAliPaybmp);
		BitBlt(hCompatibleDC, 80, 80, 40, 40, hmemDC, 0, 0, SRCCOPY);
		
		DeleteObject(hAliPaybmp);
		DeleteObject(hmemDC);
		
		SelectObject(hCompatibleDC, hOldBitmap);
		DeleteObject(hCompatibleDC);
    	ReleaseDC(hWnd, hdc);
    	//SendMessage(hwndStatic, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)QrcodeBmp); 
    	
		// Free data
		free(pRGBData);
		QRcode_free(pQRC);
	}
}

DWORD WINAPI GetRegInfoThreadProc(LPVOID lpParam)
{
	HasOnConnect = TRUE;
	DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    LPSTR pszOutBuffer = NULL;
    HINTERNET  hSession = NULL,
               hConnect = NULL,
               hRequest = NULL;

    BOOL  bResults = FALSE;

    hSession=WinHttpOpen(L"User-Agent: wowfishclient", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME,WINHTTP_NO_PROXY_BYPASS, 0);

    if(hSession)
    {
        hConnect=WinHttpConnect(hSession,L"www.czios.com", INTERNET_DEFAULT_HTTPS_PORT, 0);
    }

    if(hConnect)
    {
        hRequest=WinHttpOpenRequest(hConnect, L"POST",L"/wowfish/reg.php",L"HTTP/1.1", WINHTTP_NO_REFERER,WINHTTP_DEFAULT_ACCEPT_TYPES,WINHTTP_FLAG_SECURE);
    }
    
    
    LPCWSTR header=L"Content-type: application/x-www-form-urlencoded";


    SIZE_T len = lstrlenW(header);

    WinHttpAddRequestHeaders(hRequest,header,DWORD(len), WINHTTP_ADDREQ_FLAG_ADD);

    if(hRequest)
    {

    std::string data="code="+ MacId;


    const void *ss=(const char *)data.c_str();

    bResults=WinHttpSendRequest(hRequest, 0, 0,const_cast<void *>(ss),data.length(), data.length(), 0 );

        ////bResults=WinHttpSendRequest(hRequest,WINHTTP_NO_ADDITIONAL_HEADERS, 0,WINHTTP_NO_REQUEST_DATA, 0, 0, 0 );
    }

    if(bResults)
    {
        bResults=WinHttpReceiveResponse(hRequest,NULL);

    }
    
    if(bResults)
    {
        do
        {
            // Check for available data.

             dwSize = 0;

             if (!WinHttpQueryDataAvailable( hRequest, &dwSize))
             {
                 printf( "Error %u in WinHttpQueryDataAvailable.\n",GetLastError());

                 break;
             }

             if (!dwSize)
                 break;

              pszOutBuffer = new char[dwSize+1];

              if (!pszOutBuffer)
              {
                   printf("Out of memory\n");
                break;
              }

               ZeroMemory(pszOutBuffer, dwSize+1);

               if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer,  dwSize, &dwDownloaded))
               {
                     printf( "Error %u in WinHttpReadData.\n", GetLastError());
               }

               if (!dwDownloaded)
                   break;

        } while (dwSize > 0);
        
        vector<string> vs = config.split(pszOutBuffer,',');
        
        if(vs.size()>1){
        	int t1 = config.str2int(vs[0]);
        	time_t timep;
			time (&timep);
        	RegTimeDs = t1 -timep;
        	RegTime = config.str2int(vs[1]);
		}
       
        HasReadReg=TRUE;
        delete [] pszOutBuffer;
    }
    
    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);
    HasOnConnect = FALSE;
    return 0;
}

//实例化托盘
VOID InitTray(HINSTANCE hInstance, HWND hWnd)
{
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = IDI_TRAY;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
    nid.uCallbackMessage = WM_TRAY;
    nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL_ICON));
    lstrcpy(nid.szTip, szTitle);

    hMenu = CreatePopupMenu();//生成托盘菜单
    //为托盘菜单添加选项
    AppendMenu(hMenu, MF_STRING, ID_TRAY_SHOW, TEXT("显示"));
    AppendMenu(hMenu, MF_STRING, ID_TRAY_START, TEXT("启动"));
    AppendMenu(hMenu, MF_STRING, ID_TRAY_STOP, TEXT("停止"));
    AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, TEXT("退出"));

    Shell_NotifyIcon(NIM_ADD, &nid);
}

//演示托盘气泡提醒
VOID ShowTrayMsg(string msg)
{
    lstrcpy(nid.szInfoTitle, szTitle);
    lstrcpy(nid.szInfo, msg.data());
    nid.uTimeout = 1000;
    Shell_NotifyIcon(NIM_MODIFY, &nid);
}


