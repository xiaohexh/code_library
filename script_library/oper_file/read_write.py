#!/usr/bin/python
# -*- coding: utf-8 -*-

# write file
fd = open('test_rw.txt', 'a')
fd.write('/*==============================================================*/\n')
fd.close()

# read file
fd = open('test_rw.txt', 'r')

line = fd.readline()
print line

# print current read offset
print fd.tell()

# set read offset to the beginning of file
fd.seek(0)

lines = fd.readlines()
for line in lines:
    print line
fd.close()
