/*
 * parse config file with [.ini] format.
 *
 * compile: g++ -o my_config -g my_config.cpp
 * usage:	./my_config -c test_config.ini 
 *
 */
#include "my_config.h"

#include <stdio.h>
#include <iostream>

using namespace std;

void MyConfig::free()
{
	delete this;
}

MyConfig::MyConfig() : m_iLoaded(0), m_nullRet("")
{	
}

int MyConfig::loaded()
{
	return m_iLoaded;
}

MyConfig * MyConfig::instance()
{
	static MyConfig * _ins  = 0;

	if(0 == _ins) {
		_ins = new MyConfig;
	}
	return _ins;
}

/*************************************************
@Brief:         加载配置文件
@Param:         input,const string&,文件路径
@Return:        int，=0,加载成功，<0,,加载失败，返回错误
@Others:        svr初始化时候加载，失败svr不能启动
*************************************************/

int MyConfig::loadConfigFile(const  char * s_cnf_file)
{
	if(NULL == s_cnf_file) {
		cout << "no config file" << endl;
		return -1;	
	}
	
	FILE * fp = 0;

	fp = fopen(s_cnf_file, "rb");
	if (NULL == fp) {
		cout << "file is not exist[" << s_cnf_file << "]" <<endl;
		return -1;
	}
	char szLine[MAX_CONF_ITEM_LENTH];
	char * pszRet = 0;
	string cSectionName;
	map<string,string> cSection;

	while (!feof(fp)) {
		pszRet = fgets(szLine,MAX_CONF_ITEM_LENTH,fp);
		if (NULL == pszRet) {
			if (!cSectionName.empty()) {
				m_contener[cSectionName] = cSection;				
			}			
			fclose(fp);
			m_iLoaded = 1;
			return 0;			
		}

		//去掉空格，\t\n\r字符
		int iNum = 0,i;
		for (i = 0 ; *(szLine + i); i++) {
			if (*(szLine + i) == ' ' || *(szLine + i) == '\t' || *(szLine + i) == '\r' || *(szLine + i) == '\n') {
				iNum++;
				continue;
			}
			if(*(szLine + i) == ';') {
				*(szLine + i - iNum) = 0;	
				break;
			}
			*(szLine + i - iNum) = *(szLine + i);
		}
		*(szLine + i - iNum) = 0;
		// 跳过空白行
		if (i == iNum) {
			continue;
		}
		
		//新节开始
		if (szLine[0]=='[' && szLine[i-iNum-1] == ']') {
			if (!cSectionName.empty()) {
				m_contener[cSectionName] = cSection;
			}		

			szLine[i-iNum-1] = 0;
			cSectionName = &szLine[1];
			cSection.clear();
		} else {   //当前节中的一个配置项
			char *pszName = strchr(szLine, '=');
			if (NULL == pszName) {
				cSection[szLine] = "";
			} else {
				*pszName = 0;
				pszName++;
				cSection[szLine] = pszName;
			}
		}
	}	
	if (!cSectionName.empty()) {
		m_contener[cSectionName] = cSection;
	}
	m_iLoaded = 1;
	fclose(fp);
    return 0;
}

/*************************************************
@Brief:         获取配置文件int字段（外部使用，直接返回结构体数据）
@Param:         input,const string&,区域
@Param:         input,const string&,配置项名称
@Return:        int，=0,获取成功，<0,, 获取失败，返回错误
@Others:        svr运行时候获取，如果不能获取，证明配置有误
*************************************************/
int MyConfig::getConfigInt(const string& s_section, const string& s_item)
{
    CONFIG_CONTENER::iterator iter = m_contener.find(s_section);

    if(iter == m_contener.end()) {
    	 return -1;
    }

    map<string,string>::iterator nest_iter =  iter->second.find(s_item);
    if(iter->second.end() == nest_iter) {
    	return -1;
    }
    return  atoi( nest_iter->second.c_str());
}

/*************************************************
@Brief:         获取配置文件string字段（外部使用，直接返回结构体数据）
@Param:         input,const string&,区域
@Param:         input,const string&,配置项名称
@Return:        int，=0,获取成功，<0,, 获取失败，返回错误
@Others:        svr运行时候获取，如果不能获取，证明配置有误
*************************************************/
const char * MyConfig::getConfigStr(const string& s_section, const string& s_item)
{
   CONFIG_CONTENER::iterator iter = m_contener.find(s_section);

    if(iter == m_contener.end()) {
  		return NULL;
    }

     map<string,string>::iterator nest_iter =  iter->second.find(s_item);
    if(iter->second.end() == nest_iter) {
		return NULL;
    }

    return nest_iter->second.c_str();
}

const string & MyConfig::getCfgStr(const string& s_section, const string& s_item)
{
   CONFIG_CONTENER::iterator iter = m_contener.find(s_section);

    if(iter == m_contener.end()) {
    	return m_nullRet;
    }

	map<string,string>::iterator nest_iter =  iter->second.find(s_item);
    if(iter->second.end() == nest_iter) {
   		return m_nullRet;
   	}

    return nest_iter->second;
}

///*************************************************
//@Brief:         获取配置文件string字段(内部使用)
//@Param:         input,const string&,区域
//@Param:         input,const string&,配置项名称
//@Return:        int，=0,获取成功，<0,, 获取失败，返回错误
//@Others:        svr运行时候获取，如果不能获取，证明配置有误
//*************************************************/
//string MyConfig::loadConfigStr(const string& s_section, const string& s_item)
//{
//    int iRet;
//    char sResult[1024];
//    iRet = m_objClibIni.get_str(s_section.c_str(), s_item.c_str(), sResult, sizeof(sResult));
//    if(0 != iRet)
//    {
//        printf("加载配置项错误：[Section=%s][Item=%s]\n", s_section.c_str(), s_item.c_str());
//        return ""; 
//    }
//    return string(sResult);
//}

#if 1
/*
 * test config
 */
int main(int argc, char **argv)
{
	int status;

	if (argc < 3) {
		printf("usage: my_config -c cfgfile\n");
		return 1;
	}

	MyConfig *my_config = MyConfig::instance();

	char *cfg_file;

	cfg_file = argv[2];

	status = my_config->loadConfigFile(cfg_file);
	if (status < 0) {
		printf("load config file '%s' failed\n", cfg_file);
	}

	string name = my_config->getConfigStr("Person", "Name");
	int age = my_config->getConfigInt("Person", "Age");

	printf("name:%s age:%d\n", name.c_str(), age);

	my_config->free();

	return 0;
}

#endif
