#!/bin/bash

HOSTNAME="127.0.0.1"
PORT="3306"
USERNAME="root"
PWD="root000"
 

DBNAME="figure_model_environment"
TABLENAME_PRE="weather_atom"

MYSQL_CMD="mysql -h${HOSTNAME}  -P${PORT}  -u${USERNAME} -p${PWD}"
echo ${MYSQL_CMD}
   
# create sub-table weather_atom_0~99
for((i=0;i<100;i++))
do
    TABLENAME=${TABLENAME_PRE}_$i
    create_table_sql="create table ${TABLENAME}(
                      id                int unsigned NOT NULL AUTO_INCREMENT,
                      srcid             int,
                      updateTime        datetime not null,
                      gbcode            int  not null,
                      PRIMARY KEY (id),
                      UNIQUE KEY (gbcode)
                      )ENGINE=MyISAM DEFAULT CHARSET=utf8"

    echo ${create_table_sql} | ${MYSQL_CMD} ${DBNAME}
	if [ $? -ne 0 ]; then
        echo "create  table ${DBNAME}.${TABLENAME}  fail ..."
    fi  
done
