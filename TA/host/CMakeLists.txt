# Copyright (c) Open Enclave SDK contributors.
# Licensed under the MIT License.
add_custom_command(
  OUTPUT sgxdnn_optee_u.h sgxdnn_optee_u.c sgxdnn_optee_args.h
  DEPENDS ${CMAKE_SOURCE_DIR}/sgxdnn_optee.edl
  COMMAND
    openenclave::oeedger8r --untrusted ${CMAKE_SOURCE_DIR}/sgxdnn_optee.edl
    --search-path ${OE_INCLUDEDIR} --search-path
    ${OE_INCLUDEDIR}/openenclave/edl/optee)

add_executable(sgxdnn_optee_bridge sgxdnn_optee_bridge.cpp
                               ${CMAKE_CURRENT_BINARY_DIR}/sgxdnn_optee_u.c)

target_include_directories(sgxdnn_optee_bridge PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(sgxdnn_optee_bridge openenclave::oehost)
