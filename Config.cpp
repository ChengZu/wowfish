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
	out<<"//ħ�������Զ�����v1.1  ���°����� https://www.czios.com/wowfish \n";
	out<<"//�޸���Ҫ���������Ч��ɾ�����ļ��ɻָ�Ĭ������ \n\n";
	out<<"//���㰴������������\n";
	out<<"fishKey = " + int2str(fish.fishKey) + "\n";
	out<<"//����ʩ��ʱ�䣬��λ����\n";
	out<<"doFishTime = " + int2str(fish.doFishTime) + "\n";
	out<<"//����ѭ�����ڣ���λ����\n";
	out<<"perFishTime = " + int2str(fish.perFishTime) + "\n\n";

	out<<"//�����������������\n";
	out<<"uounKey = " + int2str(fish.uounKey) + "\n";
	out<<"//���ʩ��ʱ�䣬��λ����\n";
	out<<"doUounTime = " + int2str(fish.doUounTime) + "\n";
	out<<"//���ѭ�����ڣ���λ����\n";
	out<<"perUounTime = " + int2str(fish.perUounTime) + "\n\n";

	out<<"//��Ưʶ��ɫRGB\n";
	out<<"floatColor = " + int2str(fish.floatColor.R) + ", "+ int2str(fish.floatColor.G) +", "+ int2str(fish.floatColor.B) +"\n";
	out<<"//��Ưʶ��ɫʶ��ȣ�ȡֵ0-100\n";
	out<<"floatColorSimilar = " + int2str(fish.floatColorSimilar*100) + "\n\n";
	
	out<<"//���������״ȷ�ϣ� 0�رգ�1����\n";
	out<<"recCurShap = " + int2str(fish.recCurShap) +"\n";
	out<<"//�����״���������ICON RGBA�ܺ�\n";
	out<<"curShape = " + int2str(fish.curShape) + "\n\n";

	out<<"//�Ϲ��������ݣ�0-3KH�����ݰٷֱ�\n";
	string str="soundData = ";
	for(int i=0;i<9;i++)
			str += float2str(fish.soundData[i])+", ";
	str +=float2str(fish.soundData[9])+"\n";
	out<<str;
	out<<"//�Ϲ�����ʶ��ȣ�ȡֵ0-100\n";
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
