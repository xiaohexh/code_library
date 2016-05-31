#!/bin/bash

#array_var=(1 2 3 4 5 6)

array_var[0]="test1"
array_var[1]="test2"
array_var[2]="test3"

printf "array_var[%d]=%s\n" 2 ${array_var[2]}

idx=1
printf "array_var[%d]=%s\n" $idx ${array_var[$idx]}

# array length
echo ${#array_var[*]}
