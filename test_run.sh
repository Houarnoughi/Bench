#!/bin/sh
for i in 150 300 450 600
do
./ubifs_launch.sh update 100 $i seq
done
 
