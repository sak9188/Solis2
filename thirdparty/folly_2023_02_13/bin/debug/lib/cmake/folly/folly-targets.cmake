# Generated by CMake

if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 2.5)
   message(FATAL_ERROR "CMake >= 2.6.0 required")
endif()
cmake_policy(PUSH)
cmake_policy(VERSION 2.6...3.18)
#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Protect against multiple inclusion, which would fail when already imported targets are added once more.
set(_targetsDefined)
set(_targetsNotDefined)
set(_expectedTargets)
foreach(_expectedTarget Folly::folly Folly::folly_deps Folly::folly_test_util Folly::follybenchmark)
  list(APPEND _expectedTargets ${_expectedTarget})
  if(NOT TARGET ${_expectedTarget})
    list(APPEND _targetsNotDefined ${_expectedTarget})
  endif()
  if(TARGET ${_expectedTarget})
    list(APPEND _targetsDefined ${_expectedTarget})
  endif()
endforeach()
if("${_targetsDefined}" STREQUAL "${_expectedTargets}")
  unset(_targetsDefined)
  unset(_targetsNotDefined)
  unset(_expectedTargets)
  set(CMAKE_IMPORT_FILE_VERSION)
  cmake_policy(POP)
  return()
endif()
if(NOT "${_targetsDefined}" STREQUAL "")
  message(FATAL_ERROR "Some (but not all) targets in this export set were already defined.\nTargets Defined: ${_targetsDefined}\nTargets not yet defined: ${_targetsNotDefined}\n")
endif()
unset(_targetsDefined)
unset(_targetsNotDefined)
unset(_expectedTargets)


# Compute the installation prefix relative to this file.
get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)
if(_IMPORT_PREFIX STREQUAL "/")
  set(_IMPORT_PREFIX "")
endif()

# Create imported target Folly::folly
add_library(Folly::folly STATIC IMPORTED)

set_target_properties(Folly::folly PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "_CRT_NONSTDC_NO_WARNINGS;_CRT_SECURE_NO_WARNINGS;_SCL_SECURE_NO_WARNINGS;_ENABLE_EXTENDED_ALIGNED_STORAGE;_STL_EXTRA_DISABLED_WARNINGS=4774 4987;\$<\$<BOOL:>:_HAS_AUTO_PTR_ETC=1>;\$<\$<BOOL:ON>:WIN32_LEAN_AND_MEAN>;\$<\$<BOOL:ON>:BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE>"
  INTERFACE_COMPILE_FEATURES "cxx_generic_lambdas"
  INTERFACE_COMPILE_OPTIONS "/EHs;/GF;/Zc:referenceBinding;/Zc:rvalueCast;/Zc:implicitNoexcept;/Zc:strictStrings;/Zc:threadSafeInit;/Zc:throwingNew;/permissive-;/std:c++17;/wd4191;/wd4291;/wd4309;/wd4310;/wd4366;/wd4587;/wd4592;/wd4628;/wd4723;/wd4724;/wd4868;/wd4996;/wd4068;/wd4091;/wd4146;/wd4800;/wd4018;/wd4365;/wd4388;/wd4389;/wd4100;/wd4459;/wd4505;/wd4701;/wd4702;/wd4061;/wd4127;/wd4200;/wd4201;/wd4296;/wd4316;/wd4324;/wd4355;/wd4371;/wd4435;/wd4514;/wd4548;/wd4571;/wd4574;/wd4582;/wd4583;/wd4619;/wd4623;/wd4625;/wd4626;/wd4643;/wd4647;/wd4668;/wd4706;/wd4710;/wd4711;/wd4714;/wd4820;/wd5026;/wd5027;/wd5031;/wd5045;/we4099;/we4129;/we4566"
  INTERFACE_LINK_LIBRARIES "Folly::folly_deps"
)

# Create imported target Folly::folly_deps
add_library(Folly::folly_deps INTERFACE IMPORTED)

set_target_properties(Folly::folly_deps PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "D:/temp_scratch/installed/boost-yNYWTi1iGNG4ZlhZxdv3HnZBDA8LnEEPYsSr6Ohq0ys/include/boost-1_78;D:/temp_scratch/installed/double-conversion-XbS5oaVVS3MewYddGDZJO2QrAjCJND4jpiZ0TeKiDh8/include;D:/temp_scratch/installed/gflags-_1eoZXarVSF7Vt7IXOHHs3j2sAaZXtMH1R48kE0GQAE/include;D:/temp_scratch/installed/glog-d2qz0APm2elymVKTpqhacAXkRpF3nFB-nA2Nh9HHboU/include;D:/temp_scratch/installed/libevent-6LTzlxGYwYmfKiaibVw-2ooNBc_sU_P4dQ6IjKzMzUk/include;D:/temp_scratch/installed/zlib-NS2AS2m-D_GjlCXBBIsPBrbdHDpRBg1xydZ2eDSC_Xg/include;D:/temp_scratch/installed/openssl-R61uB9b6PwBtwyXtOyy_PTwQQABctuQT54-KlwCAQPQ/include;D:/temp_scratch/installed/lz4-YM_zxJn51yUTJsfmPISda4L_8VhLRY1dy1x9rn5OGdM/include;D:/temp_scratch/installed/zstd-FTC5VoKu1WA0ZiiLfz0WfTlOQLUGCZ-LjrlXDVc_hjc/include;D:/temp_scratch/installed/snappy-bc0jMfF1bwkuJTjOdZuExC9YGdzh1VInkGd4LvbfGmc/include;${_IMPORT_PREFIX}/include"
  INTERFACE_LINK_LIBRARIES "fmt::fmt;Threads::Threads;Iphlpapi.lib;Ws2_32.lib;Boost::context;Boost::filesystem;Boost::program_options;Boost::regex;Boost::system;Boost::thread;D:/temp_scratch/installed/double-conversion-XbS5oaVVS3MewYddGDZJO2QrAjCJND4jpiZ0TeKiDh8/lib/double-conversion.lib;gflags_shared;D:/temp_scratch/installed/glog-d2qz0APm2elymVKTpqhacAXkRpF3nFB-nA2Nh9HHboU/lib/glogd.lib;D:/temp_scratch/installed/libevent-6LTzlxGYwYmfKiaibVw-2ooNBc_sU_P4dQ6IjKzMzUk/lib/event.lib;D:/temp_scratch/installed/zlib-NS2AS2m-D_GjlCXBBIsPBrbdHDpRBg1xydZ2eDSC_Xg/lib/zlibd.lib;D:/temp_scratch/installed/openssl-R61uB9b6PwBtwyXtOyy_PTwQQABctuQT54-KlwCAQPQ/lib/libssl.lib;D:/temp_scratch/installed/openssl-R61uB9b6PwBtwyXtOyy_PTwQQABctuQT54-KlwCAQPQ/lib/libcrypto.lib;D:/temp_scratch/installed/lz4-YM_zxJn51yUTJsfmPISda4L_8VhLRY1dy1x9rn5OGdM/lib/lz4.lib;D:/temp_scratch/installed/zstd-FTC5VoKu1WA0ZiiLfz0WfTlOQLUGCZ-LjrlXDVc_hjc/lib/zstd_static.lib;D:/temp_scratch/installed/snappy-bc0jMfF1bwkuJTjOdZuExC9YGdzh1VInkGd4LvbfGmc/lib/snappy.lib"
)

# Create imported target Folly::folly_test_util
add_library(Folly::folly_test_util STATIC IMPORTED)

set_target_properties(Folly::folly_test_util PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "_CRT_NONSTDC_NO_WARNINGS;_CRT_SECURE_NO_WARNINGS;_SCL_SECURE_NO_WARNINGS;_ENABLE_EXTENDED_ALIGNED_STORAGE;_STL_EXTRA_DISABLED_WARNINGS=4774 4987;\$<\$<BOOL:>:_HAS_AUTO_PTR_ETC=1>;\$<\$<BOOL:ON>:WIN32_LEAN_AND_MEAN>;\$<\$<BOOL:ON>:BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE>"
  INTERFACE_COMPILE_OPTIONS "/EHs;/GF;/Zc:referenceBinding;/Zc:rvalueCast;/Zc:implicitNoexcept;/Zc:strictStrings;/Zc:threadSafeInit;/Zc:throwingNew;/permissive-;/std:c++17;/wd4191;/wd4291;/wd4309;/wd4310;/wd4366;/wd4587;/wd4592;/wd4628;/wd4723;/wd4724;/wd4868;/wd4996;/wd4068;/wd4091;/wd4146;/wd4800;/wd4018;/wd4365;/wd4388;/wd4389;/wd4100;/wd4459;/wd4505;/wd4701;/wd4702;/wd4061;/wd4127;/wd4200;/wd4201;/wd4296;/wd4316;/wd4324;/wd4355;/wd4371;/wd4435;/wd4514;/wd4548;/wd4571;/wd4574;/wd4582;/wd4583;/wd4619;/wd4623;/wd4625;/wd4626;/wd4643;/wd4647;/wd4668;/wd4706;/wd4710;/wd4711;/wd4714;/wd4820;/wd5026;/wd5027;/wd5031;/wd5045;/we4099;/we4129;/we4566"
  INTERFACE_LINK_LIBRARIES "Folly::folly"
)

# Create imported target Folly::follybenchmark
add_library(Folly::follybenchmark STATIC IMPORTED)

set_target_properties(Folly::follybenchmark PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "_CRT_NONSTDC_NO_WARNINGS;_CRT_SECURE_NO_WARNINGS;_SCL_SECURE_NO_WARNINGS;_ENABLE_EXTENDED_ALIGNED_STORAGE;_STL_EXTRA_DISABLED_WARNINGS=4774 4987;\$<\$<BOOL:>:_HAS_AUTO_PTR_ETC=1>;\$<\$<BOOL:ON>:WIN32_LEAN_AND_MEAN>;\$<\$<BOOL:ON>:BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE>"
  INTERFACE_COMPILE_OPTIONS "/EHs;/GF;/Zc:referenceBinding;/Zc:rvalueCast;/Zc:implicitNoexcept;/Zc:strictStrings;/Zc:threadSafeInit;/Zc:throwingNew;/permissive-;/std:c++17;/wd4191;/wd4291;/wd4309;/wd4310;/wd4366;/wd4587;/wd4592;/wd4628;/wd4723;/wd4724;/wd4868;/wd4996;/wd4068;/wd4091;/wd4146;/wd4800;/wd4018;/wd4365;/wd4388;/wd4389;/wd4100;/wd4459;/wd4505;/wd4701;/wd4702;/wd4061;/wd4127;/wd4200;/wd4201;/wd4296;/wd4316;/wd4324;/wd4355;/wd4371;/wd4435;/wd4514;/wd4548;/wd4571;/wd4574;/wd4582;/wd4583;/wd4619;/wd4623;/wd4625;/wd4626;/wd4643;/wd4647;/wd4668;/wd4706;/wd4710;/wd4711;/wd4714;/wd4820;/wd5026;/wd5027;/wd5031;/wd5045;/we4099;/we4129;/we4566"
  INTERFACE_LINK_LIBRARIES "Folly::folly"
)

if(CMAKE_VERSION VERSION_LESS 3.0.0)
  message(FATAL_ERROR "This file relies on consumers using CMake 3.0.0 or greater.")
endif()

# Load information for each installed configuration.
get_filename_component(_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
file(GLOB CONFIG_FILES "${_DIR}/folly-targets-*.cmake")
foreach(f ${CONFIG_FILES})
  include(${f})
endforeach()

# Cleanup temporary variables.
set(_IMPORT_PREFIX)

# Loop over all imported files and verify that they actually exist
foreach(target ${_IMPORT_CHECK_TARGETS} )
  foreach(file ${_IMPORT_CHECK_FILES_FOR_${target}} )
    if(NOT EXISTS "${file}" )
      message(FATAL_ERROR "The imported target \"${target}\" references the file
   \"${file}\"
but this file does not exist.  Possible reasons include:
* The file was deleted, renamed, or moved to another location.
* An install or uninstall procedure did not complete successfully.
* The installation package was faulty and contained
   \"${CMAKE_CURRENT_LIST_FILE}\"
but not all the files it references.
")
    endif()
  endforeach()
  unset(_IMPORT_CHECK_FILES_FOR_${target})
endforeach()
unset(_IMPORT_CHECK_TARGETS)

# This file does not depend on other imported targets which have
# been exported from the same project but in a separate export set.

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
cmake_policy(POP)
