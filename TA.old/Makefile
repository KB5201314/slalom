export V?=0

# If _HOST or _TA specific compilers are not specified, then use CROSS_COMPILE
HOST_CROSS_COMPILE ?= $(CROSS_COMPILE)
TA_CROSS_COMPILE ?= $(CROSS_COMPILE)

OPTEE_OUTBR_SYSROOT ?= $(shell realpath ../../../out-br/host/aarch64-buildroot-linux-gnu/sysroot/)
TA_DEV_KIT_DIR ?= $(shell realpath ../../../optee_os/out/arm/export-ta_arm64/)

.PHONY: all
all:
	$(MAKE) -C host CROSS_COMPILE="$(HOST_CROSS_COMPILE)" OPTEE_OUTBR_SYSROOT="$(OPTEE_OUTBR_SYSROOT)" --no-builtin-variables
	$(MAKE) -C ta CROSS_COMPILE="$(TA_CROSS_COMPILE)" TA_DEV_KIT_DIR="$(TA_DEV_KIT_DIR)" LDFLAGS=""

.PHONY: clean
clean:
	$(MAKE) -C host clean OPTEE_OUTBR_SYSROOT="$(OPTEE_OUTBR_SYSROOT)"
	$(MAKE) -C ta clean TA_DEV_KIT_DIR="$(TA_DEV_KIT_DIR)"
