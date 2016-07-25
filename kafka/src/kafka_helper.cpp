/**
	author: tellenji
	date:	2014-8-28
 */
 
#include "kafka_helper.h"
#include <string.h>

using namespace std;
using namespace bp;


KafkaHelper::KafkaHelper()
{
	m_topic_conf = NULL;
	m_conf = NULL;
	m_rkt = NULL;
	m_rk = NULL;
	//is_consume = false;
}

KafkaHelper::~KafkaHelper()
{
	if (m_rk) {
		/* Destroy the handle */
		rd_kafka_destroy(m_rk);
	}
}

//init for one topic to produce msg
int KafkaHelper::init(const string borkers,const string topic, bool isproduce)
{
    int ret = 0;
	char errstr[512];
	
	m_str_brokers = borkers;
	m_str_topic = topic;
	/* Kafka configuration */
	m_conf = rd_kafka_conf_new();

	/* Topic configuration */
	m_topic_conf = rd_kafka_topic_conf_new();
	/* Set up a message delivery report callback.
	 * It will be called once for each message, either on successful
	 * delivery to broker, or upon failure to deliver to broker. */
	rd_kafka_conf_set_dr_cb(m_conf, msg_delivered);

	if (isproduce) {
		/* Create Kafka handle */
		if (!(m_rk = rd_kafka_new(RD_KAFKA_PRODUCER, m_conf,
					errstr, sizeof(errstr)))) {
			fprintf(stderr,"%% Failed to create new producer: %s\n",errstr);
			return -1;
		}
	}
	else {
		/* Create Kafka handle */
		if (!(m_rk = rd_kafka_new(RD_KAFKA_CONSUMER, m_conf,
					errstr, sizeof(errstr)))) {
			fprintf(stderr,"%% Failed to create new consumer: %s\n",errstr);
			return -1;
		}
	
	}
	/* Set logger */
	//rd_kafka_set_logger(rk, logger);
	//rd_kafka_set_log_level(rk, LOG_DEBUG);

	/* Add brokers */
	//list brokers if one fail we can choose another
	if (rd_kafka_brokers_add(m_rk, m_str_brokers.c_str()) == 0) {
		fprintf(stderr, "No valid brokers specified\n");
		return -1;
	}

	/* Create topic */
	m_rkt = rd_kafka_topic_new(m_rk, m_str_topic.c_str(), m_topic_conf);

	fprintf(stderr, "KAFKA INIT OK\n");
	
	return ret;
}

/**
 * Message delivery report callback.
 * Called once for each message.
 * See rdkafka.h for more information.
 */
void KafkaHelper::msg_delivered (rd_kafka_t *rk,
					   void *payload, size_t len,
					   rd_kafka_resp_err_t error_code,
					   void *opaque, void *msg_opaque) {

	if (error_code) {
		printf("Message delivery failed: %s\n",
		       rd_kafka_err2str(error_code));
	}
	else {
		printf("Message delivered (%zd bytes)\n", len);
	}
}

//--------------------------------------------------------------------------------------------
int KafkaHelper::produce(void *buf, int len,int partition)
{
	if (!m_rkt) {
		m_rkt = rd_kafka_topic_new(m_rk, m_str_topic.c_str(), m_topic_conf);
	}
	if (!m_rkt) {
		return -1;
	}
	/* Send/Produce message. */
	rd_kafka_produce(m_rkt, partition,
			 RD_KAFKA_MSG_F_COPY,
			 /* Payload and length */
			 buf, len,
			 /* Optional key and its length */
			 NULL, 0,
			 /* Message opaque, provided in
			  * delivery report callback as
			  * msg_opaque. */
			 NULL);
	fprintf(stderr, "Sent %d bytes to topic "
		"%s partition %i\n",
		len, rd_kafka_topic_name(m_rkt), partition);

	/* Poll to handle delivery reports */
	rd_kafka_poll(m_rk, 10);
	
	return 0;
}

//--------------------------------------------------------------------------------------------
int KafkaHelper::consume_start(int partition, int start_offset)
{
	/* Create Kafka handle */

	/* Set logger */

	/* Add brokers */


	/* Create topic */

	/* Start consuming */
	if (rd_kafka_consume_start(m_rkt, partition, start_offset) == -1){
		fprintf(stderr, "Failed to start consuming: %s\n", strerror(errno));
		return errno;
	}
	
	return 0;
}

//--------------------------------------------------------------------------------------------
int KafkaHelper::consume_destroy(int partition)
{
	/* Stop consuming */
	rd_kafka_consume_stop(m_rkt, partition);

	rd_kafka_topic_destroy(m_rkt);

	rd_kafka_destroy(m_rk);
	
	return 0;
}

int KafkaHelper::msg_consume(int partition, int batch_size, msg_consum_cb callback, int time_out){
	if(batch_size){
		rd_kafka_message_t **rkmessages = NULL;
		rkmessages = (rd_kafka_message_t **)malloc(sizeof(*rkmessages) * batch_size);
		int i;
		int r = rd_kafka_consume_batch(m_rkt, partition,
							   time_out,
							   rkmessages,
							   batch_size);
		if (r != -1) {
			for (i = 0 ; i < r ; i++) {
				callback(rkmessages[i]->payload,rkmessages[i]->len);
				rd_kafka_message_destroy(rkmessages[i]);
			}
		}

		if (rkmessages)
			free(rkmessages);
	}
	return 0;
}

int KafkaHelper::msg_consume(int partition, msg_consum_cb callback, int time_out) 
{
	rd_kafka_message_t *rkmessage;

	/* Consume single message.
	 * See rdkafka_performance.c for high speed
	 * consuming of messages. */
	rkmessage = rd_kafka_consume(m_rkt, partition, time_out);
	if (!rkmessage) { /* timeout */
		return 0;
	}
	
	if (rkmessage->err) {
		if (rkmessage->err == RD_KAFKA_RESP_ERR__PARTITION_EOF) {
			printf("Consumer reached end of %s [%d] "
			       "message queue at offset %ld\n",
			       rd_kafka_topic_name(rkmessage->rkt),
			       rkmessage->partition, rkmessage->offset);
			return rkmessage->err;
		}

		printf("Consume error for topic \"%s\" [%d] "
		       "offset %ld: %s\n",
		       rd_kafka_topic_name(rkmessage->rkt),
		       rkmessage->partition,
		       rkmessage->offset,
		       rd_kafka_message_errstr(rkmessage));
   
		return rkmessage->err;
	}


	callback(rkmessage->payload, rkmessage->len);
	//hexdump(stdout, "Message Payload", rkmessage->payload, rkmessage->len);
	/* Return message to rdkafka */
	rd_kafka_message_destroy(rkmessage);
	
	return  0;
}

//--------------------------------------------------------------------------------------------
int KafkaHelper::conf_dump()
{
	return 0;
}


