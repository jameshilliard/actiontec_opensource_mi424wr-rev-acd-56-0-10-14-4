# Makefile for MIPS kernel build tools.
#
# Copyright (C) 1996 David S. Miller (dm@engr.sgi.com)
# Copyright (C) 1997 Ralf Baechle (ralf@gnu.ai.mit.edu)
#
OTHER_TARGET	:= $(TOPDIR)/include/asm-$(ARCH)/offset.h

USE_STANDARD_AS_RULE := true

$(OTHER_TARGET): offset.h
	cmp -s $^ $@ || (cp $^ $(OTHER_TARGET).new && mv $(OTHER_TARGET).new $(OTHER_TARGET))

offset.h: offset.s
	sed -n '/^@@@/s///p' $^ >$@

offset.s: offset.c $(TOPDIR)/include/linux/autoconf.h

clean:
	rm -f offset.[hs] $(OTHER_TARGET).new
	
mrproper:	
	rm -f offset.[hs] $(OTHER_TARGET).new
	rm -f $(OTHER_TARGET)

include $(TOPDIR)/Rules.make
