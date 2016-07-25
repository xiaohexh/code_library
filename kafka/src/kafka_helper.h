#ifndef _KAFKA_HELPER_H_
#define _KAFKA_HELPER_H_

#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/time.h>
#include <errno.h>
#include <string>
extern "C"
{
#include "librdkafka/rdkafka.h"
}

using namespace std;

namespace bp{
typedef void (*msg_consum_cb)(void*,int );  //声明回调函数
/**
 * @brief       
 */
class KafkaHelper
{
public:
    /**
     * @brief constructor
     */
    KafkaHelper();
    /**
     * @brief destructor
     */
    ~KafkaHelper();

    /**
     * @brief the initialize function
     */
    int init(const string borkers, const string topic, bool isproduce=true);
	
	int produce(void *buf, int len,int partition=RD_KAFKA_PARTITION_UA);
	
	int consume_start(int partition, int start_offset);
	int msg_consume(int partition, int batch_size, msg_consum_cb callback, int time_out=1000/**ms*/);
	int msg_consume(int partition, msg_consum_cb callback, int time_out=1000/**ms*/);
	int consume_destroy( int partition);
	
	int conf_dump();
	static void msg_delivered (rd_kafka_t *rk,
					   void *payload, size_t len,
					   rd_kafka_resp_err_t errcode,
					   void *opaque, void *msg_opaque);
public:
	
	
private:	
	rd_kafka_topic_t *m_rkt;
	rd_kafka_t *m_rk;
	string m_str_brokers;
	string m_str_topic;
	rd_kafka_conf_t *m_conf;
	rd_kafka_topic_conf_t *m_topic_conf;
	
};
}
#endif  // _KAFKA_HELPER_H_

