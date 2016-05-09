#!/usr/bin/python
# -*- coding: utf-8 -*-

import os 
import time 
 
for fileName in os.listdir ( './' ): 
	print fileName 

os.mkdir('testDirectory')

time.sleep(4)

os.rmdir('testDirectory') 

# create multi-layer directory
#os.makedirs( 'I/will/show/you/how/deep/the/rabbit/hole/goes' ) 
