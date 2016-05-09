#!/usr/bin/python
# -*- coding: utf-8 -*-

import MySQLdb  

db = MySQLdb.connect(host='192.168.144.76',user='root',passwd='root000',port=3306,charset='utf8')  

cur = db.cursor();

cur.execute('use test_env_portrait') # set database name

table_name = 'current_weather'

cmd = 'delete from ' + table_name + ' where country = "cn" and city = "bj"';

cur.execute(cmd)
          
cur.close()  
db.close()  
