#!/usr/bin/python
# -*- coding: utf-8 -*-

table_pre = 'current_weather_atom_'

table_num = 100

fd = open('current_weather_atom.sql', 'a')

for i in range(table_num):

	table_name = table_pre + str(i)

	fd.write('/*==============================================================*/\n')

	tstr = '/* Table: %s                                 	*/\n' % table_name
	fd.write(tstr)

	fd.write('/*==============================================================*/\n')

	tstr = 'create table %s\n' % table_name
	fd.write(tstr)

	fd.write('(\n')

	fd.write('id               int unsigned NOT NULL AUTO_INCREMENT,\n')
	fd.write('srcid            int,\n')
	fd.write('update_time      datetime not null,\n')
	fd.write('gbcode           int  NOT NULL,\n');
	fd.write('tempture         int,\n');
	fd.write('weather     	   int,\n');
	fd.write('wind_power       int,\n');
	fd.write('wind_direction   int,\n');
	fd.write('aqi   		   int,\n');
	fd.write('aqs   		   int,\n');
	fd.write('pm2_5   		   int,\n');
	fd.write('primary key (id),\n')
	fd.write('UNIQUE KEY (gbcode)\n')
	fd.write(') DEFAULT CHARSET=utf8;\n\n')

fd.close()
