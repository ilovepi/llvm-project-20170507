# Build for all components of the Syinge runtime support library.

# Syinge runtime library implementation files.
set(SYRINGE_SOURCES
    syringe_rt.cpp)

set(SYRINGE_HEADERS
  syringe_rt.h)

# Create list of all source files for
# consumption by tests.
set(SYRINGE_ALL_SOURCE_FILES
  ${SYRINGE_SOURCES}
  ${SYRINGE_HEADERS}
)
list(REMOVE_DUPLICATES SYRINGE_ALL_SOURCE_FILES)
# Make list that uses absolute paths
set(SYRINGE_ALL_SOURCE_FILES_ABS_PATHS "")
foreach (src_file ${SYRINGE_ALL_SOURCE_FILES})
  list(APPEND
      SYRINGE_ALL_SOURCE_FILES_ABS_PATHS
    "${CMAKE_CURRENT_SOURCE_DIR}/${src_file}")
endforeach()


# Now put it all together...
include_directories(..)
include_directories(../../include)

set(SYRINGE_CFLAGS ${SANITIZER_COMMON_CFLAGS})
set(SYRINGE_COMMON_DEFINITIONS SYRINGE_HAS_EXCEPTIONS=1)

# We don't need RTTI in Syringe, so turn that off.
append_rtti_flag(OFF SYRINGE_CFLAGS)

append_list_if(
    COMPILER_RT_HAS_SYRINGE_COMPILER_FLAG SYRINGE_SUPPORTED=1 SYRINGE_COMMON_DEFINITIONS)

add_compiler_rt_component(syringe)

set(SYRINGE_COMMON_RUNTIME_OBJECT_LIBS
    RTSanitizerCommon
    RTSanitizerCommonLibc)


if (TARGET cxx-headers OR HAVE_LIBCXX)
    set(SYRINGE_DEPS cxx-headers)
endif()

if (APPLE)
  set(SYRINGE_LINK_LIBS ${SANITIZER_COMMON_LINK_LIBS})
  add_weak_symbols("sanitizer_common" WEAK_SYMBOL_LINK_FLAGS)
  #add_weak_symbols("syringe" WEAK_SYMBOL_LINK_FLAGS)

  add_compiler_rt_object_libraries(RTSyringe
    OS ${SYRINGE_SUPPORTED_OS}
    ARCHS ${SYRINGE_SUPPORTED_ARCH}
    SOURCES ${SYRINGE_SOURCES} ${${arch}_SOURCES}
    ADDITIONAL_HEADERS ${SYRINGE_HEADERS}
    CFLAGS ${SYRINGE_CFLAGS}
    DEFS ${SYRINGE_COMMON_DEFINITIONS}
    DEPS ${SYRINGE_DEPS})

    # We only support running on osx for now.
  add_compiler_rt_runtime(clang_rt.syringe
    STATIC
    OS ${SYRINGE_SUPPORTED_OS}
    ARCHS ${SYRINGE_SUPPORTED_ARCH}
    OBJECT_LIBS RTSyringe
                RTSanitizerCommon
                RTSanitizerCommonLibc
    CFLAGS ${SYRINGE_CFLAGS}
    DEFS ${SYRINGE_COMMON_DEFINITIONS}
    LINK_FLAGS ${SANITIZER_COMMON_LINK_FLAGS} ${WEAK_SYMBOL_LINK_FLAGS}
    LINK_LIBS ${SYRINGE_LINK_LIBS}
    PARENT_TARGET syringe)

else() # not Apple
  foreach(arch ${SYRINGE_SUPPORTED_ARCH})
    if(NOT CAN_TARGET_${arch})
      continue()
    endif()
    add_compiler_rt_object_libraries(RTSyringe
      ARCHS ${arch}
      SOURCES ${SYRINGE_SOURCES} ${${arch}_SOURCES}
      ADDITIONAL_HEADERS ${SYRINGE_HEADERS}
      CFLAGS ${SYRINGE_CFLAGS}
      DEFS ${SYRINGE_COMMON_DEFINITIONS}
      DEPS ${SYRINGE_DEPS})

    # Common Syringe archive for instrumented binaries.
    add_compiler_rt_runtime(clang_rt.syringe
      STATIC
      ARCHS ${arch}
      CFLAGS ${SYRINGE_CFLAGS}
      DEFS ${SYRINGE_COMMON_DEFINITIONS}
      OBJECT_LIBS ${SYRINGE_COMMON_RUNTIME_OBJECT_LIBS}  RTSyringe
      PARENT_TARGET syringe)
  endforeach()
endif() # not Apple

#if(COMPILER_RT_INCLUDE_TESTS)
  #add_subdirectory(tests)
#endif()
