#!/bin/bash

mkdir mydir
chmod 777 mydir
echo 2023 > myfile
mv moveme ./mydir/
cp copyme ./mydir/copied
cat readme
gcc bad.c 2>err.txt
if [ $# -eq 0 ];then
	num=10
else
	num=$1
	fi
i=1
mkdir gen
while [ $i -le $num ]
do
	touch "./gen/$i.txt"
	i=$((i+1))
done
