#!/bin/sh

export LC_ALL=POSIX
export LC_CTYPE=POSIX

RAW=` \
    $CC -I$BUILDDIR/os/linux/include -I$BUILDDIR/pkg/build/include \
    -E -dM ${CONFIG_BUSYBOX_CFLAGS} \
    ${CFLAGS_CONFIG_SPEC} ${1:-Config.h} \
    | awk '/^#define *BB_/ && ! /BB_FEATURE/ { sub(/^.*BB_/, ""); print $1 ".c"; }' \
    | tr A-Z a-z | sort
`
test "${RAW}" != "" ||  exit
cd ${2:-.}
# By running $RAW through "ls", we avoid listing
# source files that don't exist.
ls $RAW 2>/dev/null | tr '\n' ' '

