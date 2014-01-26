TOPDIR=../../../
include $(TOPDIR)Rules.mak

A_TARGET=$(LIBC)

O_OBJS_$(LIBC)=__uClibc_main.o tempname.o errno.o __errno_location.o \
  __h_errno_location.o

OTHER_TASKS+=interp.o

CLEAN+=interp.*

include $(RGMK)

interp.c: Makefile
	echo "/* Force shared libraries to know about the correct library loader */" > interp.c
	echo "#include <features.h>" >> interp.c
	echo "#ifdef __HAVE_ELF__" >> interp.c
	echo "const char __dl_ldso__[] __attribute__ ((section " \
		"(\".interp\"))) =\""$(DYNAMIC_LINKER)"\";" >> interp.c
	echo "#endif" >> interp.c

