#!/usr/bin/python
# -*- coding: utf-8 -*-

import MySQLdb  

db = MySQLdb.connect(host='192.168.144.76',user='root',passwd='root000',port=3306,charset='utf8')  

cur =db.cursor();

cur.execute('use figure_model_environment')

table_pre = 'disaster_warning_atom_'

table_num = 100

for i in range(table_num):

	table_name = table_pre + str(i)

	cmd = 'drop table if exists ' + table_name

	cur.execute(cmd)
          
cur.close()  
db.close()  
