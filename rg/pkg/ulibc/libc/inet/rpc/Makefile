TOPDIR=../../../
include $(TOPDIR)Rules.mak

A_TARGET=$(LIBC)
CFLAGS+=-I$(TOPDIR)libpthread/linuxthreads/sysdeps/pthread

ifeq ($(strip $(UCLIBC_HAS_FULL_RPC)),y)
O_OBJS_$(LIBC)=auth_none.o auth_unix.o authunix_prot.o bindresvport.o \
  clnt_generic.o clnt_perror.o clnt_raw.o clnt_simple.o clnt_tcp.o \
  clnt_udp.o rpc_dtablesize.o get_myaddress.o getrpcent.o getrpcport.o \
  pmap_clnt.o pmap_getmaps.o pmap_getport.o pmap_prot.o \
  pmap_prot2.o pmap_rmt.o rpc_prot.o rpc_commondata.o rpc_callmsg.o \
  svc.o svc_auth.o svc_auth_unix.o svc_raw.o svc_run.o svc_simple.o \
  svc_tcp.o svc_udp.o xdr.o xdr_array.o xdr_float.o xdr_mem.o \
  xdr_rec.o xdr_reference.o xdr_stdio.o \
  rtime.o clnt_unix.o svc_unix.o create_xid.o xdr_intXX_t.o rcmd.o \
  rexec.o sa_len.o ruserpass.o rpc_thread.o
else
# Stuff needed to do an NFS mount
O_OBJS_$(LIBC)=create_xid.o pmap_clnt.o pmap_getmaps.o pmap_getport.o \
  pmap_prot.o pmap_prot2.o clnt_simple.o clnt_perror.o \
  clnt_tcp.o clnt_udp.o bindresvport.o authunix_prot.o \
  auth_none.o auth_unix.o xdr.o xdr_array.o xdr_rec.o \
  xdr_reference.o xdr_mem.o svc.o svc_auth.o svc_auth_unix.o \
  rpc_callmsg.o rpc_prot.o rpc_dtablesize.o rpc_commondata.o \
  rpc_thread.o rcmd.o rexec.o sa_len.o ruserpass.o rtime.o \
  getrpcent.o
endif

include $(RGMK)

