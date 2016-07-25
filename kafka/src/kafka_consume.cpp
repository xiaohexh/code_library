#include "kafka_consume.h"

#include <assert.h>
#include <iostream>
#include <string>

#include "Singleton.h"

#define random(x) (rand()%x)

/* 
 * 为每一个分区分配一个线程，消费分区里的数据
 * 为每一个分区分配一个容器，所有的容器都保存在g_msgs
 * 为每一个分区分配一个锁，用于消费和生成同步
 * 分配一个处理数据的线程，用于处理所有容器中的数据，
   如果数据量比较大，可以单独为每个容器分配一个处理线程
 */

/* global veriables definition */
bool					g_released = false;
bool                    g_loop = true;
 
vector<vector<string> >	g_msgs;
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

	int index = random(PARTITION_NUM);
  
	pthread_mutex_lock(&g_lock[index]);
	g_msgs[index].push_back(msg);
	pthread_mutex_unlock(&g_lock[index]);
}

void* process(void *arg)
{
	Singleton<KafkaConsume>::instance()->Process();
	return NULL;
}

void* consume(void* idx)
{
	int index = *((int*)idx);
	delete (int*)idx;

	int ret;
	ret = g_kfkhandler[index].consume_start(index, -1000);
	if (ret) {
		return NULL;
	}

	while (g_loop) {
		ret = g_kfkhandler[index].msg_consume(index, BATCH_SIZE, consume_callback);
	}

	return NULL;
}

KafkaConsume::KafkaConsume()
{
}

void KafkaConsume::release()
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

	pthread_join(m_pthread, NULL);

	for (int i = 0; i < PARTITION_NUM; ++i) {
		pthread_mutex_destroy(&g_lock[i]);
	}
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
	// create thread to process message
	if (pthread_create(&m_pthread, NULL, process, NULL) != 0) {
		return -1;
	}

	// create thread to consume from kafka 
	for (int i = 0; i < PARTITION_NUM; ++i) {
		int *cidx = new int(i);
		if (pthread_create(&m_cthread[i], NULL, consume, (void*)cidx) != 0) {
			return -1;
		}

		sleep(1);
	}

	while (g_loop) {
		sleep(1);
	}
	
   	return 0;
}

int KafkaConsume::Process()
{
	int index;

	while (g_loop) {
		for (index = 0; index < PARTITION_NUM; index++) {
			pthread_mutex_lock(&g_lock[index]);
			if (g_msgs[index].size() > 1000) {
				vector<string> tpMsg;
				tpMsg.swap(g_msgs[index]);
				g_msgs[index].clear();
				pthread_mutex_unlock(&g_lock[index]);

				for (vector<string>::iterator it = tpMsg.begin();
					 it != tpMsg.end();
				 	 ++it) {
					_store_data();
				}
				continue;
			}
			pthread_mutex_unlock(&g_lock[index]);
		}
		sleep(1);
	}

    return 0;
}

int KafkaConsume::_store_data()
{
	return 0;
}

int main(int argc, char **argv)
{
	KafkaConsume kfkc;
	kfkc.init();
	kfkc.start();
	kfkc.release();

	return 0;
}
