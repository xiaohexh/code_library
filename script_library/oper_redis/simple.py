#!/usr/bin/python
# -*- coding: utf-8 -*-

# 1) pre-install dependent libraries
# ubuntu os
# sudo apt-get install python-redis
# 2) check install success or not
# $python
# >>> import redis 
# No error happened

# for security, under production environment:
# DONOT use default port 6379;
# and highly recommend config "requirepass 'xxx'" in conf file.

import redis

#REDIS_HOST = '127.0.0.1'
REDIS_HOST = 'localhost'
REDIS_PORT = 6379

def read_redis():
	#r = redis.StrictRedis(host = REDIS_HOST, port = REDIS_PORT, db = 0)
	r = redis.Redis(host = REDIS_HOST, port = REDIS_PORT)

	# set key
	if r.set('foo', 'bar'):
		print "set 'foo' sucess"
	else:
		print "set 'foo' failed"

	# get # keys
	key_size = r.dbsize()
	print "There're %d keys" % key_size

	# iterate all the keys, only apply to all keys is str
	keys = r.keys()
	#for key in keys:
	#	print key + ":" + str(r.get(key))
	
	# delete key
	if r.exists('foo'):
		r.delete('foo')

	# pipeline
	r.set('num', 5)
	p = r.pipeline()
	p.set('foo', 'bar')
	p.incr('num')
	p.sadd('faz', 'baz')
	p.execute()

	print r.smembers('faz')

if __name__ == '__main__':
	read_redis()
