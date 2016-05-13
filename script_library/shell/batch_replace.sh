#!/bin/bash

# 将文件1.txt内的文字“garden”替换成“mirGarden”

sed -i "s/garden/mirGarden/g" 1.txt

# 将当前目录下的所有文件内的“garden”替换成“mirGarden”

sed -i "s/garden/mirGarden/g" `ls` # `ls` got multiple files
