#include <fstream>
#include<sstream>  
#include "Config.h"
using namespace std;

Config::Config(char* filename) {
	this->filename=filename;
}
void Config::read(AutoFish &fish) {
	ifstream in(filename, ios::in); 
	if(!in.is_open())return;
	vector<string>  words; 
    string      line; 
    int index;
    while(getline(in,line))
    {
    	trim(line);
    	vector<string> vs = split(line,'='); 
    	if(vs.size()>1)
    		options[vs[0]]=vs[1];
    }
	if(options.count("fishKey"))
		fish.fishKey=str2int(options["fishKey"]);
		
	if(options.count("doFishTime"))
		fish.doFishTime=str2int(options["doFishTime"]);
		
	if(options.count("perFishTime"))
		fish.perFishTime=str2int(options["perFishTime"]);
	
	if(options.count("uounKey"))
		fish.uounKey=str2int(options["uounKey"]);
		
	if(options.count("doUounTime"))
		fish.doUounTime=str2int(options["doUounTime"]);
		
	if(options.count("perUounTime"))
		fish.perUounTime=str2int(options["perUounTime"]);
	if(options.count("floatColor")){
		vector<string> vss = split(options["floatColor"],','); 
		if(vss.size()>2){
			fish.floatColor=Color(str2int(vss[0]), str2int(vss[1]), str2int(vss[2]) );
		}

	}	
	if(options.count("floatColorSimilar")){
		float num=str2int(options["floatColorSimilar"]);
		fish.floatColorSimilar=num/100.00;
	}
	if(options.count("recCurShap")){
		fish.recCurShap = str2int(options["recCurShap"]);
	}
	
	if(options.count("curShap")){
		fish.curShape = str2int(options["curShap"]);
	}
	
	if(options.count("soundData")){
		vector<string> vss = split(options["soundData"],','); 
		if(vss.size()>9){
			for(int i=0;i<10;i++)
				fish.soundData[i] = str2float(vss[i]);
		}
	}

	if(options.count("soundSimilar")){
		float num=str2int(options["soundSimilar"]);
		fish.soundSimilar=num/100.00;
	}
	
	in.close();
}
void Config::save(AutoFish &fish) {
	ofstream out(filename, ios::out);
	if(!out.is_open())return;
	out<<"//魔兽世界自动钓鱼v1.1  最新版下载 https://www.czios.com/wowfish \n";
	out<<"//修改需要重启软件生效，删除本文件可恢复默认设置 \n\n";
	out<<"//钓鱼按键键盘虚拟码\n";
	out<<"fishKey = " + int2str(fish.fishKey) + "\n";
	out<<"//钓鱼施法时间，单位毫秒\n";
	out<<"doFishTime = " + int2str(fish.doFishTime) + "\n";
	out<<"//钓鱼循环周期，单位毫秒\n";
	out<<"perFishTime = " + int2str(fish.perFishTime) + "\n\n";

	out<<"//鱼饵按键键盘虚拟码\n";
	out<<"uounKey = " + int2str(fish.uounKey) + "\n";
	out<<"//鱼饵施法时间，单位毫秒\n";
	out<<"doUounTime = " + int2str(fish.doUounTime) + "\n";
	out<<"//鱼饵循环周期，单位毫秒\n";
	out<<"perUounTime = " + int2str(fish.perUounTime) + "\n\n";

	out<<"//浮漂识别色RGB\n";
	out<<"floatColor = " + int2str(fish.floatColor.R) + ", "+ int2str(fish.floatColor.G) +", "+ int2str(fish.floatColor.B) +"\n";
	out<<"//浮漂识别色识别度，取值0-100\n";
	out<<"floatColorSimilar = " + int2str(fish.floatColorSimilar*100) + "\n\n";
	
	out<<"//开启鼠标形状确认， 0关闭，1开启\n";
	out<<"recCurShap = " + int2str(fish.recCurShap) +"\n";
	out<<"//鼠标形状特征，鼠标ICON RGBA总和\n";
	out<<"curShape = " + int2str(fish.curShape) + "\n\n";

	out<<"//上钩声音数据，0-3KH的数据百分比\n";
	string str="soundData = ";
	for(int i=0;i<9;i++)
			str += float2str(fish.soundData[i])+", ";
	str +=float2str(fish.soundData[9])+"\n";
	out<<str;
	out<<"//上钩声音识别度，取值0-100\n";
	out<<"soundSimilar = "+ int2str(fish.soundSimilar*100);
	
	out.close();
}

void Config::trim(string &s)
{
    int index = 0;
    if( !s.empty())
    {
        while( (index = s.find(' ',index)) != string::npos)
        {
            s.erase(index,1);
        }
    }
}

vector<string> Config::split(string s,char token){  
    istringstream iss(s);  
    string word;  
    vector<string> vs;  
    while(getline(iss,word,token)){  
        vs.push_back(word);  
    }  
    return vs;  
} 

string Config::int2str(const int &int_temp)  
{  
	string str;
    stringstream stream;  
    stream<<int_temp;  
    stream>>str;
    return str;
}  
int Config::str2int(const string &string_temp)  
{  
	int rs;
    stringstream stream(string_temp);  
    stream>>rs; 
	return rs; 
}  

string Config::float2str(const float &float_temp)  
{  
	string str;
    stringstream stream;  
    stream<<float_temp;  
    stream>>str;
    return str;
}  
float Config::str2float(const string &string_temp)  
{  
	float rs;
    stringstream stream(string_temp);  
    stream>>rs; 
	return rs; 
} 
Config::~Config() {};
