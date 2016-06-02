#!/usr/bin/python
# -*- coding: utf-8 -*-

import MySQLdb  

db = MySQLdb.connect(host='192.168.144.76',user='root',passwd='root000',port=3306,charset='utf8')  

cur =db.cursor();

cur.execute('use figure_model_environment')

#table_pre = 'disaster_warning_atom_'
table_pre = 'future_weather_atom_'

table_num = 100
wind_power = 5

for i in range(table_num):

	table_name = table_pre + str(i)

	#select_cmd = 'select * from %s where wind_power > %d' % (table_name, wind_power)
	select_cmd = 'select * from %s' % (table_name)

	cur.execute(select_cmd)

	for row in cur.fetchall():  
		print i
		print row  
          
cur.close()  
db.close()  
