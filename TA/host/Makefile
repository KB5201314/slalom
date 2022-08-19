# Copyright (c) Open Enclave SDK contributors.
# Licensed under the MIT License.

include ../config.mk

CFLAGS=$(shell pkg-config oeenclave-$(COMPILER) --cflags | sed s/-m64/-march=armv8-a+simd+crypto/g)
LDFLAGS=$(shell pkg-config oehost-$(COMPILER) --libs)
INCDIR=$(shell pkg-config oehost-$(COMPILER) --variable=includedir)

build:
	@ echo "Compilers used: $(CC), $(CXX)"
	oeedger8r ../sgxdnn_wrap.edl --untrusted \
		--search-path $(INCDIR) \
		--search-path $(INCDIR)/openenclave/edl/sgx
	$(CC) -g -c $(CFLAGS) host.c
	$(CC) -g -c $(CFLAGS) sgxdnn_wrap_u.c
	$(CC) -o sgxdnn_optee_bridge sgxdnn_wrap_u.o host.o $(LDFLAGS)

clean:
	rm -f sgxdnn_optee_bridge host.o sgxdnn_wrap_u.o sgxdnn_wrap_u.c sgxdnn_wrap_u.h sgxdnn_wrap_args.h
