global-incdirs-y += include
incdirs-y += ../third_party/binn/src
incdirs-y += ../../SGXDNN
srcs-y += sgxdnn_wrap.c
srcs-y += libutils_ext.c

# incdirs-y += ../../Include/eigen3_sgx ../../Include/simde ../../Include/AArch64cryptolib ../../Include
# srcs-y += ../../SGXDNN/sgxdnn_main.cpp ../../SGXDNN/json11.cpp ../../SGXDNN/aes-stream.cpp ../../SGXDNN/Crypto.cpp

libnames += binn
libdirs += ../third_party/binn
libdeps += ../third_party/binn/libbinn.a

libnames += :sgxdnn.a
libdirs += ../../SGXDNN/bin_ta
libdeps += ../../SGXDNN/bin_ta/sgxdnn.a

libnames += :libstdc++.a
libdirs += /optee/optee-qemu-v8/toolchains/aarch64/aarch64-none-linux-gnu/lib64
libdeps += /optee/optee-qemu-v8/toolchains/aarch64/aarch64-none-linux-gnu/lib64/libstdc++.a

# To remove a certain compiler flag, add a line like this
#cflags-template_ta.c-y += -Wno-strict-prototypes


