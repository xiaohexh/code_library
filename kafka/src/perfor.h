/* ************************************************************
 *  Copyright(c) 2014, Pan Hao. All rights reserverd.
 *     Name: perfor.h
 * Describe:
 *   Author: Pan Hao
 *     Date: 2015/07/01 14:50
 *  Version: 1.01
 * Modified:
 * ************************************************************/

#ifndef __PERFOR_H__
#define __PERFOR_H__

#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <string>
#include <map>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "CMysql.h"
using namespace std;
using namespace boost;

int   PerforInit();
void* PerforRun(void* arg);
void  CalStart(const char* name = NULL, int level = 1);
void  CalStop(const char* name = NULL, int level = 1);

// just global
#define CALSTART1(name) CalStart(name, 1)
#define CALSTOP1(name) CalStop(name, 1)

// include function
#define CALSTART2(name) CalStart(name, 2)
#define CALSTOP2(name) CalStop(name, 2)

class PerforStat
{
private:
    PerforStat()
    {
        gettimeofday(&gStart, NULL);
        pthread_mutex_init(&mtx, NULL);
    }

    ~PerforStat() { }
public:
    class Item
    {
    public:
        struct timeval start;
        struct timeval stop;

        long    totTime;          // us
        long    maxTime;          // us
        long    minTime;          // us

        long     errNum;
        long     totNum;
        Item ()
        {
            totTime = 0;
            maxTime = 0;
            minTime = 1000000; // 1s = 1000000us
            errNum  = 0;
            totNum  = 0;
        }
    };

    static PerforStat* Ins();
    pthread_mutex_t   mtx;
    map<string, Item> msi;
    int               lvl;
    int               step;
    CMysql            mysql;
    string            serName;
    string            table;
    string            ip;
    int               serId;
    struct timeval    gStart;
    struct timeval    gStop;

    static string FormatTime(int sec);
    static const char* GetIP();
};

string PerforStat::FormatTime(int sec)
{
    struct tm ttm;
    time_t ssec = sec;
    localtime_r(&ssec, &ttm);

    char strt[64] = {0};
    snprintf(strt, sizeof(strt), "%d-%02d-%02d %02d:%02d:%02d", \
	     ttm.tm_year + 1900, ttm.tm_mon + 1, ttm.tm_mday, ttm.tm_hour, ttm.tm_min, ttm.tm_sec);

    return string(strt);
}

const char* PerforStat::GetIP()
{
    int socket_fd;
    // struct sockaddr_in *sin;
    struct ifreq *ifr;
    struct ifconf conf;
    char buff[BUFSIZ];
    int  num;
    int  i;
    string ip;
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    conf.ifc_len = BUFSIZ;
    conf.ifc_buf = buff;
    ioctl(socket_fd, SIOCGIFCONF, &conf);
    num = conf.ifc_len / sizeof(struct ifreq);
    ifr = conf.ifc_req;
    for (i = 0; i < num; i++) {
        struct sockaddr_in *sin = (struct sockaddr_in *)(&ifr->ifr_addr);

        ioctl(socket_fd, SIOCGIFFLAGS, ifr);
        if (((ifr->ifr_flags & IFF_LOOPBACK) == 0) && (ifr->ifr_flags & IFF_UP)) {
            ip.assign(inet_ntoa(sin->sin_addr));
            return ip.c_str();
        }

        ifr++;
    }

    return "";
}

PerforStat* PerforStat::Ins()
{
    static PerforStat* _imp = NULL;
    if (_imp == NULL) {
        _imp = new PerforStat();
    }

    return _imp;
}

int PerforInit()
{
    // start pthread store infomation into db
    pthread_t pp;
    pthread_create(&pp, NULL, PerforRun, NULL);

    return 0;
}

void PerforSetServerInfo(const char* server_name, int timeLoop, int lev)
{
    PerforStat::Ins()->serName.assign(server_name);
    PerforStat::Ins()->step = timeLoop;
    PerforStat::Ins()->lvl  = lev;
    PerforStat::Ins()->ip = PerforStat::GetIP();
}

// info dbhost:port:user:password:db:table
//         0    1               2                              3     4              5
// 127.0.0.1:3306:category_fre_rw:4zfvA&ibg6pzvylcycfAnbzdwxkztm:alarm:warning_record
int PerforSetDB(const char* info)
{
    vector<string> item;
    split(item, info, is_any_of(":"));

    // connect db
    CMysql tmp_mysql(item[0].c_str(), lexical_cast<int>(item[1]), item[2].c_str(), item[3].c_str());

    PerforStat::Ins()->mysql = tmp_mysql;
    if (PerforStat::Ins()->mysql.db_connect()) {
	fprintf(stderr, "Connect to mysql db failed. [%s]\n", PerforStat::Ins()->mysql.get_error());
        return -1;
    }

    char db[32] = {0};
    strncpy(db, item[4].c_str(), sizeof(db));
    if (PerforStat::Ins()->mysql.set_db(db)) {
        fprintf(stderr, "%d MYSQL [%s]\n", __LINE__, PerforStat::Ins()->mysql.get_error());
        return -1;
    }

    if (PerforStat::Ins()->mysql.query("set names utf8")) {
        fprintf(stderr, "%d MYSQL [%s]\n", __LINE__, PerforStat::Ins()->mysql.get_error());
        return -1;
    }

    if (PerforStat::Ins()->mysql.query("set autocommit=0")) { // 不要自动提交
        fprintf(stderr, "%d MYSQL [%s]\n", __LINE__, PerforStat::Ins()->mysql.get_error());
        return -1;
    }

    // set table, step and level
    PerforStat::Ins()->table = item[5];

    return 0;
}

void* PerforRun(void* arg)
{
    pthread_detach(pthread_self());
    time_t now  = time(0);
    time_t last = now;

    while (1) {
    	time(&now);
    	if (now % PerforStat::Ins()->step != 0) {
    	    usleep(500000);
	    continue;
    	}

    	// store into db
    	map<string, PerforStat::Item> mdeal;
        // cout << "Start Lock!!!\n";
    	pthread_mutex_lock(&(PerforStat::Ins()->mtx));
    	mdeal.swap(PerforStat::Ins()->msi);
    	pthread_mutex_unlock(&(PerforStat::Ins()->mtx));

        if (PerforStat::Ins()->gStart.tv_sec % PerforStat::Ins()->step != 0) {
            PerforStat::Ins()->gStart.tv_sec = now;
            sleep(1);
            continue;
        }

        string begin = PerforStat::FormatTime(PerforStat::Ins()->gStart.tv_sec);
	PerforStat::Ins()->gStart.tv_sec = now;
        string end   = PerforStat::FormatTime(now);
	char sql_cstr[1024];
        // , PerforStat::ins()->table.c_str( ));
	long   type;
	string key;
        printf("Stored into DB\n");
	for (map<string, PerforStat::Item>::iterator iter = mdeal.begin(); iter != mdeal.end(); ++ iter) {
	    key.clear();
    	    if (iter->first == "") {
		type = 0;
		key = "exec_time";
            } else {
   	        type = 1;
		key = "func:" + iter->first;
	    }

            memset(sql_cstr, sizeof(sql_cstr), 0);
   	    snprintf(sql_cstr, sizeof(sql_cstr),
                     "update %s set execute_times = %ld, total_handle_time = %ld,"
                     "max_exe_time_consume = IF(%ld > max_exe_time_consume, %ld, max_exe_time_consume),"
                     "min_exe_time_consume = IF(%ld < min_exe_time_consume, %ld, min_exe_time_consume) "
     		     "where '%s' = server_name and '%s' = server_ip and %d = server_id and %ld = type and "
           	     "'%s' = `_key` and '%s' = end_time",
                    	PerforStat::Ins()->table.c_str(), iter->second.totNum, iter->second.totTime/1000,
                    	iter->second.maxTime/1000, iter->second.maxTime/1000, iter->second.minTime/1000, iter->second.minTime/1000,
		     PerforStat::Ins()->serName.c_str(), PerforStat::Ins()->ip.c_str(),
   		     PerforStat::Ins()->serId, type, key.c_str(), end.c_str());

            if (PerforStat::Ins()->mysql.query(sql_cstr)) {
                fprintf(stderr, "%d MYSQL [%s] sql [%s]\n", __LINE__, PerforStat::Ins()->mysql.get_error(), sql_cstr);
            }

	    if (PerforStat::Ins()->mysql.affected_rows()) {
    	        continue;
	    }

    	    memset(sql_cstr, sizeof(sql_cstr), 0);
	    snprintf(sql_cstr, sizeof(sql_cstr),
                     "insert into %s (server_name, server_ip, server_id, type, `_key`, start_time, end_time, "
                     "execute_times, total_handle_time, max_exe_time_consume, min_exe_time_consume) values "
       	             "('%s', '%s', %d, %ld, '%s', '%s', '%s', %ld, %ld, %ld, %ld)",
               	     PerforStat::Ins()->table.c_str(), PerforStat::Ins()->serName.c_str(), PerforStat::Ins()->ip.c_str(),
                    	PerforStat::Ins()->serId, type, key.c_str(), begin.c_str(), end.c_str(),
                     iter->second.totNum, iter->second.totTime/1000, iter->second.maxTime/1000, iter->second.minTime/1000);

         	 PerforStat::Ins()->mysql.query(sql_cstr);
            if (PerforStat::Ins()->mysql.query(sql_cstr)) {
                fprintf(stderr, "%d MYSQL [%s] sql [%s]\n", __LINE__, PerforStat::Ins()->mysql.get_error(), sql_cstr);
            }
	}

	PerforStat::Ins()->mysql.query("commit");
        printf("Commit! into DB\n");
    }
    // make sure deal above not finished in one second!
    sleep(1);

    return NULL;
}

void  CalStart(const char* name, int level)
{
    if (level < PerforStat::Ins()->lvl) { return; }

    if (strlen(name) == 0) {
        printf("Start Lock!\n");
        pthread_mutex_lock(&(PerforStat::Ins()->mtx));
    }

    PerforStat::Item& it = PerforStat::Ins()->msi[name];
    gettimeofday(&it.start, NULL);
}

void  CalStop(const char* name, int level)
{
    if (level < PerforStat::Ins()->lvl) {
        return;
    }

    PerforStat::Item& it = PerforStat::Ins()->msi[name];
    gettimeofday(&it.stop, NULL);
    long timeused = (it.stop.tv_sec  - it.start.tv_sec) * 1000000 + \
                    (it.stop.tv_usec - it.start.tv_usec);

    if (timeused > it.maxTime) {
        it.maxTime = timeused;
    }
    if (timeused < it.minTime) {
        it.minTime = timeused;
    }

    // cout << "name " << name << " max = " << it.maxTime << endl;
    // cout << "name " << name << " min = " << it.minTime << endl;
    ++it.totNum;
    it.totTime += timeused;

    if (strlen(name) == 0) {
        printf("Stop Unlock!\n");
    	pthread_mutex_unlock(&(PerforStat::Ins()->mtx));
        usleep(100);
    }
}

#endif /* __P_H__ */

