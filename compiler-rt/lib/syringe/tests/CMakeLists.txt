include_directories(..)

add_custom_target(SyringeUnitTests)
set_target_properties(SyringeUnitTests PROPERTIES FOLDER "Syringe unittests")

# Sanity check SYRINGE_ALL_SOURCE_FILES_ABS_PATHS
list(LENGTH SYRINGE_ALL_SOURCE_FILES_ABS_PATHS XASFAP_LENGTH)
if (${XASFAP_LENGTH} EQUAL 0)
  message(FATAL_ERROR "SYRINGE_ALL_SOURCE_FILES_ABS_PATHS cannot be empty")
endif()
unset(XASFAP_LENGTH)
foreach (src_file ${SYRINGE_ALL_SOURCE_FILES_ABS_PATHS})
  if (NOT EXISTS "${src_file}")
    message(FATAL_ERROR "Source file \"${src_file}\" does not exist")
  endif()
endforeach()

set(SYRINGE_UNITTEST_CFLAGS
  ${SYRINGE_CFLAGS}
  ${COMPILER_RT_UNITTEST_CFLAGS}
  ${COMPILER_RT_GTEST_CFLAGS}
  -I${COMPILER_RT_SOURCE_DIR}/include
  -I${COMPILER_RT_SOURCE_DIR}/lib/syringe
  -I${COMPILER_RT_SOURCE_DIR}/lib)

function(add_syringe_lib library)
  add_library(${library} STATIC ${ARGN})
  set_target_properties(${library} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    FOLDER "Compiler-RT Runtime tests")
endfunction()

function(get_syringe_lib_for_arch arch lib)
  if(APPLE)
    set(tgt_name "RTSyringe.test.osx")
  else()
    set(tgt_name "RTSyringe.test.${arch}")
  endif()
  set(${lib} "${tgt_name}" PARENT_SCOPE)
endfunction()

set(SYRINGE_TEST_ARCH ${SYRINGE_SUPPORTED_ARCH})
set(SYRINGE_UNITTEST_LINK_FLAGS
  ${CMAKE_THREAD_LIBS_INIT}
	-l${SANITIZER_CXX_ABI_LIBRARY}
  -fsyringe
  )
if (NOT APPLE)
  append_list_if(COMPILER_RT_HAS_LIBM -lm SYRINGE_UNITTEST_LINK_FLAGS)
  append_list_if(COMPILER_RT_HAS_LIBRT -lrt SYRINGE_UNITTEST_LINK_FLAGS)
  append_list_if(COMPILER_RT_HAS_LIBDL -ldl SYRINGE_UNITTEST_LINK_FLAGS)
  append_list_if(COMPILER_RT_HAS_LIBPTHREAD -pthread SYRINGE_UNITTEST_LINK_FLAGS)
endif()

macro(add_syringe_unittest testname)
  cmake_parse_arguments(TEST "" "" "SOURCES;HEADERS" ${ARGN})
  if(UNIX AND NOT APPLE)
    set(CMAKE_DL_LIBS_INIT "")
    foreach(arch ${SYRINGE_TEST_ARCH})
      set(TEST_OBJECTS)
      get_syringe_lib_for_arch(${arch} SYRINGE_RUNTIME_LIBS)
      generate_compiler_rt_tests(TEST_OBJECTS
        SyringeUnitTests "${testname}-${arch}-Test" "${arch}"
        SOURCES ${TEST_SOURCES} ${COMPILER_RT_GTEST_SOURCE}
        # Note that any change in the implementations will cause all the unit
        # tests to be re-built. This is by design, but may be cumbersome during
        # the build/test cycle.
        COMPILE_DEPS ${TEST_SOURCES} ${COMPILER_RT_GTEST_SOURCE}
        ${SYRINGE_HEADERS} ${SYRINGE_ALL_SOURCE_FILES_ABS_PATHS}
        RUNTIME "${SYRINGE_RUNTIME_LIBS}"
        DEPS gtest syringe llvm-syringe
        CFLAGS ${SYRINGE_UNITTEST_CFLAGS}
        LINK_FLAGS ${TARGET_LINK_FLAGS} ${SYRINGE_UNITTEST_LINK_FLAGS})
      set_target_properties(SyringeUnitTests
        PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    endforeach()
  endif()
endmacro()

if(COMPILER_RT_CAN_EXECUTE_TESTS)
  if (APPLE)
    add_syringe_lib("RTSyringe.test.osx"
      $<TARGET_OBJECTS:RTSyringe.osx>
      $<TARGET_OBJECTS:RTSanitizerCommon.osx>
      $<TARGET_OBJECTS:RTSanitizerCommonLibc.osx>)
  else()
    foreach(arch ${SYRINGE_SUPPORTED_ARCH})
      add_syringe_lib("RTSyringe.test.${arch}"
        $<TARGET_OBJECTS:RTSyringe.${arch}>
      $<TARGET_OBJECTS:RTSanitizerCommon.${arch}>
      $<TARGET_OBJECTS:RTSanitizerCommonLibc.${arch}>)
  endforeach()
  endif()
  add_subdirectory(unit)
endif()
