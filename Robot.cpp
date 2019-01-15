#include <cmath>
#include "Robot.h"
#include "tool/md5.h"
#include "audio/Log.h"
#include <string>

_Color::_Color()
{
}
_Color::_Color(unsigned char R1, unsigned char G1, unsigned char B1)
{
    R = R1;
    G = G1;
    B = B1;
    A = 1;
}
_Color::_Color(unsigned char R1, unsigned char G1, unsigned char B1, unsigned char A1)
{
    R = R1;
    G = G1;
    B = B1;
    A = A1;
}

Robot::Robot()
{
    imageBuff = NULL;
}

VOID Robot::captureScreen()
{
    if(onCaptureScreen)return;
    onCaptureScreen = true;
    HDC hdcScreen;
    HDC hdcMemDC = NULL;
    HBITMAP hbmScreen = NULL;
    BITMAP bmpScreen;
    do
    {
        // Retrieve the handle to a display device context for the client
        // area of the window.
        hdcScreen = GetDC(NULL);

        // Create a compatible DC which is used in a BitBlt from the window DC
        hdcMemDC = CreateCompatibleDC(hdcScreen);

        if(!hdcMemDC)
        {
            //MessageBox(hWnd, TEXT("CreateCompatibleDC has failed"), TEXT("Failed"), MB_OK);
            onCaptureScreen = false;
            break;

        }

        // Create a compatible bitmap from the Window DC
        hbmScreen = CreateCompatibleBitmap(hdcScreen, GetSystemMetrics (SM_CXSCREEN), GetSystemMetrics (SM_CYSCREEN));

        if(!hbmScreen)
        {
            //MessageBox(hWnd, TEXT("CreateCompatibleBitmap Failed"), TEXT("Failed"), MB_OK);
            onCaptureScreen = false;
            break;
        }

        // Select the compatible bitmap into the compatible memory DC.
        SelectObject(hdcMemDC, hbmScreen);

        // Bit block transfer into our compatible memory DC.
        if(!BitBlt(hdcMemDC,
                   0, 0,
                   GetSystemMetrics (SM_CXSCREEN), GetSystemMetrics (SM_CYSCREEN),
                   hdcScreen,
                   0, 0,
                   SRCCOPY))
        {
            //MessageBox(hWnd, TEXT("BitBlt has failed"), TEXT("Failed"), MB_OK);
            onCaptureScreen = false;
            break;
        }

        // Get the BITMAP from the HBITMAP
        GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);
        imageW = bmpScreen.bmWidth;
        imageH = bmpScreen.bmHeight;

        BITMAPFILEHEADER   bmfHeader;
        BITMAPINFOHEADER   bi;

        bi.biSize = sizeof(BITMAPINFOHEADER);
        bi.biWidth = bmpScreen.bmWidth;
        bi.biHeight = bmpScreen.bmHeight;
        bi.biPlanes = 1;
        bi.biBitCount = 32;
        bi.biCompression = BI_RGB;
        bi.biSizeImage = 0;
        bi.biXPelsPerMeter = 0;
        bi.biYPelsPerMeter = 0;
        bi.biClrUsed = 0;
        bi.biClrImportant = 0;

        DWORD dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

        // Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that
        // call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc
        // have greater overhead than HeapAlloc.
        HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
        char *lpbitmap = (char *)GlobalLock(hDIB);

        // Gets the "bits" from the bitmap and copies them into a buffer
        // which is pointed to by lpbitmap.
        GetDIBits(hdcMemDC, hbmScreen, 0,
                  (UINT)bmpScreen.bmHeight,
                  lpbitmap,
                  (BITMAPINFO *)&bi, DIB_RGB_COLORS);

        lock = true;

        delete[] imageBuff;
        imageBuff = new unsigned char[4 * imageW * imageH];


        for(unsigned y = 0; y < imageH; y++)
        {
            for(unsigned x = 0; x < imageW; x++)
            {
                //pixel start byte position in the BMP
                unsigned bmpos =  (imageH - y - 1) * imageW * 4 + 4 * x;
                //pixel start byte position in the new raw image
                unsigned newpos = 4 * y * imageW + 4 * x;
                imageBuff[newpos + 0] = lpbitmap[bmpos + 2]; //R
                imageBuff[newpos + 1] = lpbitmap[bmpos + 1]; //G
                imageBuff[newpos + 2] = lpbitmap[bmpos + 0]; //B
                imageBuff[newpos + 3] = lpbitmap[bmpos + 3]; //A
            }
        }

        lock = false;
        /*
        // A file is created, this is where we will save the screen capture.
        HANDLE hFile = CreateFile(TEXT("captureqwsx.bmp"),
                                  GENERIC_WRITE,
                                  0,
                                  NULL,
                                  CREATE_ALWAYS,
                                  FILE_ATTRIBUTE_NORMAL, NULL);


        // Add the size of the headers to the size of the bitmap to get the total file size
        DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

        //Offset to where the actual bitmap bits start.
        bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

        //Size of the file
        bmfHeader.bfSize = dwSizeofDIB;

        //bfType must always be BM for Bitmaps
        bmfHeader.bfType = 0x4D42; //BM

        DWORD dwBytesWritten = 0;
        WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
        WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
        WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

        //Close the handle for the file that was created
        CloseHandle(hFile);

        */
        //Unlock and Free the DIB from the heap
        GlobalUnlock(hDIB);
        GlobalFree(hDIB);


    }
    while(FALSE);
    //Clean up

    DeleteObject(hbmScreen);
    DeleteObject(hdcMemDC);
    ReleaseDC(NULL, hdcScreen);
    onCaptureScreen = false;

}


VOID Robot::delay(int ms)
{
    Sleep(ms);
}


Color Robot::getPixelColor(int x, int y)
{
    Color color;
    if(lock)return color;
    if(imageBuff == NULL)
        captureScreen();

    if(x < 0 || y < 0 || x >= imageW || y >= imageH) return color;

    int index = 4 * imageW * y + 4 * x;
    color.R = imageBuff[index];
    color.G = imageBuff[index + 1];
    color.B = imageBuff[index + 2];
    return color;
}
POINT Robot::findColor(Color color, float similar)
{
    POINT p = { -1, -1};
    for(int i = 0; i < imageH; i++)
    {
        for(int j = 0; j < imageW; j++)
        {
            if(isColorSimilar(color, getPixelColor(j, i), similar))
            {
                p.x = j;
                p.y = i;
                return p;
            }
        }
    }
    return p;
}
POINT Robot::findColor(Color color, float similar, POINT point)
{
    POINT p = { -1, -1};
    int x1 = point.x, y1 = point.y, x2 = point.x, y2 = point.y;
    bool leftEnd(0), rightEnd(0), downEnd(0), upEnd(0);
    bool hasFind = isColorSimilar(color, getPixelColor(point.x, point.y), similar);
    if(hasFind)return point;
    while(!hasFind)
    {
        x1--, y1--, x2++, y2++;
        if(x1 < 0)
        {
            x1 = 0;
            leftEnd = true;
        }
        if(y1 < 0)
        {
            y1 = 0;
            upEnd = true;
        }
        if(x2 >= imageW)
        {
            x2 = imageW - 1;
            rightEnd = true;
        }
        if(y2 >= imageH)
        {
            y2 = imageH - 1;
            downEnd = true;
        }

        if(upEnd && leftEnd && downEnd && rightEnd)
            break;
        int w = x2 - x1 + 1;
        int h = y2 - y1 + 1;
        //…œ––…®√Ë
        if(!upEnd)
        {
            p.y = y1;
            for(int i = 0; i < w; i++)
            {
                p.x = x1 + i;
                hasFind = isColorSimilar(color, getPixelColor(p.x, p.y), similar);
                if(hasFind) return p;
            }
        }
        //◊Û¡–…®√Ë
        if(!leftEnd)
        {
            p.x = x1;
            for(int i = 0; i < h; i++)
            {
                p.y = y1 + i;
                hasFind = isColorSimilar(color, getPixelColor(p.x, p.y), similar);
                if(hasFind) return p;
            }
        }

        // œ¬––…®√Ë
        if(!downEnd)
        {
            p.y = y2;
            for(int i = 0; i < w; i++)
            {
                p.x = x1 + i;
                hasFind = isColorSimilar(color, getPixelColor(p.x, p.y), similar);
                if(hasFind) return p;
            }
        }
        //”“¡–…®√Ë
        if(!rightEnd)
        {
            p.x = x2;
            for(int i = 0; i < h ; i++)
            {
                p.y = y1 + i;
                hasFind = isColorSimilar(color, getPixelColor(p.x, p.y), similar);
                if(hasFind) return p;
            }
        }
    }
    return p;
}

VOID Robot::keyPress(int keycode)
{
    keybd_event(keycode, 0, 0, 0);

}
VOID Robot::keyRelease(int keycode)
{
    keybd_event(keycode, 0, KEYEVENTF_KEYUP, 0);
}

VOID Robot::keyDownUp(int keycode)
{
    keybd_event(keycode, 0, 0, 0);
    keybd_event(keycode, 0, KEYEVENTF_KEYUP, 0);
}

VOID Robot::mouseMove(int x, int y)
{
    SetCursorPos(x, y);
}

POINT Robot::getCursorPos()
{
    POINT p;
    GetCursorPos(&p);
    return p;
}


int Robot::getCursorShap()
{	
	CURSORINFO ci;
	ci.cbSize=sizeof(CURSORINFO);
	GetCursorInfo(&ci);
    ICONINFO icon;
    GetIconInfo(ci.hCursor,&icon);
    
    if(oldCur==ci.hCursor)
    	return oldCurNum;
    oldCur=ci.hCursor;
	HBITMAP hbmp;
	BITMAP bm;
	
	GetObject(icon.hbmMask,sizeof(BITMAP),&bm);

	HDC hGlobal,hDCMask,hDCColor;
	hGlobal = GetDC(NULL);
	hDCMask = CreateCompatibleDC(hGlobal);
	hDCColor = CreateCompatibleDC(hGlobal);
	hbmp = CreateCompatibleBitmap(hGlobal,bm.bmWidth,bm.bmWidth);
	SelectObject(hDCColor,hbmp);

	//FloodFill(hDCColor,0,0,RGB(255,255,255));//±≥æ∞…´
	SelectObject(hDCMask,icon.hbmMask);
	BitBlt(hDCColor,0,0,bm.bmWidth,bm.bmWidth,hDCMask,0,0,SRCAND);
	if (icon.hbmColor == NULL){
		BitBlt(hDCColor,0,0,bm.bmWidth,bm.bmWidth,hDCMask,0,bm.bmWidth,SRCINVERT);
	}else{
		SelectObject(hDCMask,icon.hbmColor);
		BitBlt(hDCColor,0,0,bm.bmWidth,bm.bmWidth,hDCMask,0,0,SRCINVERT);
	}
	
	BITMAPFILEHEADER   bmfHeader;
    BITMAPINFOHEADER   bi;

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bm.bmWidth;
    bi.biHeight = bm.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    DWORD dwBmpSize = ((bm.bmWidth * bi.biBitCount + 31) / 32) * 4 * bm.bmHeight;
	
    HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
    unsigned char *lpbitmap = (unsigned char *)GlobalLock(hDIB);

    // Gets the "bits" from the bitmap and copies them into a buffer
    // which is pointed to by lpbitmap.
    GetDIBits(hDCColor, hbmp, 0,(UINT)bm.bmHeight, lpbitmap, (BITMAPINFO *)&bi, DIB_RGB_COLORS);


	oldCurNum=0;
	for(int i=0;i<dwBmpSize;i++)
		oldCurNum+=(int)lpbitmap[i];

    GlobalUnlock(hDIB);
    GlobalFree(hDIB);
	
	
	DeleteObject(icon.hbmMask);
	if (icon.hbmColor != NULL)
		DeleteObject(icon.hbmColor);
	DeleteObject(hbmp);
	ReleaseDC(NULL,hDCMask);
	ReleaseDC(NULL,hDCColor);
	ReleaseDC(NULL,hGlobal);
	
    return oldCurNum;
}
VOID Robot::mousePress(DWORD buttons)
{
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
}

VOID Robot::mouseRelease(DWORD buttons)
{
    mouse_event( MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

VOID Robot::mouseLeftClick()
{
    mouse_event( MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    mouse_event( MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}
VOID Robot::mouseRightClick()
{
    mouse_event( MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
    mouse_event( MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
}

VOID Robot::mouseWheel(DWORD wheelAmt)
{

}
INT Robot::getImageW(){
	return imageW;
}
INT Robot::getImageH(){
	return imageH;
}

Robot::~Robot()
{
    delete[] imageBuff;
}
