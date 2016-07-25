#!/bin/bash
#FileName: filestat.sh
# statistic number of variable types of files

if [ $# -ne 1 ]; then
     echo $0 bashpath
     echo
fi
path=$1
declare -A statarray
while read line
do
     ftype=`file -b "$line"`
     let statarray["$ftype"]++
done< <(find $path -type f -print)
echo ========= File Type And Counts ===========
for ftype in "${!statarray[@]}"
do
     echo $ftype : ${statarray["$ftype"]}
done

