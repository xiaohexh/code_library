#!/usr/bin/python
# -*- coding: utf-8 -*-

table_pre = 'disaster_warning_atom_'

table_num = 100

fd = open('disaster_warning_atom.sql', 'a')

for i in range(table_num):

	table_name = table_pre + str(i)

	fd.write('/*==============================================================*/\n')

	tstr = '/* Table: %s                                 	*/\n' % table_name
	fd.write(tstr)

	fd.write('/*==============================================================*/\n')

	tstr = 'create table %s\n' % table_name
	fd.write(tstr)

	fd.write('(\n')

	fd.write('id                   int unsigned NOT NULL AUTO_INCREMENT,\n')
	fd.write('srcid                int,\n')
	fd.write('start_time           datetime not null,\n')
	fd.write('end_time             datetime,\n')
	fd.write('gbcode               int not null,\n')
	fd.write('warning_type         int,\n')
	fd.write('warning_level        int,\n')
	fd.write('is_outtime           bool,\n')
	fd.write('create_time          datetime,\n')
	fd.write('UNIQUE KEY (gbcode, warning_type),\n')
	fd.write('primary key (id)\n')
	fd.write(') DEFAULT CHARSET=utf8;\n\n')

fd.close()
