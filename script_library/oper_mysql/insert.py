#!/usr/bin/python
# -*- coding: utf-8 -*-

import MySQLdb  

db = MySQLdb.connect(host='192.168.144.76',user='root',passwd='root000',port=3306,charset='utf8')  

cur =db.cursor();

cur.execute('use test_env_portrait')

table_name = 'current_weather'

country = 'cn'
province = 'js'
city = 'xz'

insert_cmd = 'insert into %s (country, province, city) values("%s", "%s", "%s")' % (table_name, country, province, city)

cur.execute(insert_cmd)

cur.close()  
db.close()  
