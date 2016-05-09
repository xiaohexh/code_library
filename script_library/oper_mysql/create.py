#!/usr/bin/python
# -*- coding: utf-8 -*-

import MySQLdb  

db = MySQLdb.connect(host='192.168.144.76',user='root',passwd='root000',port=3306,charset='utf8')  

cur = db.cursor();

cur.execute('use figure_model_environment')

table_pre = 'weather_atom_'

table_num = 100

for i in range(table_num):

	table = table_pre + str(i)

	cmd_pre = 'create table if not exists '
	create_cmd = cmd_pre + table + '									\
		(																\
			id               int  unsigned NOT NULL AUTO_INCREMENT,		\
			srcid            int,										\
			dt               date NOT NULL,								\
			gbcode           int  NOT NULL,								\
			maxt             int,										\
			mint             int,										\
			init_weather     int,										\
			end_weather      int,										\
			wind_power       int,										\
			wind_direction   int,										\
			ultraviolet      int,										\
			dress            int,										\
			tour             int,										\
			comfort          int,										\
			exercise         int,										\
			carwashing       int,										\
			dry              int,										\
			irritability     int,										\
			create_time      datetime,									\
			PRIMARY KEY (id),											\
			UNIQUE KEY (dt,gbcode)										\
		) DEFAULT CHARSET=utf8;'										

	cur.execute(create_cmd)

cur.close()  
db.close()  
