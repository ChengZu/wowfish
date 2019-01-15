/*
 * Log.cpp
 *
 *  Created on: 2016-04-03
 *      Author: ChengZu  Email: 1351606745@qq.com
 */

#include "Log.h"
using namespace std;


Log::Log()
{
}
void Log::out(std::string str)
{
    ofstream out("log.txt", ios::app);
    if (out.is_open())
    {
        out << str;
        out << "\n";
        out.close();
    }
};
void Log::out(int str)
{
    ofstream out("log.txt", ios::app);
    if (out.is_open())
    {
        out << str;
        out << "\n";
        out.close();
    }
};
void Log::out(float str)
{
    ofstream out("log.txt", ios::app);
    if (out.is_open())
    {
        out << str;
        out << "\n";
        out.close();
    }
};
void Log::out(WORD str)
{
    ofstream out("log.txt", ios::app);
    if (out.is_open())
    {
        out << str;
        out << "\n";
        out.close();
    }
};
void Log::out(DWORD str)
{
    ofstream out("log.txt", ios::app);
    if (out.is_open())
    {
        out << str;
        out << "\n";
        out.close();
    }
};
Log::~Log()
{
    // TODO Auto-generated destructor stub
}

