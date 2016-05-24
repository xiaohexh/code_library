'''
	This program shows how to invoke C/C++ function
	that's in dll/so lib with python.
'''
#!/usr/bin/python
# -*- coding: utf-8 -*-

from ctypes import *
import os

libmath = cdll.LoadLibrary(os.getcwd() + '/libmath_example.so')
print libmath.add(2, 2)
