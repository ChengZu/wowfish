#include <windows.h>

#define HK_ID_CQ  (WM_USER+12016)
#define HK_ID_F9  (WM_USER+12017)
#define HK_ID_F10 (WM_USER+12018)
#define HK_ID_F11 (WM_USER+12019)
#define HK_ID_F12 (WM_USER+12020)

void OnHotKey(HWND hWnd) {  }

void RegisterAllHotKey(HWND hWnd)
{
	RegisterHotKey(hWnd, HK_ID_CQ, MOD_CONTROL, 0x41);
    RegisterHotKey(hWnd, HK_ID_F9, 0, VK_F9);
    //RegisterHotKey(hWnd, HK_ID_F10, 0, VK_F10);
    RegisterHotKey(hWnd, HK_ID_F11, 0, VK_F11);
    //RegisterHotKey(hWnd, HK_ID_F12, 0, VK_F12);

}

void UnregisterAllHotKey(HWND hWnd)
{
	UnregisterHotKey(hWnd, HK_ID_CQ);
    UnregisterHotKey(hWnd, HK_ID_F9);
    //UnregisterHotKey(hWnd, HK_ID_F10);
    UnregisterHotKey(hWnd, HK_ID_F11);
    //UnregisterHotKey(hWnd, HK_ID_F12);
}
