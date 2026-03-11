# ################################################################
# Copyright (c) Meta Platforms, Inc. and affiliates.
# All rights reserved.
#
# This source code is licensed under both the BSD-style license (found in the
# LICENSE file in the root directory of this source tree) and the GPLv2 (found
# in the COPYING file in the root directory of this source tree).
# ################################################################

project(tests)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

# Define directories
set(LIBRARY_DIR ${ZSTD_SOURCE_DIR}/lib)
set(TESTS_DIR ${ZSTD_SOURCE_DIR}/tests)

include_directories(${TESTS_DIR} ${LIBRARY_DIR})

#
# test-zstd-api
#
add_executable(test-zstd-api ${TESTS_DIR}/test-zstd.c)
target_link_libraries(test-zstd-api libzstd_static)
add_test(NAME test-zstd-api COMMAND test-zstd-api)

# All other tests (datagen, fullbench, fuzzer, zstreamtest, playTests, paramgrill)
# are disabled to simplify the test suite as requested.
