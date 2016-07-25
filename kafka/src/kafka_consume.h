#ifndef _KAFKA_CONSUME_H_
#define _KAFKA_CONSUME_H_

#include <map>
#include <string>
#include <vector>

#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>

#include "kafka_helper.h"

#define CONFIG_FILE			"../etc/kafka_consume.ini"
#define DEFAULT_EXPIRE_TIME 20
#define PARTITION_NUM		40

#ifndef likely
#define likely(x)  __builtin_expect(!!(x), 1)
#endif
#ifndef unlikely
#define unlikely(x)  __builtin_expect(!!(x), 0)
#endif

using namespace std;
using namespace bp;

class KafkaConsume
{
public:
    class CateParam{
    public:
        CateParam():clp(-1), click_times(0){}

        int clp;
        unsigned click_times;
    };
    static  const unsigned C_ARRAY_LEN = 3;

public:
	KafkaConsume();
	int init();
	int start();
    int Process();
	void release();

private:
    //业务辅助函数
    int _store_data();
    string _gen_md5(string origin_str);

private:

    //GUID数据结构
    long   m_uid_bucketsize;
    long   m_uid_nodesize;
    long   m_uid_chunks;
    long   m_uid_chunksize;

	pthread_t	m_cthread[PARTITION_NUM];
	pthread_t	m_pthread;
};

#endif
