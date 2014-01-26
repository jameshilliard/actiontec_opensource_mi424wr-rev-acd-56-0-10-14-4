#!/bin/sh

exec_alert()
{
    if ! $@
    then
        echo "failed executing: $@"
	exit_build -1
    fi
}

exec_alert_root()
{
    exec_alert $BUILDDIR/pkg/build/exec_as_root $@ 
}

exit_build()
{
    $BUILDDIR/pkg/build/exec_as_root umount $RAMDISK_MOUNT_POINT 2>/dev/null
    exit $1
}

if [ "$TARGET_ENDIANESS" = "BIG" ] ; then
    TARGET_ENDIANESS_L=b
else
    TARGET_ENDIANESS_L=l
fi

# Create everything which is not created somewhere else
cd $RAMDISK_RW_DIR

if [ "y" = "$CONFIG_RG_RGLOADER" ] ; then
    REDUCED_DEVICE_SET="y"
fi    

if [ -z $REDUCED_DEVICE_SET ] || [ -n $CONFIG_RG_UML ] ; then
$RG_MKDIR proc
$RG_MKDIR etc
fi
if [ -z $REDUCED_DEVICE_SET ] ; then
$RG_MKDIR tmp
$RG_CHMOD 777 tmp
$RG_MKDIR var/state/dhcp
$RG_MKDIR var/log
$RG_MKDIR var/lock/subsys
$RG_MKDIR var/run
fi

$RG_MKDIR mnt/flash
$RG_MKDIR mnt/cramfs
$RG_MKDIR mnt/modfs

if [ -n "$CONFIG_JFFS2_FS_FEATURE" ] ; then
    if [ $CONFIG_JFFS2_FS_FEATURE -ne 0 ] ; then
        $RG_MKDIR mnt/jffs2
    fi
fi

$RG_MKDIR $RAMDISK_DEV_DIR
cd $RAMDISK_DEV_DIR

if [ "y" = "$CONFIG_HW_ST_20190" ] ; then
$RG_MKNOD ctrle c 120 0
fi

if [ -z $REDUCED_DEVICE_SET ] ; then
$RG_MKNOD ttyUSB0 c 188 0
$RG_MKNOD ttyUSB1 c 188 1
fi
if [ -n "$CONFIG_USB_PRINTER" ] ; then
    for i in 0 1 2 3 4 5 6 7 8 9
    do
	$RG_MKNOD usblp$i c 180 $i
    done
fi

$RG_MKNOD video0 c 81 0

$RG_MKNOD random c 1 8
$RG_MKNOD urandom c 1 9
$RG_MKNOD loop0 b 7 0
$RG_MKNOD loop1 b 7 1
if [ -z $REDUCED_DEVICE_SET ] ; then
$RG_MKNOD cua0 c 5 64
$RG_MKNOD fb0 c 29 0
$RG_MKNOD fb1 c 29 32
$RG_MKNOD fb2 c 29 64
$RG_MKNOD fb3 c 29 96
$RG_MKNOD fd0 b 2 0
$RG_MKNOD fb0H1440 b 2 28
$RG_MKNOD full c 1 7
$RG_MKNOD hda b 3 0
$RG_MKNOD hda1 b 3 1
$RG_MKNOD hda2 b 3 2
$RG_MKNOD hda3 b 3 3
$RG_MKNOD hda4 b 3 4
$RG_MKNOD hda5 b 3 5
$RG_MKNOD hda6 b 3 6
$RG_MKNOD hda7 b 3 7
$RG_MKNOD hda8 b 3 8
$RG_MKNOD hdb b 3 64
$RG_MKNOD hdb1 b 3 65
$RG_MKNOD hdb2 b 3 66
$RG_MKNOD hdb3 b 3 67
$RG_MKNOD hdb4 b 3 68
$RG_MKNOD hdb5 b 3 69
$RG_MKNOD hdb6 b 3 70
$RG_MKNOD hdb7 b 3 71
$RG_MKNOD hdb8 b 3 72
$RG_MKNOD hdc b 22 0
$RG_MKNOD hdc1 b 22 1
$RG_MKNOD hdc2 b 22 2
$RG_MKNOD hdc3 b 22 3
$RG_MKNOD hdc4 b 22 4
$RG_MKNOD hdc5 b 22 5
$RG_MKNOD hdc6 b 22 6
$RG_MKNOD hdc7 b 22 7
$RG_MKNOD hdc8 b 22 8
$RG_MKNOD hdd b 22 64
$RG_MKNOD hdd1 b 22 65
$RG_MKNOD hdd2 b 22 66
$RG_MKNOD hdd3 b 22 67
$RG_MKNOD hdd4 b 22 68
$RG_MKNOD hdd5 b 22 69
$RG_MKNOD hdd6 b 22 70
$RG_MKNOD hdd7 b 22 71
$RG_MKNOD hdd8 b 22 72
fi
$RG_MKNOD kmem c 1 2
$RG_MKNOD mem c 1 1
$RG_MKNOD null c 1 3
$RG_MKNOD port c 1 4 
$RG_MKNOD ptyp0 c 2 0
$RG_MKNOD ptyp1 c 2 1
$RG_MKNOD ptyp2 c 2 2
$RG_MKNOD ptyp3 c 2 3
$RG_MKNOD ptyp4 c 2 4
$RG_MKNOD ptyp5 c 2 5
$RG_MKNOD ptyp6 c 2 6
$RG_MKNOD ptyp7 c 2 7
if [ -z $REDUCED_DEVICE_SET ] ; then
$RG_MKNOD ptyp8 c 2 8
$RG_MKNOD ptyp9 c 2 9
$RG_MKNOD ptypa c 2 10
$RG_MKNOD ptypb c 2 11
$RG_MKNOD ptypc c 2 12
$RG_MKNOD ptypd c 2 13
$RG_MKNOD ptype c 2 14
$RG_MKNOD ptypf c 2 15
$RG_MKNOD ptyq0 c 2 16
$RG_MKNOD ptyq1 c 2 17
$RG_MKNOD ptyq2 c 2 18
$RG_MKNOD ptyq3 c 2 19
$RG_MKNOD ptyq4 c 2 20
$RG_MKNOD ptyq5 c 2 21
$RG_MKNOD ptyq6 c 2 22
$RG_MKNOD ptyq7 c 2 23
$RG_MKNOD ptyq8 c 2 24
$RG_MKNOD ptyq9 c 2 25
$RG_MKNOD ptyqa c 2 26
$RG_MKNOD ptyqb c 2 27
$RG_MKNOD ptyqc c 2 28
$RG_MKNOD ptyqd c 2 29
$RG_MKNOD ptyqe c 2 30
$RG_MKNOD ptyqf c 2 31
$RG_MKNOD ptyr0 c 2 32
$RG_MKNOD ptyr1 c 2 33
$RG_MKNOD ptyr2 c 2 34
$RG_MKNOD ptyr3 c 2 35
$RG_MKNOD ptyr4 c 2 36
$RG_MKNOD ptyr5 c 2 37
$RG_MKNOD ptyr6 c 2 38
$RG_MKNOD ptyr7 c 2 39
$RG_MKNOD ptyr8 c 2 40
$RG_MKNOD ptyr9 c 2 41
$RG_MKNOD ptyra c 2 42
$RG_MKNOD ptyrb c 2 43
$RG_MKNOD ptyrc c 2 44
$RG_MKNOD ptyrd c 2 45
$RG_MKNOD ptyre c 2 46
$RG_MKNOD ptyrf c 2 47
$RG_MKNOD ptys0 c 2 48
$RG_MKNOD ptys1 c 2 49
$RG_MKNOD ptys2 c 2 50
$RG_MKNOD ptys3 c 2 51
$RG_MKNOD ptys4 c 2 52
$RG_MKNOD ptys5 c 2 53
$RG_MKNOD ptys6 c 2 54
$RG_MKNOD ptys7 c 2 55
$RG_MKNOD ptys8 c 2 56
$RG_MKNOD ptys9 c 2 57
$RG_MKNOD ptysa c 2 58
$RG_MKNOD ptysb c 2 59
$RG_MKNOD ptysc c 2 60
$RG_MKNOD ptysd c 2 61
$RG_MKNOD ptyse c 2 62
$RG_MKNOD ptysf c 2 63
$RG_MKNOD rtc c 10 135
$RG_MKNOD tpanel c 10 11
fi
$RG_MKNOD ram0 b 1 0
$RG_MKNOD ram1 b 1 1
$RG_MKNOD ram2 b 1 2
$RG_MKNOD ram3 b 1 3
$RG_MKNOD nvram c 10 144
$RG_MKNOD tty c 5 0 
$RG_MKNOD tty0 c 4 0
$RG_MKNOD tty1 c 4 1
$RG_MKNOD tty2 c 4 2
if [ -z $REDUCED_DEVICE_SET ] ; then
$RG_MKNOD tty3 c 4 3
$RG_MKNOD tty4 c 4 4
$RG_MKNOD tty5 c 4 5
$RG_MKNOD tty6 c 4 6
$RG_MKNOD tty7 c 4 7
$RG_MKNOD tty8 c 4 8
fi
$RG_MKNOD ttyS0 c 4 64
$RG_MKNOD ttyS1 c 4 65
$RG_MKNOD ttyp0 c 3 0
$RG_MKNOD ttyp1 c 3 1
$RG_MKNOD ttyp2 c 3 2
if [ -z $REDUCED_DEVICE_SET ] ; then
$RG_MKNOD ttyp3 c 3 3
$RG_MKNOD ttyp4 c 3 4
$RG_MKNOD ttyp5 c 3 5
$RG_MKNOD ttyp6 c 3 6
$RG_MKNOD ttyp7 c 3 7
$RG_MKNOD ttyp8 c 3 8
$RG_MKNOD ttyp9 c 3 9
$RG_MKNOD ttypa c 3 10
$RG_MKNOD ttypb c 3 11
$RG_MKNOD ttypc c 3 12
$RG_MKNOD ttypd c 3 13
$RG_MKNOD ttype c 3 14
$RG_MKNOD ttypf c 3 15
$RG_MKNOD ttyq0 c 3 16
$RG_MKNOD ttyq1 c 3 17
$RG_MKNOD ttyq2 c 3 18
$RG_MKNOD ttyq3 c 3 19
$RG_MKNOD ttyq4 c 3 20
$RG_MKNOD ttyq5 c 3 21
$RG_MKNOD ttyq6 c 3 22
$RG_MKNOD ttyq7 c 3 23
$RG_MKNOD ttyq8 c 3 24
$RG_MKNOD ttyq9 c 3 25
$RG_MKNOD ttyqa c 3 26
$RG_MKNOD ttyqb c 3 27
$RG_MKNOD ttyqc c 3 28
$RG_MKNOD ttyqd c 3 29
$RG_MKNOD ttyqe c 3 30
$RG_MKNOD ttyqf c 3 31
$RG_MKNOD ttyr0 c 3 32
$RG_MKNOD ttyr1 c 3 33
$RG_MKNOD ttyr2 c 3 34
$RG_MKNOD ttyr3 c 3 35
$RG_MKNOD ttyr4 c 3 36
$RG_MKNOD ttyr5 c 3 37
$RG_MKNOD ttyr6 c 3 38
$RG_MKNOD ttyr7 c 3 39
$RG_MKNOD ttyr8 c 3 40
$RG_MKNOD ttyr9 c 3 41
$RG_MKNOD ttyra c 3 42
$RG_MKNOD ttyrb c 3 43
$RG_MKNOD ttyrc c 3 44
$RG_MKNOD ttyrd c 3 45
$RG_MKNOD ttyre c 3 46
$RG_MKNOD ttyrf c 3 47
$RG_MKNOD ttys0 c 3 48
$RG_MKNOD ttys1 c 3 49
$RG_MKNOD ttys2 c 3 50
$RG_MKNOD ttys3 c 3 51
$RG_MKNOD ttys4 c 3 52
$RG_MKNOD ttys5 c 3 53
$RG_MKNOD ttys6 c 3 54
$RG_MKNOD ttys7 c 3 55
$RG_MKNOD ttys8 c 3 56
$RG_MKNOD ttys9 c 3 57
$RG_MKNOD ttysa c 3 58
$RG_MKNOD ttysb c 3 59
$RG_MKNOD ttysc c 3 60
$RG_MKNOD ttysd c 3 61
$RG_MKNOD ttyse c 3 62
$RG_MKNOD ttysf c 3 63
$RG_MKNOD vscisdn c 61 0
$RG_MKNOD vsctdm c 60 0
fi
$RG_MKNOD zero c 1 5
$RG_MKNOD chardev c 254 0

#MTD (Standard flash support)
if [ "y" = "$CONFIG_MTD" ] ; then
$RG_MKNOD mtd0 c 90 0
$RG_MKNOD mtdr0 c 90 1
$RG_MKNOD mtd1 c 90 2
$RG_MKNOD mtdr1 c 90 3
$RG_MKNOD mtd2 c 90 4
$RG_MKNOD mtdr2 c 90 5
$RG_MKNOD mtd3 c 90 6
$RG_MKNOD mtdr3 c 90 7
$RG_MKNOD mtdblock0 b 31 0
$RG_MKNOD mtdblock1 b 31 1
$RG_MKNOD mtdblock2 b 31 2
$RG_MKNOD mtdblock3 b 31 3
$RG_LN mtdblock1 $RAMDISK_DEV_DIR/mtd_rgconf0 
$RG_LN mtdblock2 $RAMDISK_DEV_DIR/mtd_rgconf1 
fi

if [ "$CONFIG_RG_HW" = "CENTAUR" ] ; then
  $RG_MKNOD ttyAM0 c 204 16
  if [ -n "$CONFIG_KSFLASH" ] ; then
    $RG_MKNOD perm_storage c 245 0
  fi
fi

#SCSI disks (required for firewire-ieee1394 and USB)
if [ "y" = "$CONFIG_BLK_DEV_SD" -o "m" = "$CONFIG_BLK_DEV_SD" ] ; then
    m=8
    n=0
    for i in a b c d e f g h i j k l m n o p q r s t u v w x y z \
	aa ab ac ad ae af
    do
	for j in '' 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
	do
	    $RG_MKNOD sd$i$j b $m $n
	    n=$((n+1))
	    if [ $n -gt 255 ]
	    then
		n=0
		m=65
	    fi
	done
    done
fi

# Create 2 block device inodes for 2 md devices
if [ -n "$CONFIG_BLK_DEV_MD" ] ; then
  $RG_MKNOD md0 b 9 0
  $RG_MKNOD md1 b 9 1
fi

if [ -n "$CONFIG_RG_UML" ] ; then
  $RG_MKNOD ubd0 b 98 0
  $RG_MKNOD ubd1 b 98 1
fi

if [ "" != "$CONFIG_INCAIP" ] ; then
  $RG_MKNOD inca-port c 248 0
fi

if [ "" != "$CONFIG_INCAIPSSC" ] ; then
  $RG_MKNOD ssc1 c 249 0
  $RG_MKNOD ssc2 c 249 1
fi

if [ "" != "$CONFIG_INCAIPPWM" ] ; then
  $RG_MKNOD pwm_contrast c 244 0
  $RG_MKNOD pwm_bright c 244 1
fi

if [ "" != "$CONFIG_INCAIP_KEYPAD" ] ; then
  $RG_MKNOD keypad c 250 0
fi

if [ "" != "$CONFIG_INCAIP_DSP" ] ; then
  $RG_MKNOD oakcmd c 247 1
  $RG_MKNOD oakvoice0 c 247 2
  $RG_MKNOD oakvoice1 c 247 3
fi

if [ "" != "$CONFIG_VINETIC" ] ; then
  $RG_MKNOD vin10 c 246 10
  $RG_MKNOD vin11 c 246 11
  $RG_MKNOD vin12 c 246 12
  $RG_MKNOD vin13 c 246 13
  $RG_MKNOD vin14 c 246 14
fi

if [ "" != "$CONFIG_ZSP400" ] ; then
  $RG_MKNOD vp0 c 120 0
  $RG_MKNOD vp1 c 120 1
  $RG_MKNOD vp2 c 120 2
  $RG_MKNOD vp3 c 120 3
fi

if [ "" != "$CONFIG_INCAIP_LEDMATRIX" ] ; then
  $RG_MKNOD ledmatrix c 245 0
fi

if ! [ -e $RAMDISK_DEV_DIR/console ] ; then
  if [ $CONFIG_RG_CONSOLE_DEVICE = "console" ] ; then
    $RG_MKNOD console c 5 1
  else
    $RG_LN /dev/$CONFIG_RG_CONSOLE_DEVICE $RAMDISK_DEV_DIR/console
  fi
fi

cd $RAMDISK_RW_DIR

find dev/ -not -type l -and -not -type d | xargs $RG_CHMOD 666
$RG_CHMOD 620 dev/ttyp*

# Let the usual mechanism create /home as file links to cramfs, but here
# override with a link to the real directory to save inodes on the writable
# root file system.
exec_alert_root rm -rf home
$RG_LN /mnt/cramfs/home home

cd $DISK_IMAGE_DIR

if [ "y" = "$CONFIG_GLIBC" ] ; then
    GLIBC_BUILD=$BUILDDIR/pkg/build/lib/
    LIBS="libc.so.6 ld-linux.so.2 libdl.so.2 libutil.so.1 libresolv.so.2 \
        libcrypt.so.1 libnss_files.so.2 libnss_dns.so.2 libnsl.so.1 libm.so.6"
    
    if [ "y" = "$CONFIG_RG_THREADS" ] ; then
      LIBS="libpthread.so.0 "$LIBS
    fi    

    for lib in $LIBS
    do
	lib_base=`basename $lib`
	cp -f $GLIBC_BUILD/$lib $CRAMFS_DIR/lib/
	$RG_LN /mnt/cramfs/lib/$lib_base $RAMDISK_RW_DIR/lib/
    done

    mkdir -p $RAMDISK_RW_DIR/$TOOLS_PREFIX
    $RG_LN / $RAMDISK_RW_DIR/$TOOLS_PREFIX/$TARGET_MACHINE
fi

if [ "y" = "$CONFIG_INSURE" ] ; then
    for lib in libinsure.so libtql_c_gcc.so libdlsym.so ; do
	cp -f /usr/local/parasoft/lib.linux2/$lib $CRAMFS_DIR/lib/
	$RG_LN /mnt/cramfs/lib/`basename $lib` $RAMDISK_RW_DIR/lib/
    done

    mkdir -p $RAMDISK_RW_DIR/usr/local/parasoft/
    $RG_LN  /lib $RAMDISK_RW_DIR/usr/local/parasoft/lib.linux2
    cp -f $BUILDDIR/pkg/build/pkg/build/psrc $RAMDISK_RW_DIR/usr/local/parasoft/.psrc
fi

export RAMDISK_IMG_GZ="$DISK_IMAGE_DIR/ramdisk.img.gz"

if [ "y" = "$CONFIG_CRAMFS_DYN_BLOCKSIZE" ] ; then
    CRAMFS_BLKSZ=$CONFIG_CRAMFS_BLKSZ
else
    CRAMFS_BLKSZ=4096
fi

if [ "y" != "$CONFIG_SIMPLE_CRAMDISK" ] ; then
    # calculate the minimal ramdisk size unless overridden from create_config 
    if [ "y" = "$CONFIG_SIMPLE_RAMDISK" ] ; then
	# cramfs is copied into the ramdisk image, so calculate it as well
	CALC_DIRS="$CRAMFS_DIR $RAMDISK_RW_DIR"
    else
	# count only the ramdisk directory
	CALC_DIRS="$RAMDISK_RW_DIR"
    fi
    CALC_RAMDISK_SIZE=`du -skc $CALC_DIRS | grep total | cut -f 1`
    # add 50K for various ramdisk using applications (pptp etc.) and if
    # necessary, resize to the minimum size of 256K
    CALC_RAMDISK_SIZE=`expr $CALC_RAMDISK_SIZE + 50`
    # add 250K for samba user state temporary files
    if [ "" != "$CONFIG_RG_FILESERVER" ] ; then
        CALC_RAMDISK_SIZE=`expr $CALC_RAMDISK_SIZE + 250`
    fi
    if [ $CALC_RAMDISK_SIZE -lt 256 ] ; then
        CALC_RAMDISK_SIZE=256
    fi
    echo "Ramdisk calculated size is $CALC_RAMDISK_SIZE Kb"
    
    if [ "$CONFIG_RAMDISK_SIZE" == "" -o "$CONFIG_RAMDISK_SIZE" == "0" ] ; then
	CONFIG_RAMDISK_SIZE=$CALC_RAMDISK_SIZE	
    fi

    if [ $CONFIG_RAMDISK_SIZE -lt 4096 ] ; then
        INODES='-N 1200'
    fi

    # make an empty file
    if [ "y" = "$CONFIG_RG_INITFS_RAMDISK" ] ; then
	dd if=/dev/zero of=$DISK_IMAGE_DIR/rd.img bs=1k \
	    count=$CONFIG_RAMDISK_SIZE
	if [ "y" = "$CONFIG_EXT2_FS" ] ; then
	    # make a file system on it
	    /sbin/mkfs.ext2 $INODES -F $DISK_IMAGE_DIR/rd.img
	fi
    fi

    # Try to install loop module if /dev/loop does not exists
    if [[ ! -e /dev/loop && ! -e /dev/loop0 ]] ; then
	echo "/dev/loop does not exists!"
	echo "Trying to install loop module (insmod loop)"

	exec_alert_root /sbin/insmod loop

	echo "loop module installed successfully"
    fi
    
    if [[ "y" = "$CONFIG_RG_INITFS_RAMDISK" && "y" = "$CONFIG_EXT2_FS" ]] ; then
        exec_alert_root mount -o user -o exec -o suid -o dev -o loop rd.img \
        $RAMDISK_MOUNT_POINT
    fi

    exec_alert_root cp -af $RAMDISK_RW_DIR/* $RAMDISK_MOUNT_POINT

    if [ "y" = "$CONFIG_SIMPLE_RAMDISK" ] ; then
	# Copy cramfs onto the ramdisk image...
	exec_alert_root cp -af $CRAMFS_DIR/* $RAMDISK_MOUNT_POINT/mnt/cramfs
    fi

    if [ "y" = "$CONFIG_RG_KERNEL_NEEDED_SYMBOLS" ] ; then
	# Get undefined symbols from all modules
	find $CRAMFS_DIR/lib/modules $MODFS_DIR/lib/modules -type f | \
	    xargs --no-run-if-empty -n 1 $NM --undefined-only | \
	    sed -e 's/^[ \t]*U //' | sort -u > $NEEDED_SYMBOLS
    fi

    if [ "y" = "$CONFIG_RG_MODFS" ] ; then
        $GENROMFS -f $DISK_IMAGE_DIR/mod.img -d $MODFS_DIR -v
        cp -f $DISK_IMAGE_DIR/mod.img $MODFS_DISK
    fi

    exec_alert_root chown -h -R 0:0 $RAMDISK_MOUNT_POINT

    if [ "y" = "$CONFIG_RG_INITFS_CRAMFS" ] ; then
    	$BUILDDIR/os/linux/scripts/cramfs/mkcramfs -b $CRAMFS_BLKSZ \
	    -c $CONFIG_RG_CRAMFS_COMP_METHOD $RAMDISK_MOUNT_POINT \
	    $DISK_IMAGE_DIR/cramfs_init.img
	exec_alert_root rm -rf $RAMDISK_MOUNT_POINT
    elif [ "y" = "$CONFIG_RG_INITFS_RAMDISK" ] ; then
	if [ "y" = "$CONFIG_EXT2_FS" ] ; then
	    exec_alert_root umount $RAMDISK_MOUNT_POINT
	elif [ "y" = "$CONFIG_ROMFS_FS" ] ; then
	    $GENROMFS -f $DISK_IMAGE_DIR/rd.img -d $RAMDISK_MOUNT_POINT -v
	    exec_alert_root rm -rf $RAMDISK_MOUNT_POINT
	fi
        if [ "y" = "$CONFIG_RG_NONCOMPRESSED_USERMODE_IMAGE" ] ; then
	    cp $DISK_IMAGE_DIR/rd.img $RAMDISK_IMG_GZ
        else
	    gzip -v9 -c $DISK_IMAGE_DIR/rd.img > $RAMDISK_IMG_GZ
        fi
    fi
else
    # Copy cramfs onto the ramdisk image...
    exec_alert_root cp -af $CRAMFS_DIR/* $RAMDISK_RW_DIR/mnt/cramfs
fi

if [ "y" = "$CONFIG_CRAMFS_FS" ] ; then
    # COMPRESSED_DISK and COMPRESSED_RAMDISK are defined in envir.mak and their 
    # location is inside the kernel (ARCH specific)
    if [ "LINUX_22" = "$CONFIG_RG_OS" ] ; then
	$BUILDDIR/os/linux/scripts/cramfs/mkcramfs -e $TARGET_ENDIANESS_L \
	    $CRAMFS_DIR $DISK_IMAGE_DIR/cramfs.img
	cat cramfs.img > $COMPRESSED_DISK
	cat $RAMDISK_IMG_GZ >> $COMPRESSED_DISK
	$BUILDDIR/pkg/build/checksum cramfs.img $TARGET_ENDIANESS_L >> $COMPRESSED_DISK
    elif [ "LINUX_24" = "$CONFIG_RG_OS" ] ; then
	$BUILDDIR/os/linux/scripts/cramfs/mkcramfs -b $CRAMFS_BLKSZ \
	    -c $CONFIG_RG_CRAMFS_COMP_METHOD $CRAMFS_DIR \
	    $DISK_IMAGE_DIR/cramfs.img
	cp -f $DISK_IMAGE_DIR/cramfs.img $COMPRESSED_DISK
	if [ "y" = "$CONFIG_RG_INITFS_CRAMFS" ] ; then
	    cp -f $DISK_IMAGE_DIR/cramfs_init.img $COMPRESSED_INIT_DISK
	else
	    cp -f $RAMDISK_IMG_GZ $COMPRESSED_RAMDISK
	fi
    fi
else
    if [ "y" = "$CONFIG_SIMPLE_CRAMDISK" ] ; then
	$BUILDDIR/os/linux/scripts/cramfs/mkcramfs $RAMDISK_RW_DIR \
	    $RAMDISK_IMG_GZ
    fi
    cp -f $RAMDISK_IMG_GZ $COMPRESSED_DISK
fi

if [ "$CONFIG_BINFMT_FLAT" = "y" ] ; then
    EXECUTABLES_TYPE="flat"
fi
export CONFIG_RAMDISK_SIZE

if [ -n "$CONFIG_RG_UML" ] ; then
  # XXX DEVELOPMENT, for now we use a ubd device and not ramdisk for UML
  gunzip -c $COMPRESSED_DISK > $BUILDDIR/os/linux/fs.img
fi
