/*
 * Config.h
 *
 *  Created on: 2018-06-19
 *      Author: ChengZu  Email: 1351606745@qq.com
 */

#ifndef CONFIG_H_
#define CONFIG_H_
#include <iostream> 
#include <string> 
#include <map> 
#include <vector> 
#include "AutoFish.h"

class Config
{
public:
    Config(char* filename) ;
    void read(AutoFish &fish);
    void save(AutoFish &fish);
    
    void trim(std::string &s);
    std::vector<std::string> split(std::string s,char token);
    std::string int2str(const int &int_temp);
    int str2int(const std::string &string_temp);
    std::string float2str(const float &float_temp);
    float str2float(const std::string &string_temp);
    virtual ~Config() ;
private:
	char* filename;
	std::map<std::string, std::string> options;
};


#endif /* CONFIG_H_ */

