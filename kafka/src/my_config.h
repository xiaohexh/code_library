#ifndef _MY_CONFIG_H_
#define _MY_CONFIG_H_

#include <string.h>
#include <stdlib.h>

#include <map>
#include <string>

#define  MAX_CONF_ITEM_LENTH  1024

using std::string;
using std::map;

typedef map<string , map < string ,string> > CONFIG_CONTENER;

class MyConfig
{
public:	
	static MyConfig* instance();
	int loadConfigFile(const char * s_cnf_file);
	const char * getConfigStr(const string& s_section, const string& s_item);
	const string & getCfgStr(const string& s_section, const string& s_item);
	int getConfigInt(const string& s_section, const string& s_item);

	int loaded();

	void free();
protected:
	MyConfig();
	MyConfig(const MyConfig& objLog);
	MyConfig& operator = (const MyConfig& objLog);

private:
	 CONFIG_CONTENER m_contener;
	 int m_iLoaded;

	const string m_nullRet;
	 //string loadConfigStr(const string& s_section, const string& s_item);
};

#endif
