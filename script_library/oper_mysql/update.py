#!/usr/bin/python
# -*- coding: utf-8 -*-

import MySQLdb  

db = MySQLdb.connect(host='192.168.144.76',user='root',passwd='root000',port=3306,charset='utf8')  

cur =db.cursor();

cur.execute('use test_env_portrait')

table_name = 'current_weather'
province = 'js'
old_city = 'xz'
new_city = 'sz'

cmd = 'update %s set city = "%s" where province = "%s" and city = "%s"' % (table_name, new_city, province, old_city)

cur.execute(cmd)
          
cur.close()  
db.close()  
