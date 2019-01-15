/*
 * UUID.h
 *
 *  Created on: 2016-04-03
 *      Author: ChengZu  Email: 1351606745@qq.com
 */

#ifndef UTILS_LOG_H_
#define UTILS_LOG_H_
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

class Log
{
public:
    Log();
    static void out(std::string str);
    static void out(int str);
    static void out(float str);
    static void out(WORD str);
    static void out(DWORD str);
    virtual ~Log();
private:
    std::string file;
};



#endif /* UTILS_UUID_H_ */
