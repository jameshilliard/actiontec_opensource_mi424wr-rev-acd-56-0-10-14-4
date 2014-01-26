RGSRC=../../../../
include $(RGSRC)/envir.mak

OTHER_TASKS=modules tests

CLEAN+=test_module/test_module.o test_module/.test_module.o.flags
include $(RGMK)

modules: dummy
	$(MAKE) -C $(RGSRC)/linux SUBDIRS=`pwd`/test_module modules

tests:: dummy
	$(MAKE) -f Makefile.test_tool

clean::
	$(MAKE) -f Makefile.test_tool clean

.PHONY: modules tests
