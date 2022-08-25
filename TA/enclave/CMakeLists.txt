# Copyright (c) Open Enclave SDK contributors.
# Licensed under the MIT License.

# Use the edger8r to generate C bindings from the EDL file.
add_custom_command(
  OUTPUT sgxdnn_optee_t.h sgxdnn_optee_t.c sgxdnn_optee_args.h
  DEPENDS ${CMAKE_SOURCE_DIR}/sgxdnn_optee.edl
  COMMAND
    openenclave::oeedger8r --trusted ${CMAKE_SOURCE_DIR}/sgxdnn_optee.edl
    --search-path ${OE_INCLUDEDIR} --search-path
    ${OE_INCLUDEDIR}/openenclave/edl/optee)

# For AArch64cryptolib
list(APPEND TEMP_AWK_STRING "'($$1" "==" "\"<genassym>\")" "{" "print" "\"#define" "\"" "$$2" "\"\\t\"" "$$3" "}'")
# awk '($1 == "<genassym>") { print "#define " $2 "\t" $3 }'
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/assym.s
  COMMAND ${CMAKE_C_COMPILER} -O0 -S ${CMAKE_CURRENT_SOURCE_DIR}/../../Include/AArch64cryptolib/genassym.c -o - | awk ${TEMP_AWK_STRING} > ${CMAKE_CURRENT_BINARY_DIR}/assym.s) 


add_enclave_optee(
  TARGET
  sgxdnn_optee
  UUID
  cf215436-3453-4888-bcc2-59dc1bb8b5b7
  SOURCES
  ../../SGXDNN/sgxdnn_main.cpp
  ../../SGXDNN/json11.cpp
  ../../SGXDNN/aes-stream.cpp
  ../../SGXDNN/Crypto.cpp
  ../../SGXDNN/optee-stub.cpp
  ../../Include/AArch64cryptolib/AArch64cryptolib_aes_cbc.c
  ../../Include/AArch64cryptolib/AArch64cryptolib_opt_big/aes/aes_core.S
  ../../Include/AArch64cryptolib/AArch64cryptolib_opt_big/sha1/sha1_core.S
  ../../Include/AArch64cryptolib/AArch64cryptolib_opt_big/sha256/sha256_core.S
  ../../Include/AArch64cryptolib/AArch64cryptolib_opt_big/aes_cbc_sha1/aes128cbc_sha1_hmac.S
  ../../Include/AArch64cryptolib/AArch64cryptolib_opt_big/aes_cbc_sha256/aes128cbc_sha256_hmac.S
  ../../Include/AArch64cryptolib/AArch64cryptolib_opt_big/aes_cbc_sha1/sha1_hmac_aes128cbc_dec.S
  ../../Include/AArch64cryptolib/AArch64cryptolib_opt_big/aes_cbc_sha256/sha256_hmac_aes128cbc_dec.S
  assym.s
  ../../Include/AArch64cryptolib/AArch64cryptolib_aes_gcm.c
  enc.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/sgxdnn_optee_t.c
  )

enclave_include_directories(sgxdnn_optee PRIVATE
  ../../SGXDNN/../Include/eigen 
  ../../SGXDNN/../Include/simde 
  ../../SGXDNN/../Include/AArch64cryptolib 
  ../../SGXDNN/../Include 
  ../../SGXDNN/crypto)
set (COMMON_FLAGS "-Wno-enum-compare -DSIMDE_ENABLE_NATIVE_ALIASES -march=armv8-a+simd+crypto -fno-stack-protector -DUSE_SGX=1 -Wno-narrowing -flax-vector-conversions")

enclave_include_directories(sgxdnn_optee PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
enclave_link_libraries(sgxdnn_optee
  openenclave::oelibcxx
  openenclave::oeenclave
  openenclave::oecryptombedtls
  )

enclave_include_directories(sgxdnn_optee PRIVATE
  /opt/openenclave/include
  /opt/openenclave/include/openenclave/3rdparty
  /opt/openenclave/include/openenclave/3rdparty/optee/liboeutee)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS} -I/opt/openenclave/include/openenclave/3rdparty/libc")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=0 -std=c++11 -Os -fdata-sections -ffunction-sections -Wl,--gc-sections -fno-exceptions -I/opt/openenclave/include/openenclave/3rdparty/libcxx")

enclave_include_directories(sgxdnn_optee PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../../SGXDNN)


# # link with sgxdnn.a
# add_custom_command(OUTPUT /optee/optee-qemu-v8/ai_tee/slalom-optee/SGXDNN/bin_ta/sgxdnn.a
#   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../SGXDNN/
#   COMMAND env CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} AR=${CMAKE_AR} CROSS_COMPILE=TRUE make static)
# add_custom_target(sgxdnn
#   DEPENDS /optee/optee-qemu-v8/ai_tee/slalom-optee/SGXDNN/bin_ta/sgxdnn.a)
# 
# add_dependencies(sgxdnn_optee sgxdnn)


