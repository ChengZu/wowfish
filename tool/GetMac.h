#include <iostream>
#include <string>
#include <windows.h>


BOOL GetMacByCmd(char *lpszMac, int len/*=128*/)
{

    const long MAX_COMMAND_SIZE = 10000; //命令行输出缓冲大小
    CHAR szFetCmd[]            = "ipconfig /all"; //获取MAC命令行
    const std::string strEnSearch = "Physical Address. . . . . . . . . : "; //网卡MAC地址的前导信息
    const std::string strChSearch = "物理地址. . . . . . . . . . . . . : ";

    BOOL   bret       = FALSE;
    HANDLE hReadPipe  = NULL; //读取管道
    HANDLE hWritePipe = NULL; //写入管道
    PROCESS_INFORMATION pi;   //进程信息
    STARTUPINFO         si;   //控制命令行窗口信息
    SECURITY_ATTRIBUTES sa;   //安全属性

    char            szBuffer[MAX_COMMAND_SIZE + 1] = {0}; //放置命令行结果的输出缓冲区
    std::string		strBuffer;
    unsigned long   count = 0;
    long            ipos  = 0;

    pi.hProcess = NULL;
    pi.hThread  = NULL;
    si.cb       = sizeof(STARTUPINFO);
    sa.nLength  = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle       = TRUE;
    do{
        //1.0 创建管道
        bret = CreatePipe(&hReadPipe, &hWritePipe, &sa, 0);
        if(!bret)
        {
            break;
        }

        //2.0 设置命令行窗口的信息为指定的读写管道
        GetStartupInfo(&si);
        si.hStdError    = hWritePipe;
        si.hStdOutput   = hWritePipe;
        si.wShowWindow  = SW_HIDE; //隐藏命令行窗口
        si.dwFlags      = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

        //3.0 创建获取命令行的进程
        bret = CreateProcess(NULL, szFetCmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi );
        if(!bret)
        {
            break;
        }

        //4.0 读取返回的数据
        WaitForSingleObject (pi.hProcess, 2000/*INFINITE*/);
        bret  =  ReadFile(hReadPipe,  szBuffer,  MAX_COMMAND_SIZE,  &count,  0);
        if(!bret)
        {
            break;
        }

        //5.0 查找MAC地址，默认查找第一个,一般为以太网的MAC
        strBuffer = szBuffer;
        ipos = strBuffer.find(strEnSearch);

        if (ipos < 0)//区分中英文的系统
        {
            ipos = strBuffer.find(strChSearch);
            if (ipos < 1)
            {
                break;
            }
            //提取MAC地址串
            strBuffer = strBuffer.substr(ipos + strChSearch.length());
        }
        else
        {
            //提取MAC地址串
            strBuffer = strBuffer.substr(ipos + strEnSearch.length());
        }

        ipos = strBuffer.find("\n");
        strBuffer = strBuffer.substr(0, ipos);

        memset(szBuffer, 0x00, sizeof(szBuffer));
        strcpy(szBuffer, strBuffer.c_str());

        //去掉中间的“00-50-EB-0F-27-82”中间的'-'得到0050EB0F2782
        int j = 0;
        for(int i = 0; i < strlen(szBuffer); i++)
        {
            if(szBuffer[i] != '-')
            {
                lpszMac[j] = szBuffer[i];
                j++;
            }
        }
    }
    while(false);
    bret = TRUE;


    //关闭所有的句柄
    CloseHandle(hWritePipe);
    CloseHandle(hReadPipe);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return(bret);
}

