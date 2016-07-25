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
		ILog::instance()->writeLog(LOG_LEV_ERROR, "[%s %d]pb msg:%s parse failed",
									__FILE__, __LINE__, msg.c_str());
	    return;
	}
  
	string evt, bat_no, ver, wt;
	common::pair pt;
	// common head
	for (int j = 0; j < repMsg.field_size(); ++j) {
		pt = repMsg.field(j);
		if (pt.key() == "osp") {
			wt = pt.val();
		} else if (pt.key() == "apv") {
			ver = pt.val();
		}
    }

    // private info
	for (int i = 0; i < repMsg.data_size(); ++i) {
		bool coupon_click = true;
		all::sub_msg smsg = repMsg.data(i);

		for (int j = 0; j < smsg.field_size(); ++j) {
			pt = smsg.field(j);
			if (pt.key() == "cls") {
				evt = pt.val();

				if (evt != "CouponCenter_ToReceive"
					&& evt != "CouponGet_CouponSuccess") { // 找出优惠券的点击和领取

					coupon_click = false;
					break;
				}
			} else if (pt.key() == "clp") {
				bat_no = pt.val();
				int pos = bat_no.find_last_of("_");
				if (pos != -1) {
					bat_no = bat_no.substr(pos + 1);
				}
			}
		}
		if (!coupon_click) {
			continue;
		}

		MSG tmpM;
		if (evt == "CouponCenter_ToReceive") {
			tmpM.event_type = "coupontorecv";
		} else {
			tmpM.event_type = "couponsuccess";
		}

		tmpM.batch_no = bat_no;
		tmpM.wireless_type = wt;
		tmpM.version = ver;

		// format date & time
		time_t now = time(0);
        struct tm ttm;
        localtime_r(&now, &ttm);
        char date[16];
		memset(date, '\0', 16);
        snprintf(date, sizeof(date), "%04d-%02d-%02d",
                 ttm.tm_year+1900, ttm.tm_mon+1, ttm.tm_mday);
		tmpM.rpt_date = date;
		
        char hour[8];
		memset(hour, '\0', 8);
        snprintf(hour, sizeof(hour), "%02d", ttm.tm_hour);
		tmpM.rpt_hour = hour;

		// one click report store into MSG.
		pthread_mutex_lock(&g_lock[index]);
		g_msgs[index].push_back(tmpM);
		pthread_mutex_unlock(&g_lock[index]);

		//ILog::instance()->writeLog(LOG_LEV_INFO, "uid:%s, pin:%s, clp:%s",
		//						   tmpM.uid.c_str(), tmpM.pin.c_str(), tmpM.clp.c_str());
	}
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
	BROKER = "172.28.111.121:9092,172.28.111.122:9092,172.28.111.123:9092,172.28.111.124:9092,172.28.111.125:9092,172.28.111.126:9092,172.28.111.127:9092";

	//TOPIC = IConfig::instance()->getConfigStr("KAFKA", "TOPIC");
	TOPIC = "click_trim";

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

/**
 * eg:
 * coupon_type:		coupontorecv (or:couponsuccess)
 * batch_no:		10001
 * wireless_type:	ios (or:android/ipad)
 * version:			4.3.1
 * date:			2015-12-30
 * time:			14:23
 *
 * key_format: coupon_type|batch_no|wt|subversion|date|hour
 *
 * coupontorecv|-|-|-|-|2015-12-30|14
 * coupontorecv|-|IOS|-|2015-12-30|14
 * coupontorecv|-|IOS|4.3.1|2015-12-30|14
 * coupontorecv|10001|-|-|2015-12-30|-
 * coupontorecv|10001|-|-|2015-12-30|14
 * coupontorecv|10001|IOS|4.3.1|2015-12-30|-
 * coupontorecv|10001|IOS|4.3.1|2015-12-30|14
 */
int KafkaConsume::_store_data(redisContext *hredis, const vector<MSG>::iterator &iter)
{
	//ILog::instance()->writeLog(LOG_LEV_ERROR, "[%s %d]store data, et:%s bn:%s wt:%s ver:%s dt:%s hr:%s",
	//		__FILE__, __LINE__, iter->event_type.c_str(), iter->batch_no.c_str(), iter->wireless_type.c_str(), iter->version.c_str(), iter->rpt_date.c_str(), iter->rpt_hour.c_str());

	char key1[128] = {0};
	memset(key1, '\0', 128);
	snprintf(key1, 128, "%s|-|-|-|%s|%s",
			iter->event_type.c_str(), iter->rpt_date.c_str(), iter->rpt_hour.c_str());

	char key2[128] = {0};
	memset(key2, '\0', 128);
	snprintf(key2, 128, "%s|-|%s|-|%s|%s",
			iter->event_type.c_str(), iter->wireless_type.c_str(), iter->rpt_date.c_str(), iter->rpt_hour.c_str());

	char key3[128] = {0};
	memset(key3, '\0', 128);
	snprintf(key3, 128, "%s|-|%s|%s|%s|%s",
			iter->event_type.c_str(), iter->wireless_type.c_str(), iter->version.c_str(), iter->rpt_date.c_str(), iter->rpt_hour.c_str());

	char key4[128] = {0};
	memset(key4, '\0', 128);
	snprintf(key4, 128, "%s|%s|-|-|%s|-",
			iter->event_type.c_str(), iter->batch_no.c_str(), iter->rpt_date.c_str());

	char key5[128] = {0};
	memset(key5, '\0', 128);
	snprintf(key5, 128, "%s|%s|-|-|%s|%s",
			iter->event_type.c_str(), iter->batch_no.c_str(), iter->rpt_date.c_str(), iter->rpt_hour.c_str());

	char key6[128] = {0};
	memset(key6, '\0', 128);
	snprintf(key6, 128, "%s|%s|%s|%s|%s|-",
			iter->event_type.c_str(), iter->batch_no.c_str(), iter->wireless_type.c_str(), iter->version.c_str(), iter->rpt_date.c_str());

	char key7[128] = {0};
	memset(key7, '\0', 128);
	snprintf(key7, 128, "%s|%s|%s|%s|%s|%s",
			iter->event_type.c_str(), iter->batch_no.c_str(), iter->wireless_type.c_str(), iter->version.c_str(), iter->rpt_date.c_str(), iter->rpt_hour.c_str());


	if (unlikely(NULL == hredis)) {
		ILog::instance()->writeLog(LOG_LEV_ERROR, "[%s %d]before exe credisCommand hredis is NULL, this is lead to coredump", __FILE__, __LINE__);
		_connect_redis(&hredis);
	}

	/* increase key by one */
	//pt.reset();
	redisReply* reply1 = (redisReply*)redisCommand(hredis, "INCR %s", key1);
	redisReply* reply2 = (redisReply*)redisCommand(hredis, "INCR %s", key2);
	redisReply* reply3 = (redisReply*)redisCommand(hredis, "INCR %s", key3);
	redisReply* reply4 = (redisReply*)redisCommand(hredis, "INCR %s", key4);
	redisReply* reply5 = (redisReply*)redisCommand(hredis, "INCR %s", key5);
	redisReply* reply6 = (redisReply*)redisCommand(hredis, "INCR %s", key6);
	redisReply* reply7 = (redisReply*)redisCommand(hredis, "INCR %s", key7);
	//PerforCal("queryipinfo_Req2Redis", pt.used());
	
    if (NULL == reply1 || NULL == reply2 || NULL == reply3 || NULL == reply4
		|| NULL == reply5 || NULL == reply6 || NULL == reply7) {
		ILog::instance()->writeLog(LOG_LEV_ERROR, "[%s %d]credisCommand 'INCR' failed:reply is NULL", __FILE__, __LINE__);
		// close and reconnect to redis
		if (NULL != hredis) {
			redisFree(hredis);
			if (_connect_redis(&hredis) < 0) {
				if(NULL != reply1) freeReplyObject(reply1);
				if(NULL != reply2) freeReplyObject(reply2);
				if(NULL != reply3) freeReplyObject(reply3);
				if(NULL != reply4) freeReplyObject(reply4);
				if(NULL != reply5) freeReplyObject(reply5);
				if(NULL != reply6) freeReplyObject(reply6);
				if(NULL != reply7) freeReplyObject(reply7);
				return -1;
		    }
		}
    } 

    if(NULL != reply1) freeReplyObject(reply1);
    if(NULL != reply2) freeReplyObject(reply2);
    if(NULL != reply3) freeReplyObject(reply3);
    if(NULL != reply4) freeReplyObject(reply4);
    if(NULL != reply5) freeReplyObject(reply5);
    if(NULL != reply6) freeReplyObject(reply6);
    if(NULL != reply7) freeReplyObject(reply7);

	/* set expire time of key */
	reply1 = (redisReply*)redisCommand(hredis, "EXPIRE %s %d", key1, m_redis_expime);
	reply2 = (redisReply*)redisCommand(hredis, "EXPIRE %s %d", key2, m_redis_expime);
	reply3 = (redisReply*)redisCommand(hredis, "EXPIRE %s %d", key3, m_redis_expime);
	reply4 = (redisReply*)redisCommand(hredis, "EXPIRE %s %d", key4, m_redis_expime);
	reply5 = (redisReply*)redisCommand(hredis, "EXPIRE %s %d", key5, m_redis_expime);
	reply6 = (redisReply*)redisCommand(hredis, "EXPIRE %s %d", key6, m_redis_expime);
	reply7 = (redisReply*)redisCommand(hredis, "EXPIRE %s %d", key7, m_redis_expime);

	//PerforCal("queryipinfo_Req2Redis", pt.used());
	
    if (NULL == reply1 || NULL == reply2 || NULL == reply3 || NULL == reply4
		|| NULL == reply5 || NULL == reply6 || NULL == reply7) {
		ILog::instance()->writeLog(LOG_LEV_ERROR, "[%s %d]credisCommand 'INCR' failed:reply is NULL", __FILE__, __LINE__);
		// close and reconnect to redis
		if (NULL != hredis) {
			redisFree(hredis);
			if (_connect_redis(&hredis) < 0) {
				if(NULL != reply1) freeReplyObject(reply1);
				if(NULL != reply2) freeReplyObject(reply2);
				if(NULL != reply3) freeReplyObject(reply3);
				if(NULL != reply4) freeReplyObject(reply4);
				if(NULL != reply5) freeReplyObject(reply5);
				if(NULL != reply6) freeReplyObject(reply6);
				if(NULL != reply7) freeReplyObject(reply7);
		        return -1;
		    }
		}
    }

    if(NULL != reply1) freeReplyObject(reply1);
    if(NULL != reply2) freeReplyObject(reply2);
    if(NULL != reply3) freeReplyObject(reply3);
    if(NULL != reply4) freeReplyObject(reply4);
    if(NULL != reply5) freeReplyObject(reply5);
    if(NULL != reply6) freeReplyObject(reply6);
    if(NULL != reply7) freeReplyObject(reply7);

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
