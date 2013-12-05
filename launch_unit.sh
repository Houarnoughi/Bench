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
DEST=NFS/API_test/Unit
MAX_REC=6000

if [ $# -lt 2 ];
then
	echo "USAGE: $0 <req_type> <nb_rec> <avg_rec_sz> <seq/rand>"
	exit 0
fi

# Prepare the test environnement 
_prepare ()
{
	# Erase flash
	flash_erase $DEV_PART 0 0 > /dev/null
	
	# Mount partition 
	mount -t jffs2 $BDEV_PART $MNT_POINT
	
	# Isert flashmon
	insmod flashmon.ko TRACED_PART=$PART_NUM LOG_MODE=921600
}

# Intilize VFS and flasmon
_init ()
{
	# Flush the page cache
	echo "Init function"
	sync
	echo 3 > /proc/sys/vm/drop_caches
	echo reset > $FMCMD
}



for i in 5 10 15 20 25 30 35 40 45 50
do	
	_prepare
	
	echo "Creating table ..."
	./tab_create $MNT_POINT/database.db table_1 $SZ_REC
	
	_init
	./benchdb $MNT_POINT/database.db $REQ_TYPE table_1 $i $SZ_REC $PAT_ACC

	sleep 2
	
	# Save trace (in this case it's saved in NFS file)
	cat $FMLOG > $DEST/$REQ_TYPE/"$REQ_TYPE"_"$i"_"$SZ_REC".dat

	# Save time stamping file and sql script
	mv "$REQ_TYPE"_time.dat $DEST/$REQ_TYPE/"$REQ_TYPE"_time_"$i"_"$SZ_REC".dat

	# Dumping the part
	cat $DEV_PART > $DEST/$REQ_TYPE/dump_"$i"_"$SZ_REC".dat

	# Extarct csv file from duped part
	#jffs2dump -c $DEST/$REQ_TYPE/dump_"$i"_"$SZ_REC".dat > $DEST/$REQ_TYPE/dump_"$i"_"$SZ_REC".csv

	
	# Remove database file
	rm $MNT_POINT/database.db

	# Remoce tracer
	rmmod flashmon.ko

	# unmout parition 
	umount $MNT_POINT
done

