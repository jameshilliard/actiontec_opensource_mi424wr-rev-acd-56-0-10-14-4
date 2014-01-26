TOPDIR=../../
include $(TOPDIR)Rules.mak

ifeq ($(strip $(UCLIBC_HAS_RPC)),y)
  SUBDIRS=rpc
endif

A_TARGET=$(LIBC)

MSRC=addr.c
MOBJ=inet_aton.o inet_addr.o inet_ntoa.o inet_makeaddr.o inet_lnaof.o \
  inet_netof.o

MSRC2=resolv.c
MOBJ2=encodeh.o decodeh.o encoded.o decoded.o lengthd.o encodeq.o \
  decodeq.o lengthq.o encodea.o decodea.o encodep.o decodep.o \
  formquery.o dnslookup.o resolveaddress.o opennameservers.o \
  closenameservers.o resolvename.o gethostbyname.o res_init.o \
  res_query.o gethostbyaddr.o read_etc_hosts_r.o get_hosts_byname_r.o \
  get_hosts_byaddr_r.o gethostbyname2.o getnameinfo.o gethostent.o \
  gethostbyname_r.o gethostbyname2_r.o gethostbyaddr_r.o

MSRC3=socketcalls.c
MOBJ3= accept.o bind.o connect.o getpeername.o getsockname.o getsockopt.o \
  listen.o recv.o recvfrom.o recvmsg.o send.o sendmsg.o sendto.o \
  setsockopt.o shutdown.o socket.o socketpair.o 

O_OBJS_$(LIBC)=getservice.o getproto.o hostid.o getnetent.o getnetbynm.o \
  getnetbyad.o inet_net.o ntop.o herror.o if_nametoindex.o gai_strerror.o \
  getaddrinfo.o in6_addr.o ether_addr.o

CFLAGS+=-DL_$*

OTHER_OBJS=$(MOBJ) $(MOBJ2) $(MOBJ3)
O_OBJS_$(LIBC)+=$(OTHER_OBJS)

ifeq ($(LIBC_ARCH),mips)
  # Unresolved warnings:
  # "use of memory input without lvalue in asm operand 7 is deprecated"
  CFLAGS_REMOVE_getsockopt.o+=-Werror
  CFLAGS_REMOVE_setsockopt.o+=-Werror
  CFLAGS_REMOVE_recvfrom.o+=-Werror
  CFLAGS_REMOVE_sendto.o+=-Werror
endif

include $(RGMK)

$(MOBJ): $(MSRC)
$(MOBJ2): $(MSRC2)
$(MOBJ3): $(MSRC3)

