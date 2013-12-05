#!/bin/sh

#-----------------------------------------------------------------------
#	Description: Test for insert queries
#	Usage: ./launch.sh <req_type> <nb_rec> <avg_rec_sz> <seq/rand>
#	Author: Hamza Ouarnoughi
#-----------------------------------------------------------------------

REQ_TYPE=$1
NB_REC=$2
SZ_REC=$3
PAT_ACC=$4
PART_NUM=5
MNT_POINT=/mnt/flash
DEV_PART=/dev/mtd"$PART_NUM"
BDEV_PART=/dev/mtdblock"$PART_NUM"
FMLOG=/proc/flashmon_log
FMCMD=/proc/flashmon
DEST=NFS/API_test/ubifs
MAX_REC=6000

if [ $# -lt 2 ];
then
	echo "USAGE: $0 <req_type> <nb_rec> <avg_rec_sz> <seq/rand>"
	exit 0
fi

# Prepare the test environnement 
_prepare ()
{
	echo "Format part" 
	ubiformat /dev/mtd5 > /dev/null
	echo "Mount part" 
	umount $MNT_POINT
	ubiattach /dev/ubi_ctrl -m 5 -O 2048
	ubimkvol /dev/ubi0 -N user -m
	mount -t ubifs ubi0:user $MNT_POINT
	sleep 10
	echo "Insert Flashmon" 
	insmod flashmon.ko TRACED_PART=5 LOG_MODE=921600
	echo "Reset flashmon" 
	echo reset > $FMCMD
}

# Intilize VFS and flasmon
_init ()
{
	# Flush the page cache
	sync
	echo 3 > /proc/sys/vm/drop_caches
	echo reset > $FMCMD
}


_prepare
# Request execution

if [[ "$REQ_TYPE" != "insert" ]];
then

	if [[ "$REQ_TYPE" == "join" ]];
	then
	echo "Create database and insert records ..."
	./tab_create $MNT_POINT/database.db table_1 $SZ_REC
	./tab_create $MNT_POINT/database.db table_2 $SZ_REC
	_init
	./benchdb $MNT_POINT/database.db insert table_1 $MAX_REC $SZ_REC $PAT_ACC
	./benchdb $MNT_POINT/database.db insert table_2 $MAX_REC $SZ_REC $PAT_ACC
	cat $FMLOG > $DEST/$REQ_TYPE/insert_"$MAX_REC"_"$SZ_REC".dat

	# Dumping the part
	#cat $DEV_PART > $DEST/$REQ_TYPE/dump_"$NB_REC"_"$SZ_REC".dat

	for i in 100 200 300 400 500 600 700 800 900 1000 1100 1200 1300 1400 1500 1600 1700 1800 1900 2000 2100 2200 2300 2400 2500 2600 2700 2800 2900 3000 3100 3200 3300 3400 3500 3600 3700 3800 3900 4000 4100 4200 4300 4400 4500 4600 4700 4800 4900 5000
	do
	echo "Begin request ..."
	_init
	./benchdb $MNT_POINT/database.db $REQ_TYPE table_1 $i $SZ_REC $PAT_ACC > /dev/null
	echo "End request ..."

	# Save trace (in this case it's saved in NFS file)
	cat $FMLOG > $DEST/$REQ_TYPE/"$REQ_TYPE"_"$i"_"$SZ_REC".dat

	# Save time stamping file and sql script
	mv "$REQ_TYPE"_time.dat $DEST/$REQ_TYPE/"$REQ_TYPE"_time_"$i"_"$SZ_REC".dat
	done

	# Remove tracer
	rmmod flashmon.ko
	
	# Remove database
	rm $MNT_POINT/database.db
	
	# unmout parition 
	umount $MNT_POINT
	
	exit 0
	fi

echo "Create database and insert records ..."
./tab_create $MNT_POINT/database.db table_1 $SZ_REC
_init
./benchdb $MNT_POINT/database.db insert table_1 $MAX_REC $SZ_REC $PAT_ACC
cat $FMLOG > $DEST/$REQ_TYPE/insert_"$MAX_REC"_"$SZ_REC".dat

# Dumping the part
#cat $DEV_PART > $DEST/$REQ_TYPE/dump_"$NB_REC"_"$SZ_REC".dat

for i in 100 200 300 400 500 600 700 800 900 1000 1100 1200 1300 1400 1500 1600 1700 1800 1900 2000 2100 2200 2300 2400 2500 2600 2700 2800 2900 3000 3100 3200 3300 3400 3500 3600 3700 3800 3900 4000 4100 4200 4300 4400 4500 4600 4700 4800 4900 5000
do
echo "Begin request ..."
_init
./benchdb $MNT_POINT/database.db $REQ_TYPE table_1 $i $SZ_REC $PAT_ACC > /dev/null
echo "End request ..."

# Save trace (in this case it's saved in NFS file)
cat $FMLOG > $DEST/$REQ_TYPE/"$REQ_TYPE"_"$i"_"$SZ_REC".dat

# Save time stamping file and sql script
mv "$REQ_TYPE"_time.dat $DEST/$REQ_TYPE/"$REQ_TYPE"_time_"$i"_"$SZ_REC".dat
done

# Remoce tracer
rmmod flashmon.ko

# Remove database
rm $MNT_POINT/database.db

# unmout parition 
umount $MNT_POINT

else

for i in 100 200 300 400 500 600 700 800 900 1000 1100 1200 1300 1400 1500 1600 1700 1800 1900 2000 2100 2200 2300 2400 2500 2600 2700 2800 2900 3000 3100 3200 3300 3400 3500 3600 3700 3800 3900 4000 4100 4200 4300 4400 4500 4600 4700 4800 4900 5000
do	
	echo "Creating table"
	./tab_create $MNT_POINT/database.db table_1 $SZ_REC
	_init
	
	echo "Insert request"
	./benchdb $MNT_POINT/database.db $REQ_TYPE table_1 $i $SZ_REC $PAT_ACC


	# Save trace (in this case it's saved in NFS file)
	cat $FMLOG > $DEST/$REQ_TYPE/"$REQ_TYPE"_"$i"_"$SZ_REC".dat

	# Save time stamping file and sql script
	mv "$REQ_TYPE"_time.dat $DEST/$REQ_TYPE/"$REQ_TYPE"_time_"$i"_"$SZ_REC".dat

	# Dumping the part
	#cat $DEV_PART > $DEST/$REQ_TYPE/dump_"$i"_"$SZ_REC".dat

	# Remove database file
	rm $MNT_POINT/database.db
	
	# Remoce tracer
	# rmmod flashmon.ko

	# unmout parition 
	# umount $MNT_POINT
	
	sleep 10
done
fi

