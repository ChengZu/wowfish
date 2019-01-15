#include <iostream>
#include <string>
#include <windows.h>


BOOL GetMacByCmd(char *lpszMac, int len/*=128*/)
{

    const long MAX_COMMAND_SIZE = 10000; //��������������С
    CHAR szFetCmd[]            = "ipconfig /all"; //��ȡMAC������
    const std::string strEnSearch = "Physical Address. . . . . . . . . : "; //����MAC��ַ��ǰ����Ϣ
    const std::string strChSearch = "�����ַ. . . . . . . . . . . . . : ";

    BOOL   bret       = FALSE;
    HANDLE hReadPipe  = NULL; //��ȡ�ܵ�
    HANDLE hWritePipe = NULL; //д��ܵ�
    PROCESS_INFORMATION pi;   //������Ϣ
    STARTUPINFO         si;   //���������д�����Ϣ
    SECURITY_ATTRIBUTES sa;   //��ȫ����

    char            szBuffer[MAX_COMMAND_SIZE + 1] = {0}; //���������н�������������
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
        //1.0 �����ܵ�
        bret = CreatePipe(&hReadPipe, &hWritePipe, &sa, 0);
        if(!bret)
        {
            break;
        }

        //2.0 ���������д��ڵ���ϢΪָ���Ķ�д�ܵ�
        GetStartupInfo(&si);
        si.hStdError    = hWritePipe;
        si.hStdOutput   = hWritePipe;
        si.wShowWindow  = SW_HIDE; //���������д���
        si.dwFlags      = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

        //3.0 ������ȡ�����еĽ���
        bret = CreateProcess(NULL, szFetCmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi );
        if(!bret)
        {
            break;
        }

        //4.0 ��ȡ���ص�����
        WaitForSingleObject (pi.hProcess, 2000/*INFINITE*/);
        bret  =  ReadFile(hReadPipe,  szBuffer,  MAX_COMMAND_SIZE,  &count,  0);
        if(!bret)
        {
            break;
        }

        //5.0 ����MAC��ַ��Ĭ�ϲ��ҵ�һ��,һ��Ϊ��̫����MAC
        strBuffer = szBuffer;
        ipos = strBuffer.find(strEnSearch);

        if (ipos < 0)//������Ӣ�ĵ�ϵͳ
        {
            ipos = strBuffer.find(strChSearch);
            if (ipos < 1)
            {
                break;
            }
            //��ȡMAC��ַ��
            strBuffer = strBuffer.substr(ipos + strChSearch.length());
        }
        else
        {
            //��ȡMAC��ַ��
            strBuffer = strBuffer.substr(ipos + strEnSearch.length());
        }

        ipos = strBuffer.find("\n");
        strBuffer = strBuffer.substr(0, ipos);

        memset(szBuffer, 0x00, sizeof(szBuffer));
        strcpy(szBuffer, strBuffer.c_str());

        //ȥ���м�ġ�00-50-EB-0F-27-82���м��'-'�õ�0050EB0F2782
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


    //�ر����еľ��
    CloseHandle(hWritePipe);
    CloseHandle(hReadPipe);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return(bret);
}

