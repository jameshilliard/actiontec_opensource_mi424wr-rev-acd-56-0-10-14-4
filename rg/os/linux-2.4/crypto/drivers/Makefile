O_TARGET := drivers.o

list-multi := ipsec_tunnel.o

ipsec_tunnel-objs := ipsec_sa.o ipsec_dev.o

obj-$(CONFIG_CRYPTOLOOP)	+= cryptoloop.o
obj-$(CONFIG_IPSEC_TUNNEL)	+= ipsec_tunnel.o

include $(TOPDIR)/Rules.make

ipsec_tunnel.o: $(ipsec_tunnel-objs)
	$(LD) -r -o $@ $(ipsec_tunnel-objs)
