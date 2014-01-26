RGSRC=../../

include $(RGSRC)/envir.mak

ifndef CONFIG_RG_GPL
LOCAL_TARGET+=$(if ($(CONFIG_RG_RGLOADER)$(CONFIG_OPENRG)),lang_compiler)
O_OBJS_lang_compiler+=lang_compiler.o
LDLIBS_lang_compiler:=-llocal_jutil $(LDLIBS_lang_compiler)
JPKG_TARGET_BIN_lang_compiler=y
endif

JPKG_TARGET_BIN_jspawn=y

ifdef CONFIG_RG_IR_TEST
  TARGET=tty_write
  O_OBJS_tty_write=tty_write.o
endif

CD_EXPORTED_FILES+=dep_l.mak test_lang.csv
CD_EXPORTED_FILES+=kos_26_mod.c

EXPORT_AS_SRC+=enums.c mkconfig.c rg_version.c openrg_gpl.c 
INTERNAL_HEADERS+=estream_int.h code2str_ex_test.h getdate.h

# Used by create_config
EXPORT_AS_SRC+=str.c

LOCAL_TARGET+=mkconfig
MGT_LIB=libmgt_client.a
O_OBJS_mkconfig+=mkconfig.o
JPKG_TARGET_BIN_mkconfig=y

EXPORT_LIBS+=$(LIB_RG) $(LIB_JUTIL) $(LOCAL_LIB_RG) $(LOCAL_LIB_JUTIL) \
  $(LIB_GPL) $(MGT_LIB) $(CONFIG_LIB) $(CONFIG_LOCAL_LIB)

EXPORT_HEADERS_DIR=util/
EXPORT_HEADERS+=estream_ssl.h rg_ssl.h checksum.h clock.h base64.h \
  etask.h state_machine.h conv.h rg_system.h rg_time.h tftp_utils.h \
  openrg_gpl.h seq_detector.h file.h date.h str.h strconv.h \
  rg_memory.h rg_random.h dot11.h \
  eresolv.h names.h rg_error.h rg_print.h rg_error_crypto.h \
  memarea.h rg_error_rate.h util.h snprintf.h strace.h alloc.h \
  lines.h attrib.h attrib_io.h console_colors.h csv.h \
  dbi.h dbi_int.h file_util.h fnmatch_ext.h get_next_id.h hash.h \
  html.h jftw.h keyword.h mail.h md5.h navbar.h os_ip.h \
  proc.h rg_error_color.h sed.h sort.h \
  sql.h sql_html_edit.h sql_print.h sql_util.h str_escape.h str_regex.h \
  str_sql.h str_util.h sys.h term.h testutil.h user.h xtime.h \
  csv_util.h cvs.h dbd.h jtest.h unittest.h parse.h uml.h rg_serial.h \
  net_wait.h mgt_client.h linked_list.h rg_auth.h cmd_t.h http_auth.h \
  matrix.h matrix_io.h lang.h rtnetlink.h rtnetlink_route.h \
  rg_chardev.h

LOCAL_TARGET+=jspawn
O_OBJS_jspawn=jspawn.o
LDLIBS_jspawn:=-llocal_jutil $(LDLIBS_jspawn)

EXPORT_HEADERS+=rg_error.c__c/rg_error.c rg_error_rate.c__c/rg_error_rate.c \
  conv.c__c/conv.c ip_proto_name.c__c/ip_proto_name.c \
  inet_conv.c__c/inet_conv.c parse.c__c/parse.c str_util.c__c/str_util.c \
  rg_print.c__c/rg_print.c alloc.c__c/alloc.c lines.c__c/lines.c \
  rg_chardev.c__c/rg_chardev.c

SUBDIRS+=eresolv

ifdef CONFIG_DYN_LINK
  LIB_RG=libopenrg.so
  LIB_JUTIL=libjutil.so
  CONFIG_LIB=librg_config.so
  SO_TARGET=$(if $(CONFIG_RG_RGLOADER)$(CONFIG_OPENRG), \
    $(LIB_RG)) $(LIB_JUTIL) $(CONFIG_LIB)
else
  LIB_RG=libopenrg.a
  LIB_JUTIL=libjutil.a
  CONFIG_LIB=librg_config.a
  A_TARGET=$(if $(CONFIG_RG_RGLOADER)$(CONFIG_OPENRG), \
    $(LIB_RG)) $(LIB_JUTIL) $(CONFIG_LIB)
endif

JPKG_TARGET_liblocal_rg_config.a=y
CONFIG_LOCAL_LIB=liblocal_rg_config.a
LOCAL_A_TARGET+=$(CONFIG_LOCAL_LIB)
O_OBJS_$(CONFIG_LIB)=rg_c_config.o
O_OBJS_$(CONFIG_LOCAL_LIB)+=rg_c_config.o
AUTOGEN_SRC=rg_c_config.c

A_TARGET+=$(if $(CONFIG_RG_RGLOADER)$(CONFIG_OPENRG),$(MGT_LIB))
O_OBJS_$(MGT_LIB)=mgt_client.o

ifneq ($(CONFIG_RG_RGLOADER)$(CONFIG_OPENRG),)
ifneq ($(CONFIG_RG_OS_LINUX)$(CONFIG_RG_JPKG_SRC),)
# Note: Any change here, should be done also in Makefile.gpl
  ifdef CONFIG_DYN_LINK
    LIB_GPL=libopenrg_gpl.so
    SO_TARGET+=$(LIB_GPL)
    ifdef CONFIG_GLIBC
      SO_LDFLAGS_$(LIB_GPL)+=-lulibc_syslog -L$(RGSRC)/pkg/lib
    endif
  else
    LIB_GPL=libopenrg_gpl.a
    A_TARGET+=$(LIB_GPL)
  endif
  JPKG_TARGET_BIN_$(LIB_GPL)=y
  EXPORT_LIBS+=$(LIB_GPL)
  O_OBJS_$(LIB_GPL)=openrg_gpl.o ipc.o bio_socket.o rg_memory.o alloc.o
endif # CONFIG_RG_OS_LINUX CONFIG_RG_JPKG_SRC
endif # CONFIG_OPENRG CONFIG_RG_RGLOADER

LOCAL_LIB_RG=liblocal_openrg.a
LOCAL_LIB_JUTIL=liblocal_jutil.a
LDLIBS_$(LOCAL_LIB_RG):=-llocal_rg_config $(LDLIBS_$(LOCAL_LIB_RG))
JPKG_TARGET_BIN_liblocal_openrg.a=y
JPKG_TARGET_liblocal_jutil.a=y

LOCAL_A_TARGET+=$(if $(CONFIG_WINDOWS_HOST),,\
  $(if $(CONFIG_RG_RGLOADER)$(CONFIG_OPENRG),$(LOCAL_LIB_RG)) $(LOCAL_LIB_JUTIL))

OBJS_COMMON_RG+=esyscall.o rg_iplist.o ipc.o estream_utils.o rg_set_utils.o \
  rg_error_rate.o checksum.o names.o obscure.o rg_random.o \
  syscalls.o bio_socket.o stop.o enums.o etask.o dot11.o tftp_utils.o \
  memarea.o ebuf.o ip_proto_name.o net_wait.o cmd_t.o http_auth.o rg_chardev.o \
  $(if $(CONFIG_RG_SSL),rg_ssl.o estream_ssl.o) \
  $(if $(HAVE_FORK),daemon_init.o) $(if $(CONFIG_HW_CLOCK),clock.o) \
  $(if $(CONFIG_RG_UML)$(CONFIG_RG_JPKG_SRC),uml.o) \
  $(if $(CONFIG_RG_CRYPTO),rg_error_crypto.o) \
  $(if $(CONFIG_RG_SERIAL),rg_serial.o) $(if $(CONFIG_RG_AUTH),rg_auth.o) \
  $(if $(CONFIG_RG_PPPOS_CLI),seq_detector.o)

OBJS_COMMON_JUTIL+=event.o set.o set_utils.o alloc.o snprintf.o net_utils.o \
  str.o lines.o rg_version.o rg_error.o conv.o unittest.o rg_memory.o date.o \
  base64.o rg_time.o attrib.o proc.o str_escape.o str_sql.o html.o inet_conv.o \
  str_regex.o sys.o file_util.o fnmatch_ext.o jftw.o sed.o keyword.o \
  attrib_io.o sort.o str_util.o testutil.o parse.o lang.o strconv.o estream.o \
  rg_print.o jtest.o

LINUX_OBJS+=rtnetlink.o rtnetlink_route.o rg_system.o process_funcs.o

O_OBJS_$(LIB_RG)+=$(OBJS_COMMON_RG) \
  $(if $(CONFIG_RG_TARGET_LINUX),$(LINUX_OBJS)) \
  $(if $(CONFIG_RG_TARGET_VXWORKS)$(CONFIG_RG_JPKG_SRC),\
  os_libc_extra.o vx_net_fixup.o)
  
O_OBJS_$(LIB_JUTIL)+=$(OBJS_COMMON_JUTIL) \
  $(if $(CONFIG_RG_TARGET_LINUX),file.o)

O_OBJS_$(LOCAL_LIB_RG)+=$(OBJS_COMMON_RG) $(LINUX_OBJS)

O_OBJS_$(LOCAL_LIB_JUTIL)+=$(OBJS_COMMON_JUTIL) csv.o csv_util.o dbd_helper.o \
  dbi_main.o file.o dbi_result.o get_next_id.o mail.o md5.o navbar.o \
  rg_error_color.o sql.o sql_file.o sql_html_edit.o sql_print.o sql_util.o \
  term.o user.o matrix.o matrix_io.o getdate.o $(if $(HAVE_CVS),cvs.o) \
  $(if $(HAVE_MYSQL),sql_mysql.o)

ifeq ($(CONFIG_OPENRG),)
  O_OBJS_$(LOCAL_LIB_JUTIL)+=rg_version.o attrib_io.o date.o jtest.o
endif  

CFLAGS+=$(if $(CONFIG_TI_404_COMMON),\
  -I$(RGSRC)/os/vx/ti_404/baselinevxw/pform/inc/)

CFLAGS_REMOVE_local_getdate.o+=-Wstrict-prototypes
CFLAGS_snprintf.o+=$(if $(CONFIG_CC_HAVE_VA_COPY),-DHAVE_VA_COPY)

# XXX temporary patch to avoid illegal assembly generation for the arm compiler
CFLAGS_esyscall.o+=$(if $(CONFIG_ARM),-O1)

ifdef CONFIG_RG_DATE
  TARGET+=rg_date
  O_OBJS_rg_date=rg_date.o
  LDLIBS_rg_date:=$(OPENRG_LDLIBS) $(LDLIBS_rg_date)
  EXPORT_AS_SRC+=rg_date.c
endif

ARCHCONFIG_FIRST_TASKS+=mkconfig

ifndef CONFIG_RG_GPL
local_test.o test.o: test_lang.h test_lang_static.h

test_lang_static.csv: test_lang.csv
	# the following should have been 'V\?T', but this ('?')
	# does not seem to work on SunOS's /bin/sed, so it was replaced
	# by 'V*T', which is close enough for this purpose.
	sed 's/^\(V*T[^,]*\),/\1_static,/g' < $< > $@

test_lang.h: test_lang.csv
	./lang_compiler csv2h test $< $@

test_lang.c: test_lang.csv
	./lang_compiler csv2c test $< $@

test_lang_static.h: test_lang_static.csv
	./lang_compiler --static csv2h test_static $< $@

test_lang_static.c: test_lang_static.csv
	./lang_compiler --static csv2c test_static $< $@
else
LOCAL_TARGET:= $(filter %config,$(LOCAL_TARGET))
TARGET:= $(filter %config %gpl,$(TARGET))
SO_TARGET:= $(filter %config.so %gpl.so,$(SO_TARGET))
A_TARGET:= $(filter %config.a %gpl.a $(MGT_LIB) ,$(A_TARGET))
LOCAL_A_TARGET:= $(filter %config.a %gpl.a,$(LOCAL_A_TARGET))
EXPORT_LIBS:= $(filter %config.a %gpl.a %config.so %gpl.so $(MGT_LIB) ,$(EXPORT_LIBS))
SUBDIRS=
endif
include $(RGMK)
