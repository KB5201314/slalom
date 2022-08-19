# Copyright (c) Open Enclave SDK contributors.
# Licensed under the MIT License.

cmake_minimum_required(VERSION 3.11)

# Specific enclave type
set(OE_TRUSTZONE 1)

project(sgxdnn_optee LANGUAGES C CXX ASM)

# This will bring in the Open Enclave targets under the openenclave:: namespace.
find_package(OpenEnclave CONFIG REQUIRED)

include(/optee/openenclave_qemu/sdk/cmake/enclave_cmake_wrappers.cmake)

# Replace include(add_enclave.cmake) with self-mantained codes
#include(/optee/openenclave_qemu/sdk/cmake/add_enclave.cmake)
macro (add_enclave_optee)
  set(oneValueArgs TARGET UUID KEY CRYPTO_LIB CXX)
  set(multiValueArgs SOURCES)
  cmake_parse_arguments(ENCLAVE "" "${oneValueArgs}" "${multiValueArgs}"
                        ${ARGN})

  # Set up the linker flags exactly as we need them such that the resulting
  # binary be compatible with OP-TEE's loader.
  set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)
  set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS)
  set(CMAKE_EXE_EXPORTS_C_FLAG)

  string(REPLACE "gcc" "ld" LINKER ${CMAKE_C_COMPILER})
  set(CMAKE_C_LINK_EXECUTABLE
      "${LINKER} <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> <LINK_LIBRARIES> -lgcc -o <TARGET>"
  )
  set(CMAKE_CXX_LINK_EXECUTABLE
      "${LINKER} <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> <LINK_LIBRARIES> -lgcc -o <TARGET>"
  )

  # Generate linker script from template.
  string(REPLACE "gcc" "cpp" C_PREPROCESSOR ${CMAKE_C_COMPILER})
  set(TA_LINKER_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/ta.ld)
  set(TA_LINKER_SCRIPT
      ${TA_LINKER_SCRIPT}
      PARENT_SCOPE)
  add_custom_target(
    ${ENCLAVE_TARGET}.ld
    COMMAND ${C_PREPROCESSOR} -Wp,-P -DASM=1 -DARM64 -nostdinc
            ${OE_TZ_TA_DEV_KIT_LINKER_SCRIPT_TEMPLATE} > ${TA_LINKER_SCRIPT}
    SOURCES ${OE_TZ_TA_DEV_KIT_LINKER_SCRIPT_TEMPLATE}
    DEPENDS ${OE_TZ_TA_DEV_KIT_LINKER_SCRIPT_TEMPLATE}
    BYPRODUCTS ${TA_LINKER_SCRIPT})

  # Ask GCC where is libgcc.
  execute_process(
    COMMAND ${CMAKE_C_COMPILER} ${OE_TZ_TA_C_FLAGS} -print-libgcc-file-name
    OUTPUT_VARIABLE LIBGCC_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  get_filename_component(LIBGCC_PATH ${LIBGCC_PATH} DIRECTORY)

  # Set up the target.
  add_executable(${ENCLAVE_TARGET} ${ENCLAVE_SOURCES})
  set_property(TARGET ${ENCLAVE_TARGET} PROPERTY C_STANDARD 99)
  set_target_properties(${ENCLAVE_TARGET} PROPERTIES OUTPUT_NAME
                                                     ${ENCLAVE_UUID})
  set_target_properties(${ENCLAVE_TARGET} PROPERTIES SUFFIX ".elf")
  add_dependencies(${ENCLAVE_TARGET} ${ENCLAVE_TARGET}.ld)
  target_include_directories(${ENCLAVE_TARGET}
                             PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/optee)

  target_link_libraries(${ENCLAVE_TARGET} openenclave::oeenclave)

  # Note that the OpenSSL-based crypto library is currently not supported on OP-TEE.
  target_link_libraries(${ENCLAVE_TARGET} openenclave::oecryptombedtls)

  if (ENCLAVE_CXX)
    target_link_libraries(${ENCLAVE_TARGET} openenclave::oelibcxx)
  endif ()

  # Strip unneeded bits.
  string(REPLACE "gcc" "objcopy" OBJCOPY ${CMAKE_C_COMPILER})
  add_custom_target(
    ${ENCLAVE_TARGET}.stripped.elf
    COMMAND ${OBJCOPY} --strip-unneeded $<TARGET_FILE:${ENCLAVE_TARGET}>
            $<TARGET_FILE_DIR:${ENCLAVE_TARGET}>/${ENCLAVE_UUID}.stripped.elf
    BYPRODUCTS $<TARGET_FILE_DIR:${ENCLAVE_TARGET}>/${ENCLAVE_UUID}.stripped.elf
  )
  add_dependencies(${ENCLAVE_TARGET}.stripped.elf ${ENCLAVE_TARGET})

  # Sign the TA with the given key, or with the default key if none was given.
  if (NOT ENCLAVE_KEY)
    set(ENCLAVE_KEY ${OE_TZ_TA_DEV_KIT_DEFAULT_SIGNING_KEY})
  endif ()
  add_custom_target(
    ${ENCLAVE_TARGET}.ta ALL
    COMMAND
      ${OE_TZ_TA_DEV_KIT_SIGN_TOOL} --key ${ENCLAVE_KEY} --uuid ${ENCLAVE_UUID}
      --version 0 --in
      $<TARGET_FILE_DIR:${ENCLAVE_TARGET}>/${ENCLAVE_UUID}.stripped.elf --out
      $<TARGET_FILE_DIR:${ENCLAVE_TARGET}>/${ENCLAVE_UUID}.ta
    BYPRODUCTS $<TARGET_FILE_DIR:${ENCLAVE_TARGET}>/${ENCLAVE_UUID}.ta)
  add_dependencies(${ENCLAVE_TARGET}.ta ${ENCLAVE_TARGET}.stripped.elf)

  # Set linker options.
  # NOTE: This has to be at the end, apparently:
  #       https://gitlab.kitware.com/cmake/cmake/issues/17210
  set(CMAKE_EXE_LINKER_FLAGS
      "-T ${TA_LINKER_SCRIPT} -L${LIBGCC_PATH} --entry=_start")
  if (ENCLAVE_CXX)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --eh-frame-hdr")
  endif ()
endmacro ()



add_subdirectory(enclave)
add_subdirectory(host)
