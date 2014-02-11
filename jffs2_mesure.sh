#!/bin/sh

#-----------------------------------------------------------------------
#	Description: Test for insert queries
#	Usage: ./launch.sh <req_type> <avg_rec_sz> <ser_number>
#	Author: Hamza Ouarnoughi
#-----------------------------------------------------------------------

REQ_TYPE=$1
SZ_REC=$2
SER=$3
PART_NUM=5
MNT_POINT=/mnt/flash
DEV_PART=/dev/mtd"$PART_NUM"
BDEV_PART=/dev/mtdblock"$PART_NUM"
FMLOG=/proc/flashmon_log
FMCMD=/proc/flashmon
DEST=NFS/jffs2_results
MAX_REC=6000

# Check input parameters 

if [ $# -lt 3 ];
then
	echo "USAGE: $0 <req_type> <avg_rec_sz> <serie_number>"
	exit 1
fi

# Check all directories
chk_dir ()
{
	# Mount dir check
	if [ ! -d "$MNT_POINT" ];
	then
		echo "$MNT_POINT doesn't exist. It will be created"
		mkdir "$MNT_POINT"
	fi
	
	# NFS dir check
	if [ ! -d "$DEST" ];
	then
		echo "$DEST doesn't exist. It will be created"
		mkdir "$DEST"
	fi
	
	# Check dirs by request type
	if [ ! -d "$DEST/$REQ_TYPE/$SER" ];
	then
		echo "$REQ_TYPE directory doesn't exist. It will be created"
		mkdir "$DEST/$REQ_TYPE/$SER"
	fi	
}

# Prepare the test environnement 
_prepare ()
{
	echo "Prepare function ..."
	# Erase flash
	flash_erase $DEV_PART 0 0 > /dev/null
	
	# Mount partition 
	mount -t jffs2 $BDEV_PART $MNT_POINT
	
	# Isert flashmon
	insmod flashmon.ko TRACED_PART=$PART_NUM LOG_MODE=921600
}

# Intialize VFS and flashmon
_init ()
{
	echo "Init function ..."
	sync
	echo 3 > /proc/sys/vm/drop_caches
	echo reset > $FMCMD
}

_insert ()
{
for i in $(seq 50)
do	
	echo "Creating table"
	./tab_create $MNT_POINT/database.db table_1 $SZ_REC
	_init
	
	echo "Insert request"
	./benchdb $REQ_TYPE $MNT_POINT/database.db table_1 $(($i*100)) $SZ_REC

	# Save trace in NFS mounted directory
	cat $FMLOG > $DEST/$REQ_TYPE/"$REQ_TYPE"_"$(($i*100))"_"$SZ_REC".dat

	# Save time stamping file
	mv "$REQ_TYPE"_time.dat $DEST/$REQ_TYPE/"$REQ_TYPE"_time_"$(($i*100))"_"$SZ_REC".dat

	# Dumping part
	#cat $DEV_PART > $DEST/$REQ_TYPE/dump_"$(($i*100))"_"$SZ_REC".dat

	# Remove database file
	rm $MNT_POINT/database.db
done
}

_sel_up ()
{
  echo "Create database and insert records ..."
  ./tab_create $MNT_POINT/database.db table_1 $SZ_REC
  _init
  ./benchdb insert $MNT_POINT/database.db table_1 $MAX_REC $SZ_REC
  sleep 2
  cat $FMLOG > $DEST/$REQ_TYPE/$SER/insert_"$MAX_REC"_"$SZ_REC".dat
	
for i in $(seq 50)
do	
	echo "Begin request ..."
	_init
	./benchdb $REQ_TYPE $MNT_POINT/database.db table_1 $(($i*100)) $SZ_REC > /dev/null
	echo "End request ..."

	# Save trace (in this case it's saved in NFS file)
	cat $FMLOG > $DEST/$REQ_TYPE/$SER/"$REQ_TYPE"_"$(($i*100))"_"$SZ_REC".dat

	# Save time stamping file and sql script
	mv "$REQ_TYPE"_time.dat $DEST/$REQ_TYPE/$SER/"$REQ_TYPE"_time_"$(($i*100))"_"$SZ_REC".dat
done
}

_join ()
{
  echo "Create database and insert records ..."
  ./tab_create $MNT_POINT/database.db table_1 $SZ_REC
  ./tab_create $MNT_POINT/database.db table_2 $SZ_REC
  _init
  ./benchdb insert $MNT_POINT/database.db table_1 $MAX_REC $SZ_REC
  ./benchdb insert $MNT_POINT/database.db table_2 $MAX_REC $SZ_REC
  cat $FMLOG > $DEST/$REQ_TYPE/insert_"$MAX_REC"_"$SZ_REC".dat
	
for i in $(seq 50)
do	
	echo "Begin request ..."
	_init
	./benchdb $REQ_TYPE $MNT_POINT/database.db table_1 $(($i*100)) $SZ_REC table_2 > /dev/null
	echo "End request ..."

	# Save trace (in this case it's saved in NFS file)
	cat $FMLOG > $DEST/$REQ_TYPE/"$REQ_TYPE"_"$(($i*100))"_"$SZ_REC".dat

	# Save time stamping file and sql script
	mv "$REQ_TYPE"_time.dat $DEST/$REQ_TYPE/"$REQ_TYPE"_time_"$(($i*100))"_"$SZ_REC".dat
done
}

# Check directories 
chk_dir

# Prepare partition + insert flashmon
_prepare

# Request execution

case "$REQ_TYPE" in
	
	"insert")	_insert ;;
	
	"select") 	_sel_up ;;
	
	"join")		_join ;;
	
	"update")	_sel_up ;;
	
	*)			echo "ERROR: Uknown request type" 
				exit 1;; 
esac

# Remove I/O tracer
rmmod flashmon.ko

# Remove database
rm $MNT_POINT/database.db

# unmout parition 
umount $MNT_POINT
