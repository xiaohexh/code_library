#!/usr/bin/python
# -*- coding: utf-8 -*-

table_pre = 'weather_atom_'

table_num = 100

fd = open('weather_atom.sql', 'a')

for i in range(table_num):

	table_name = table_pre + str(i)

	fd.write('/*==============================================================*/\n')

	tstr = '/* Table: %s                                 		*/\n' % table_name
	fd.write(tstr)

	fd.write('/*==============================================================*/\n')

	tstr = 'create table %s\n' % table_name
	fd.write(tstr)

	fd.write('(\n')

	fd.write('id               int unsigned NOT NULL AUTO_INCREMENT,\n')
	fd.write('srcid            int,\n')
	fd.write('dt               date NOT NULL,\n')
	fd.write('gbcode           int  NOT NULL,\n');
	fd.write('maxt             int,\n');
	fd.write('mint             int,\n');
	fd.write('init_weather     int,\n');
	fd.write('end_weather      int,\n');
	fd.write('wind_power       int,\n');
	fd.write('wind_direction   int,\n');
	fd.write('ultraviolet      int,\n');
	fd.write('dress            int,\n');
	fd.write('tour             int,\n');
	fd.write('comfort          int,\n');
	fd.write('exercise         int,\n');
	fd.write('carwashing       int,\n');
	fd.write('dry              int,\n');
	fd.write('irritability     int,\n');
	fd.write('create_time      datetime,\n')
	fd.write('primary key (id),\n')
	fd.write('UNIQUE KEY (dt,gbcode)\n')
	fd.write(') DEFAULT CHARSET=utf8;\n\n')

fd.close()
