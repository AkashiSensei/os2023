#!/bin/bash

line_numbers=(32 128 512 1024)
sed -n 8p $1 > $2
for line_number in ${line_numbers[@]}
do
	sed -n "${line_number}p" $1 >> $2
done
