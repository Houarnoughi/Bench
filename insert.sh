#!/bin/sh

#------------------------------------------------------
#	Description: Test for insert queries
#	Usage: ./insert.sh <nb_rec> <avg_rec_sz>
#	Author: Hamza Ouarnoughi
#------------------------------------------------------

NB_REC=$1
SZ_REC=$2
PART_NUM=5
MNT_POINT=/mnt/flash
DEV_PART=/dev/mtd"$PART_NUM"
BDEV_PART=/dev/mtdblock"$PART_NUM"
FMLOG=/proc/flashmon_log
FMCMD=/proc/flashmon
DEST=NFS/API_test


if [ $# -lt 2 ];
then
	echo "USAGE: $0 <nb_rec> <avg_rec_sz> <part_num>"
	exit (0)
fi

# Prepare the test environnement 
_prepare ()
{
	# Erase flash
	flash_earse $DEV_PART 0 0 > /dev/null
	
	# Mount partition 
	mount -t jffs2 $BDEV_PART $MNT_POINT
	
	# Isert flashmon
	insmod flashmon.ko TRACED_PART=$PART_NUM LOG_MODE=921600
}

# Intilize VFS and flasmon
_init ()
{
	# Flush the page cache
	sync
	echo 3 > /proc/sys/vm/drop_caches
	echo reste > $FMCMD
}

# Request execution
_init
echo "Execute resuest ..."
./benchdb $MNT_POINT/database.db insert $NB_REC $SZ_REC seq

# Save trace (in this case it's saved in NFS file)
cat $FMLOG > $DEST/insert_"$NB_REC"_"$SZ_REC".dat

# Save time stamping file and sql script
mv insert_time.dat $DEST/insert_time_"$NB_REC"_"SZ_REC".dat
mv insert_sql.dat $DEST/insert_sql_"$NB_REC"_"SZ_REC".dat

# Dumping the part
cat $DEV_PART > $DEST/dump_"$NB_REC"_"SZ_REC".dat


