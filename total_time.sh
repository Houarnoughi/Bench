#!/bin/sh

# USAGE: ./total_time.sh <request_type> <request_size>

if [ $# -lt 2 ];
then
	echo "USAGE: $0 <request_type> <request_size>"
	exit 0
fi

for i in 100 200 300 400 500 600 700 800 900 1000 1100 1200 1300 1400 1500 1600 1700 1800 1900 2000 2100 2200 2300 2400 2500 2600 2700 2800 2900 3000 3100 3200 3300 3400 3500 3600 3700 3800 3900 4000 4100 4200 4300 4400 4500 4600 4700 4800 4900 5000
do
	if [ -e "$1"_time_"$i"_"$2".dat ];
	then
		echo "$i;`awk -F \";\" 'BEGIN {SUM = 0}{SUM += $2} END {print SUM}' "$1"_time_"$i"_"$2".dat`" >> "$1"_"$2"_total_time.csv
	else
		echo "Missing file: "$1"_time_"$i"_"$2".dat"
	fi
done
