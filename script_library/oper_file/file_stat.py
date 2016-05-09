#!/usr/bin/python
# -*- coding: utf-8 -*-

import os 
import stat 
import time
 
file_stats = os.stat('file_stat.py') 

file_info = { 
	'Size':file_stats[stat.ST_SIZE], 
	'LastModified':time.ctime(file_stats[stat.ST_MTIME]), 
	'LastAccessed':time.ctime(file_stats[stat.ST_ATIME]), 
	'CreationTime':time.ctime(file_stats[stat.ST_CTIME]), 
	'Mode':file_stats[stat.ST_MODE]
}
  
for info_field in file_info.keys(): 
	print info_field
	print file_info[info_field]

if stat.S_ISDIR(file_stats[stat.ST_MODE]): 
	print 'Directory. ' 
else: 
	print 'Non-directory.'  
