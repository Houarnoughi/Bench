#!/bin/sh

#if [ ! -d NFS/ubifs_results/update ];
#then
	
	#mkdir NFS/ubifs_results
	#mkdir NFS/ubifs_results/update
#fi

if [ ! -d NFS/jffs2_results/update ];
then
	
	mkdir NFS/jffs2_results
	mkdir NFS/jffs2_results/update
fi		

# UBIFS all tests
#for i in $(seq 20)
#do
#	for j in 150 300 450 600
#	do
#		./ubifs_mesure.sh update $j $i
#	done
#done

# JFFS2 all tests
for i in $(seq 20)
do	
	for j in 150 300 450 600
	do
		./jffs2_mesure.sh update $j $i
	done
done
