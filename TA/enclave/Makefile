# Copyright (c) Open Enclave SDK contributors.
# Licensed under the MIT License.

include ../config.mk

CRYPTO_LDFLAGS := $(shell pkg-config oeenclave-$(COMPILER) --variable=${OE_CRYPTO_LIB}libs)

CFLAGS=$(shell pkg-config oeenclave-$(COMPILER) --cflags | sed s/-m64/-march=armv8-a+simd+crypto/g)
LDFLAGS=$(shell pkg-config oeenclave-$(COMPILER) --libs)
INCDIR=$(shell pkg-config oeenclave-$(COMPILER) --variable=includedir)

CFLAGS+=-I../../SGXDNN -I/opt/openenclave/include/openenclave/3rdparty/optee/liboeutee

LDFLAGS+=-L../../SGXDNN/bin_ta -l:sgxdnn.a -L/opt/openenclave/lib/openenclave/optee/libteec -lteec
LDFLAGS+=-loelibcxx


all:
	$(MAKE) build
	$(MAKE) keys
	$(MAKE) sign

build:
	@ echo "Compilers used: $(CC), $(CXX)"
	oeedger8r ../sgxdnn_optee.edl --trusted \
		--search-path $(INCDIR) \
		--search-path $(INCDIR)/openenclave/edl/sgx
	$(CXX) -g -c $(CFLAGS) -DOE_API_VERSION=2 enc.cpp -o enc.o
	$(CC) -g -c $(CFLAGS) -DOE_API_VERSION=2 sgxdnn_optee_t.c -o sgxdnn_optee_t.o
	$(CXX) -o sgxdnn_optee sgxdnn_optee_t.o enc.o $(LDFLAGS) $(CRYPTO_LDFLAGS)

sign:
	oesign sign -e sgxdnn_optee -c sgxdnn_optee.conf -k private.pem

clean:
	rm -f enc.o sgxdnn_optee sgxdnn_optee.signed private.pem public.pem sgxdnn_optee_t.o sgxdnn_optee_t.h sgxdnn_optee_t.c sgxdnn_optee_args.h

keys:
	openssl genrsa -out private.pem -3 3072
	openssl rsa -in private.pem -pubout -out public.pem
