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
DEST=NFS/API_test


if [ $# -lt 2 ];
then
	echo "USAGE: $0 <req_type> <nb_rec> <avg_rec_sz> <seq/rand>"
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
./benchdb $MNT_POINT/database.db $REQ_TYPE $NB_REC $SZ_REC $PAT_ACC

# Save trace (in this case it's saved in NFS file)
cat $FMLOG > $DEST/"$REQ_TYPE"_"$NB_REC"_"$SZ_REC".dat

# Save time stamping file and sql script
mv "$REQ_TYPE"_time.dat $DEST/"$REQ_TYPE"_time_"$NB_REC"_"SZ_REC".dat
mv "$REQ_TYPE"_sql.dat $DEST/"$REQ_TYPE"_sql_"$NB_REC"_"SZ_REC".dat

# Dumping the part
cat $DEV_PART > $DEST/dump_"$NB_REC"_"SZ_REC".dat
