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
bool                    g_stop = false;

vector<vector<string> >	g_msgs;
KafkaHelper             *g_kfkhandler = NULL;
pthread_mutex_t         *g_lock = NULL;

int                     partitions = 0; // kafka partions
int                     batch_size = 0; // kafka 批量取个数
string                  topic;      // kafka topic
string                  broker;     // kafak broker

void consume_callback(void* buff, int size)
{
	if (0 == size) {
		return;
	}

    string msg((char*)buff, size);

	int index = random(partitions);
  
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

	while (!g_stop) {
		ret = g_kfkhandler[index].msg_consume(index, batch_size, consume_callback);
	}

	return NULL;
}

KafkaConsume::KafkaConsume()
	: m_cthread(NULL)
{
}

void KafkaConsume::release()
{
	g_stop = true;

	if (g_kfkhandler != NULL) {
		for (int i = 0; i < partitions; ++i) {
			g_kfkhandler[i].consume_destroy(i);
		}

		delete [] g_kfkhandler;
		g_kfkhandler = NULL;
	}

	// join all thread
	if (m_cthread != NULL) {
		for (int i = 0; i < partitions; ++i) {
			pthread_join(m_cthread[i], NULL);
		}
		delete [] m_cthread;
		m_cthread = NULL;
	}

	pthread_join(m_pthread, NULL);

	if (g_lock != NULL) {
		for (int i = 0; i < partitions; ++i) {
			pthread_mutex_destroy(&g_lock[i]);
		}
		delete [] g_lock;
		g_lock = NULL;
	}
}

int KafkaConsume::init()
{
	int ret = 0;

	/*初始化kafka*/
	broker = MyConfig::instance()->getConfigStr("KAFKA", "BROKER");
	log_debug(LOG_DEBUG, "kafka broker:%s", broker.c_str());
	//BROKER = "172.8.11.12:9092,172.8.11.22:9092,172.8.11.23:9092,172.28.11.24:9092,172.8.11.25:9092,172.8.11.26:9092,172.8.11.27:9092";

	topic = MyConfig::instance()->getConfigStr("KAFKA", "TOPIC");
	log_debug(LOG_DEBUG, "kafka topic:%s", topic.c_str());
	//TOPIC = "uv";

	batch_size = MyConfig::instance()->getConfigInt("KAFKA", "BATCH_SIZE");
	log_debug(LOG_DEBUG, "kafka batch size:%d", batch_size);
	//BATCH_SIZE = 1000;

	partitions = MyConfig::instance()->getConfigInt("KAFKA", "PARTITION_NUM");
	log_debug(LOG_DEBUG, "kafka partition number:%d", partitions);
	assert(partitions > 0);

	g_kfkhandler = new KafkaHelper[partitions];
	if (g_kfkhandler == NULL) {
		log_error("allocate KafkaHelper failed");
		return -1;
	}

	for (int i = 0; i < partitions; ++i) {
		ret = g_kfkhandler[i].init(broker, topic, false);
		if (ret) {
			log_error("KafkaHelper init failed");
			return -1;
		}
	}

	m_cthread = new pthread_t[partitions];
	if (m_cthread == NULL) {
		log_error("allocate consume threads failed");
		return -1;
	}

	g_lock = new pthread_mutex_t[partitions];
	if (g_lock == NULL) {
		log_error("allocate mutex failed");
		return -1;
	}

	for (int i = 0; i < partitions; ++i) {
		pthread_mutex_init(&g_lock[i], NULL);
	}

	g_msgs.resize(partitions);

	cout << "Init success!!" << endl;

	return 0;
}

void sigterm_handler(int signo)
{
    log_debug(LOG_DEBUG, "recv stop signal, I will exit!\n");
	g_stop = true;
}

void setup_signal_handler(void)
{
    struct sigaction sa; 

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigterm_handler;
    sigaction(SIGINT, &sa, NULL);
}

int KafkaConsume::start()
{

	setup_signal_handler();

	// create thread to process message
	if (pthread_create(&m_pthread, NULL, process, NULL) != 0) {
		return -1;
	}

	// create thread to consume from kafka 
	for (int i = 0; i < partitions; ++i) {
		int *cidx = new int(i);
		if (pthread_create(&m_cthread[i], NULL, consume, (void*)cidx) != 0) {
			return -1;
		}

		sleep(1);
	}

	while (!g_stop) {
		sleep(1);
	}

   	return 0;
}

int KafkaConsume::Process()
{
	int index;

	while (!g_stop) {
		for (index = 0; index < partitions; index++) {
			pthread_mutex_lock(&g_lock[index]);
			if (g_msgs[index].size() > 1000) {
				vector<string> tpMsg;
				tpMsg.swap(g_msgs[index]);
				g_msgs[index].clear();
				pthread_mutex_unlock(&g_lock[index]);

				for (vector<string>::iterator it = tpMsg.begin();
					 it != tpMsg.end();
				 	 ++it) {
					_store_data(*it);
				}
				continue;
			}
			pthread_mutex_unlock(&g_lock[index]);
		}
		sleep(1);
	}

    return 0;
}

int KafkaConsume::_store_data(const std::string &msg)
{
	log_debug(LOG_DEBUG, "%s", msg.c_str());
	return 0;
}

int main(int argc, char **argv)
{
	int status;

	KafkaConsume kfkc;

	status = log_init(LOG_DEBUG, LOG_FILE);
	if (status < 0) {
		log_stderr("log init failed\n");
		goto err;
	}
	
	status = MyConfig::instance()->loadConfigFile(CONFIG_FILE);
	if (status < 0) {
		log_error("load config file failed\n");
		goto err;
	}

	status = kfkc.init();
	if (status < 0) {
		log_error("init failed\n");
		goto err;
	}

	kfkc.start();

	kfkc.release();

err:
	kfkc.release();
	log_deinit();

	return 0;
}
