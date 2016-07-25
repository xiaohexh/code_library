#include "KafkaConsume.h"

#include <assert.h>
#include <iostream>

#include "all.pb.h"
#include "ILog.h"
#include "IConfig.h"
#include "Utility.h"
#include "Singleton.h"


/* global veriables definition */
bool					g_released = false;
bool                    g_loop = true;
 
vector<vector<MSG> >	g_msgs;
KafkaHelper             g_kfkhandler[PARTITION_NUM];
pthread_mutex_t         g_lock[PARTITION_NUM];

int                     g_partitions = 0; // kafka partions
int                     BATCH_SIZE = 0; // kafka 批量取个数
string                  TOPIC;      // kafka topic
string                  BROKER;     // kafak broker

void consume_callback(void* buff, int size)
{
	if (0 == size) {
		return;
	}

    string msg((char*)buff, size);
	all::all_msg repMsg;
    if (!repMsg.ParseFromString(msg)) {
	    return;
	}
  
	pthread_mutex_lock(&g_lock[index]);
	g_msgs[index].push_back(tmpM);
	pthread_mutex_unlock(&g_lock[index]);
}

void* process(void* arg)
{
	Singleton<KafkaConsume>::instance()->Process(arg);
	return NULL;
}

void* consume(void* idx)
{
	int index = *((int*)idx);
	delete (int*)idx;

	int ret;
	ret = g_kfkhandler[index].consume_start(index, -1000);
	if (ret) {
		ILog::instance()->writeLog(LOG_LEV_ERROR, "[%s %d] partition %d consume_start failed!",
									__FILE__, __LINE__, index);
		return NULL;
	} else {
		ILog::instance()->writeLog(LOG_LEV_INFO, "[%s %d] partition %d consume_start success!",
									__FILE__, __LINE__, index);
	}

	while (g_loop) {
		ret = g_kfkhandler[index].msg_consume(index, BATCH_SIZE, consume_callback);
		//sleep(5);
	}

	ILog::instance()->writeLog(LOG_LEV_INFO, "g_loop was set to False, and quit process");

	return NULL;
}

KafkaConsume::KafkaConsume()
{
}

KafkaConsume::~KafkaConsume()
{
	if (!g_released) {
		Release();
	}
}

void KafkaConsume::Release()
{
	g_released = true;
	g_loop = false;

	for (int i = 0; i < PARTITION_NUM; ++i) {
		g_kfkhandler[i].consume_destroy(i);
	}

	// join all thread
	for (int i = 0; i < PARTITION_NUM; ++i) {
		pthread_join(m_cthread[i], NULL);
	}

	for (int i = 0; i < PARTITION_NUM; ++i) {
		pthread_join(m_pthread[i], NULL);
	}

	for (int i = 0; i < PARTITION_NUM; ++i) {
		pthread_mutex_destroy(&g_lock[i]);
	}
}

inline int KafkaConsume::_connect_redis(redisContext **redis_ctxt)
{
    struct timeval timeout = { 1, 500000 }; // 1.5 seconds
    *redis_ctxt = redisConnectWithTimeout(m_redis_host.c_str(), m_redis_port, timeout);
    if ((*redis_ctxt) == NULL || (*redis_ctxt)->err) {
		if (*redis_ctxt) {
			printf("Connection error: %s\n", (*redis_ctxt)->errstr);
            redisFree(*redis_ctxt);
			*redis_ctxt = NULL;
        } else {
            printf("Connection error: can't allocate redis context\n");
        } 
        return -1; 
    }
	return 0;
}

int KafkaConsume::init()
{
	int ret = 0;

	/*初始化kafka*/
	//BROKER = IConfig::instance()->getConfigStr("KAFKA", "BROKER");
	BROKER = "172.8.11.12:9092,172.8.11.22:9092,172.8.11.23:9092,172.28.11.24:9092,172.8.11.25:9092,172.8.11.26:9092,172.8.11.27:9092";

	//TOPIC = IConfig::instance()->getConfigStr("KAFKA", "TOPIC");
	TOPIC = "uv";

	//BATCH_SIZE = IConfig::instance()->getConfigInt("KAFKA", "BATCH_SIZE");
	BATCH_SIZE = 1000;

	for (int i = 0; i < PARTITION_NUM; ++i) {
		ret = g_kfkhandler[i].init(BROKER, TOPIC, false);
		if (ret) {
			cerr << "KafkaHelper init err\n";
			return -1;
		}
	}

	for (int i = 0; i < PARTITION_NUM; ++i) {
		pthread_mutex_init(&g_lock[i], NULL);
	}

	g_msgs.resize(PARTITION_NUM);

	cout << "Init success!!" << endl;

	return 0;
}

int KafkaConsume::start()
{
	// create thread to process message & consume from kafka 
	for (int i = 0; i < PARTITION_NUM; ++i) {

		int *pidx = new int(i);
		if (pthread_create(&m_pthread[i], NULL, process, (void*)pidx) != 0) {
			return -1;
		}

		int *cidx = new int(i);
		if (pthread_create(&m_cthread[i], NULL, consume, (void*)cidx) != 0) {
			ILog::instance()->writeLog(LOG_LEV_ERROR, "create consume thread failed");
			return -1;
		}

		sleep(1);
	}

	while (g_loop) {
		sleep(1);
	}
	
   	return 0;
}

int KafkaConsume::Process(void *idx)
{
	int index = *((int *)idx);
	delete (int*)idx;

	redisContext *redis_ctxt = NULL;
	// if connect to redis failed, stop!
	assert(_connect_redis(&redis_ctxt) == 0);
	assert(redis_ctxt != NULL);

	while (g_loop) {
		pthread_mutex_lock(&g_lock[index]);
		if (g_msgs[index].size() > 1000) {
			vector<MSG> tpMsg;
			tpMsg.swap(g_msgs[index]);
			g_msgs[index].clear();
			pthread_mutex_unlock(&g_lock[index]);

			for (vector<MSG>::iterator it = tpMsg.begin();
				 it != tpMsg.end();
				 ++it) {
				_store_data(redis_ctxt, it);
			}
			continue;
		}
		pthread_mutex_unlock(&g_lock[index]);
		sleep(1);
	}

	if (redis_ctxt != NULL) {
		redisFree(redis_ctxt);
		redis_ctxt = NULL;
	}

    return 0;
}

int KafkaConsume::_store_data(redisContext *hredis, const vector<MSG>::iterator &iter)
{
	return 0;
}

void KafkaConsume::Quit()
{
	if (!g_released) {
		Release();
	}

	return;
}

int main(int argc, char **argv)
{
	KafkaConsume kfkc;
	kfkc.init();
	kfkc.start();
}
