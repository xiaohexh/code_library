#!/usr/bin/python
# -*- coding: utf-8 -*-

# there is some bug when read from file
'''
import sys

if len(sys.argv) < 2:
	print "usage: python chinese_decode.py file"
	exit(1)
 
file_name = sys.argv[1]

# read file
fd = open(file_name, 'r')

lines = fd.readlines()
for line in lines:
	print line
	print line.decode('utf-8')

fd.close()
'''
#上海
#上海市
#未知
print "\xe4\xb8\x8a\xe6\xb5\xb7".decode('utf-8')
print "\xe4\xb8\x8a\xe6\xb5\xb7\xe5\xb8\x82".decode('utf-8')
print "\xe6\x9c\xaa\xe7\x9f\xa5".decode('utf-8')
