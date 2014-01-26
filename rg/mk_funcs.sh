#!/bin/bash

panic()
{
	echo "Panic: $1" 1>&2
	exit 1
}

rg_vpath_cp()
{
    src=$1
    dst=$2
    dst_dir=${dst%/*}
    
    if [ ! -d $dst_dir ] ; then
        echo "mkdir -p $dst_dir"
        mkdir -p $dst_dir || return 1
    fi
    if [ "${src:0:1}" == / ] ; then
        src_path=$src
    elif [ -e $PWD_SRC/$src ] ; then
        src_path=$PWD_SRC/$src
    else
        src_path=$PWD_BUILD/$src
    fi

    if [ -e $dst ] ; then
        rm $dst || return 1
    fi

    FAST_COPY=y
    if [ -z "$CONFIG_RG_JPKG_DEBUG" -a \
        "$src_path" != "${src_path/%.[Ssch]/}" ] ; then

        FILE_LICENSE_HEADER=`head -1 $src_path | grep "LICENSE_CODE"`

        if [ -n "$FILE_LICENSE_HEADER" ] ; then
	    FAST_COPY=
            LICENSE_HEADER=
            for f in $FILE_LICENSE_HEADER 
            do
                if [ "$f" == "GPL" -o "$f" == "BSD_GPL" ] ; then
                    LICENSE_HEADER=license_gpl.txt
                elif [ "$f" == "JGPL" -o "$f" == "LGPL" ] ; then
                    LICENSE_HEADER=license_jgpl.txt
                elif [ "$f" == "JUNGO" ] ; then
                    LICENSE_HEADER=license_jungo.txt
                fi
            done

            if [ ! $LICENSE_HEADER ] ; then
                panic "Bad LICENSE_CODE($FILE_LICENSE_HEADER) in $src_path"
            fi

	    rel_src_path=${src_path/$BUILDDIR/rg}
	    rel_src_path=${rel_src_path/$RGSRC/rg}
	    cat $LICENSES_DIR/$LICENSE_HEADER | sed "s|FILENAME|$rel_src_path|g" \
	    >> $dst || panic "Can't create license header for $src_path"

	    sed 1d $src_path >> $dst || panic "Can't copy file $src_path" 
	    echo "'$src_path' -> '$dst'"
        fi
    fi

    if [ $FAST_COPY ] ; then
        cp -vl $src_path $dst || return 1
    fi
}

rg_lnf()
{
    if [ -e $1 ] ; then
        ln -sfn $1 $2
    fi
}

