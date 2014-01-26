TOPDIR=../
include $(TOPDIR)Rules.mak

ifeq ($(strip $(HAS_FPU)),y)
  ifeq ($(TARGET_ARCH),$(wildcard $(TARGET_ARCH)))
    SUBDIRS+=$(TARGET_ARCH)
  endif
endif

LIBM=libm.a

LIB_STATIC=$(LIBM)
LIB_SHARED=libm.so
LIB_SHARED_MAJORNAME=$(LIB_SHARED).$(MAJOR_VERSION)
LIB_SHARED_FULLNAME=libm-$(VERSION).so

A_TARGET=$(LIBM)

CFLAGS_$(LIBM)+=-D_IEEE_LIBM -D_ISOC99_SOURCE -D_SVID_SOURCE

ifeq ($(strip $(DO_C99_MATH)),y)
  O_OBJS_$(LIBM)=e_acos.o e_acosh.o e_asin.o e_atan2.o e_atanh.o e_cosh.o \
    e_exp.o e_fmod.o e_gamma.o e_gamma_r.o e_hypot.o e_j0.o \
    e_j1.o e_jn.o e_lgamma.o e_lgamma_r.o e_log.o e_log10.o \
    e_pow.o e_remainder.o e_rem_pio2.o e_scalb.o e_sinh.o \
    e_sqrt.o k_cos.o k_rem_pio2.o k_sin.o k_standard.o k_tan.o \
    s_asinh.o s_atan.o s_cbrt.o s_ceil.o s_copysign.o s_cos.o \
    s_erf.o s_expm1.o s_fabs.o s_finite.o s_floor.o s_frexp.o \
    s_ilogb.o s_ldexp.o s_lib_version.o s_log1p.o s_logb.o \
    s_matherr.o s_modf.o s_nextafter.o s_rint.o s_scalbn.o \
    s_signgam.o s_significand.o s_sin.o s_tan.o s_tanh.o \
    w_acos.o w_acosh.o w_asin.o w_atan2.o w_atanh.o w_cabs.o \
    w_cosh.o w_drem.o w_exp.o w_fmod.o w_gamma.o w_gamma_r.o \
    w_hypot.o w_j0.o w_j1.o w_jn.o w_lgamma.o w_lgamma_r.o \
    w_log.o w_log10.o w_pow.o w_remainder.o w_scalb.o w_sinh.o \
    w_sqrt.o w_sqrtf.o fpmacros.o nan.o s_ceilf.o s_floorf.o
else
  # This list of math functions was taken from POSIX/IEEE 1003.1b-1993
  O_OBJS_$(LIBM)=w_acos.o w_asin.o s_atan.o w_atan2.o s_ceil.o s_cos.o \
    w_cosh.o w_exp.o s_fabs.o s_floor.o w_fmod.o s_frexp.o \
    s_ldexp.o w_log.o w_log10.o s_modf.o w_pow.o s_sin.o \
    w_sinh.o w_sqrt.o s_tan.o s_tanh.o \
    s_expm1.o s_scalbn.o s_copysign.o e_acos.o e_asin.o e_atan2.o \
    k_cos.o e_cosh.o e_exp.o e_fmod.o e_log.o e_log10.o e_pow.o \
    k_sin.o e_sinh.o e_sqrt.o k_tan.o e_rem_pio2.o k_rem_pio2.o \
    s_finite.o
endif

include $(ULIBCMK)

$(LIB_SHARED_FULLNAME): $(LIB_STATIC)
	$(LD) $(EXTRA_LDFLAGS) -soname=$(LIB_SHARED_MAJORNAME) \
	  -o $@ --whole-archive $< \
	  --no-whole-archive $(TOPDIR)/libc/misc/internals/interp.o \
	  -L$(TOPDIR)/lib -lc

