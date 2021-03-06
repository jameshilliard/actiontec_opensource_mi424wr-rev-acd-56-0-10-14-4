#! /bin/bash

###############################################
### See if we have a busybox.def.h.ORG file ###
### If not, create it...                    ###
###############################################
if [ ! -e "busybox.def.h.ORG" ]; then
	echo "Creating busybox.def.h.ORG"
	cp busybox.def.h busybox.def.h.ORG
	if [ ! -e "busybox.def.h.ORG" ]; then
		echo "$0: ABORTING: Unable to create busybox.def.h.ORG"
		exit
	fi
fi

###############################################################
### See if we have a bb.def.h file.  If not, extract the    ###
### unchangeable portion of busybox.def.h.ORG into bb.def.h ###
###############################################################
if [ ! -e "bb.def.h" ]; then
	echo "Creating bb.def.h"
	POSITION=`grep -n "Nothing beyond this point should ever be touched" \
		busybox.def.h.ORG | cut -d: -f1`
	TOTALLINES=`cat busybox.def.h.ORG | wc -l`
	NUMLINES=$[${TOTALLINES}-${POSITION}+2]
	tail -n ${NUMLINES} busybox.def.h.ORG > bb.def.h
	if [ ! -e "bb.def.h" ]; then
		echo "$0: ABORTING: Unable to create bb.def.h"
		exit
	fi
fi

#####################################################################
### See if we have a bb.OptionsAndFeatures file.  If not, extract ###
### all the BB_xxx options and features into a unique sorted list ###
### and stuff them into bb.OptionsAndFeatures.                    ###
#####################################################################
if [ ! -e "bb.OptionsAndFeatures" ]; then
	echo "Creating bb.OptionsAndFeatures"
	grep BB_ *.[ch] \
		| tr ' 	,(){}|&' '         ' \
		| grep '^BB_' \
		| sort \
		| uniq \
		| grep -v '^BB_BLAH$' \
		| grep -v '^BB_BUSYBOX$' \
		| grep -v '^BB_DEBUG' \
		| grep -v '^BB_BT$' \
		| grep -v '^BB_VER$' \
		| grep -v '^BB_DEF_MESSAGE$' \
		| grep -v '^BB_DECLARE_EXTERN$' \
		| grep -v '^BB_applet$' \
		> tmpfile.1
	echo BB_NOOP > bb.OptionsAndFeatures
	grep '^BB_FEATURE_' tmpfile.1 >> bb.OptionsAndFeatures
	grep -v '^BB_FEATURE_' tmpfile.1 >> bb.OptionsAndFeatures
	rm -f tmpfile.1


	if [ ! -e "bb.OptionsAndFeatures" ]; then
		echo "$0: ABORTING: Unable to create bb.OptionsAndFeatures"
		exit
	fi
fi

RESULTSFILE="`basename $0`.results"
echo "RESULTSFILE is ${RESULTSFILE}"

BUSYBOXDEFS=busybox.def.h

rm -f ${RESULTSFILE}
touch ${RESULTSFILE}

NOOP_STATIC_SIZE=0
NOOP_STORAGE_SIZE=0

for i in `cat bb.OptionsAndFeatures`
do
	rm -f busybox
	rm -f *.o

	rm -f ${BUSYBOXDEFS}
	touch ${BUSYBOXDEFS}

	echo "===== $i ========================="
	echo "===== $i =========================" >> ${RESULTSFILE}

	echo "#define BB_BUSYBOX"
	echo "#define BB_BUSYBOX" >> ${BUSYBOXDEFS}

	if [ \
		"${i}" = "BB_DF" \
		-o "${i}" = "BB_KILLALL" \
		-o "${i}" = "BB_LSMOD" \
		-o "${i}" = "BB_MOUNT" \
		-o "${i}" = "BB_PS" \
		-o "${i}" = "BB_UMOUNT" \
	]; then
		echo "#define BB_FEATURE_USE_PROCFS"
		echo "#define BB_FEATURE_USE_PROCFS" >> ${BUSYBOXDEFS}
	fi

	echo "#define $i"
	echo "#define $i" >> ${BUSYBOXDEFS}

	cat bb.def.h >> ${BUSYBOXDEFS}

	make

	if [ -e busybox ]; then

		###strip -s busybox ### ALREADY DONE

		STATIC_SIZE=`size busybox | grep busybox | cut -d\	 -f4 | tr -d " "`
		if [ "${i}" = "BB_NOOP" ]; then
			NOOP_STATIC_SIZE=${STATIC_SIZE}
			echo "STATIC_SIZE=${STATIC_SIZE}"
			echo "STATIC_SIZE=${STATIC_SIZE}" >> ${RESULTSFILE}
		else
			SIZEDIFF=$[${STATIC_SIZE}-${NOOP_STATIC_SIZE}]
			echo "STATIC_SIZE=${STATIC_SIZE} (${SIZEDIFF})"
			echo "STATIC_SIZE=${STATIC_SIZE} (${SIZEDIFF})" >> ${RESULTSFILE}
		fi

		STORAGE_SIZE=`ls -la busybox | cut -c29-42 | tr -d " "`
		if [ "${i}" = "BB_NOOP" ]; then
			NOOP_STORAGE_SIZE=${STORAGE_SIZE}
			echo "STORAGE_SIZE=${STORAGE_SIZE}"
			echo "STORAGE_SIZE=${STORAGE_SIZE}" >> ${RESULTSFILE}
		else
			SIZEDIFF=$[${STORAGE_SIZE}-${NOOP_STORAGE_SIZE}]
			echo "STORAGE_SIZE=${STORAGE_SIZE} (${SIZEDIFF})"
			echo "STORAGE_SIZE=${STORAGE_SIZE} (${SIZEDIFF})" >> ${RESULTSFILE}
		fi

		ldd busybox | grep -v libc.so.6 | grep -v ld-linux.so.2
		ldd busybox | grep -v libc.so.6 | grep -v ld-linux.so.2 >> ${RESULTSFILE}
	else
		echo "$i Build Failure"
		echo "$i Build Failure" >> ${RESULTSFILE}
	fi
done

echo "...All done..."
cd ${PWD}
exit

