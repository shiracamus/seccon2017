#!/usr/bin/env ptyhon2
# -*- coding:utf-8 -*-

hello_world = 'Hello, World!'
flag4 = open('flag4.txt').read()

data = chr(0) + hello_world[::-1] + chr(0) + hello_world + chr(1)
data = data.ljust(128, '\n')
data += (flag4[::-1] + '  ').rjust(128, '\n')
print data
