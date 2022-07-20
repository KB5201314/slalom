CC      ?= $(CROSS_COMPILE)gcc
LD      ?= $(CROSS_COMPILE)ld
AR      ?= $(CROSS_COMPILE)ar
NM      ?= $(CROSS_COMPILE)nm
OBJCOPY ?= $(CROSS_COMPILE)objcopy
OBJDUMP ?= $(CROSS_COMPILE)objdump
READELF ?= $(CROSS_COMPILE)readelf

OBJS = optee_bridge.o

CFLAGS += -shared -Wall --sysroot=$(OPTEE_OUTBR_SYSROOT) -I../ta/include -I./include -L$(OPTEE_OUTBR_SYSROOT)/include -L$(OPTEE_OUTBR_SYSROOT)/usr/include

CFLAGS += -I../third_party/binn/src

#Add/link other required libraries here
LDADD += -lteec --sysroot=$(OPTEE_OUTBR_SYSROOT) -L$(OPTEE_OUTBR_SYSROOT)/lib -L$(OPTEE_OUTBR_SYSROOT)/usr/lib

BINARY = optee_bridge.so

.PHONY: all
all: $(BINARY)

.PHONY: ../third_party/binn/libbinn.a
../third_party/binn/libbinn.a:
	make -C ../third_party/binn static CROSS_PREFIX=$(CROSS_COMPILE)

$(BINARY): $(OBJS) ../third_party/binn/libbinn.a
	$(CC) $(LDFLAGS) -o $@ $^ $(LDADD)

.PHONY: clean
clean:
	rm -f $(OBJS) $(BINARY)
	make -C ../third_party/binn clean CROSS_PREFIX=$(CROSS_COMPILE)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
