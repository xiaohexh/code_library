#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include <pthread.h>

#include <event.h>
#include <evdns.h>
#include <event2/util.h>
#include <event2/event.h>

#include <string>
#include <map>
#include <queue>
using std::string;
using std::map;
using std::queue;

//维护url原始字符串
typedef struct surl {
    string	url;
    int		level;	// url抓取深度
    int		type;	// 抓取类型
} surl;

//解析后的
typedef struct ourl {
    string	domain;	//域名
    string	path;	//路径
    string	ip;		//IP
    int		port;	//端口
    int		level;	//深度
} ourl;

pthread_mutex_t lock;
pthread_cond_t 	cond;

queue<surl *>	surl_q;
queue<ourl *> 	ourl_q;
map<string, string> host_ip_m;

surl s_url;

ourl *surl2ourl(surl *s_url);

//DNS解析回调函数
static void dns_callback(int result, char type,
						 int count, int ttl,
						 void *addresses, void *arg)
{
    ourl * o_url = (ourl *)arg;
    struct in_addr *addrs = (in_addr *)addresses;

    if (result != 0 || count == 0) {
        printf("Dns resolve fail: %s", o_url->domain.c_str());
    } else {
        char * ip = inet_ntoa(addrs[0]);
        printf("Dns resolve OK: %s -> %s\n", o_url->domain.c_str(), ip);
        host_ip_m[o_url->domain] = ip;
        o_url->ip = ip;
        ourl_q.push(o_url);
    }
    event_loopexit(NULL); // not safe for multithreads
}

void *url_parse(void *arg)
{
    surl *s_url = NULL;
    ourl *o_url = NULL;

    pthread_mutex_lock(&lock);
    while (surl_q.empty()) {
        pthread_cond_wait(&cond, &lock);
    }
    s_url = surl_q.front();
    surl_q.pop();
    pthread_mutex_unlock(&lock);

    o_url = surl2ourl(s_url);
    if (o_url == NULL) {
        return NULL;
    }

    map<string, string>::iterator iter = host_ip_m.find(o_url->domain);
    if (iter == host_ip_m.end()) {
        event_base * base = event_init();
        evdns_init();
        evdns_resolve_ipv4(o_url->domain.c_str(), 0, dns_callback, o_url);
        event_dispatch();
        event_base_free(base);
    } else {
        o_url->ip = strdup(iter->second.c_str());
        ourl_q.push(o_url);
    }

    return NULL;
}

ourl *surl2ourl(surl *s_url)
{
    ourl *o_url = new ourl();
    if (o_url == NULL) {
        return NULL;
    }

    string::size_type pos;
    // get domain & path
    if ((pos = s_url->url.find_first_of("/")) == string::npos) { // no path
        o_url->domain = s_url->url;
    } else {
        o_url->domain = s_url->url.substr(0, pos);
        o_url->path = s_url->url.substr(pos + 1);;
    }

    // get port number
    if ((pos = s_url->url.find_first_of(":")) == string::npos) {
        o_url->port = 80;
    } else {
        o_url->port = atoi(s_url->url.substr(pos + 1).c_str());
        if (o_url->port == 0) {
            o_url->port = 80;
        }
    }

    // get level
    o_url->level = s_url->level;

    return o_url;
}

int main(int argc, char **argv)
{
    int status;

    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_t tid;
    status = pthread_create(&tid, NULL, url_parse, NULL);
    if (status < 0) {
        printf("pthread_create failed: %s\n", strerror(errno));
        return 1;
    }

    pthread_mutex_lock(&lock);

    if (argc < 2) {
    	s_url.url = "www.google.com";
        printf("use default domain:%s\n", s_url.url.c_str());
    } else {
    	s_url.url = argv[1];
	}
    surl_q.push(&s_url);
    pthread_cond_signal(&cond);

    pthread_mutex_unlock(&lock);

    pthread_join(tid, NULL);

    while (!ourl_q.empty()) {
        delete(ourl_q.front());
        ourl_q.pop();
    }

    return 0;
}
