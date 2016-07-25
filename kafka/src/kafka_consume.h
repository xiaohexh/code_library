#ifndef _COUPON_PROCESS_H_
#define _COUPON_PROCESS_H_

#include <string>
#include <vector>
#include <map>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>

#include "kafka_helper.h"

#define CONFIG_FILE			"../etc/couponreald.ini"
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

struct MSG
{
	string event_type;      // CouponCenter_ToReceive/CouponGet_CouponSuccess
	string batch_no;
	string rpt_date;
	string rpt_hour;
    string wireless_type;   // iphone/android/ipad
    string version;         // v4.2.1/v4.3.3/...
};

struct tpidx
{
	int thread_idx; // thread index
	int part_idx;   // partition index
};
 
 
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
	~KafkaConsume();
	int Init();
	int Start();
    int Process(void *idx);
	void Quit();

private:
    //业务辅助函数
	inline int _connect_redis(redisContext **redis_ctxt);
    int _store_data(redisContext *hredis, const vector<MSG>::iterator &iter);
    string _gen_md5(string origin_str);

	void Release();

private:

    //GUID数据结构
    long   m_uid_bucketsize;
    long   m_uid_nodesize;
    long   m_uid_chunks;
    long   m_uid_chunksize;

	std::string m_redis_host;
	int			m_redis_port;
	int			m_redis_expime;
	pthread_t	m_cthread[PARTITION_NUM];
	pthread_t	m_pthread[PARTITION_NUM];
};

#endif
