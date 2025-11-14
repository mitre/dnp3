cmake_minimum_required(VERSION 3.11)

# Check if we are the root project
set(is_root OFF)
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
  set(is_root ON)
endif()

# Project declaration
set(DNP3TOOLS_MAJOR_VERSION 2)
set(DNP3TOOLS_MINOR_VERSION 0)
set(DNP3TOOLS_PATCH_VERSION 0)
set(DNP3TOOLS_VERSION
    ${DNP3TOOLS_MAJOR_VERSION}.${DNP3TOOLS_MINOR_VERSION}.${DNP3TOOLS_PATCH_VERSION}
)
project(dnp3tools VERSION ${DNP3TOOLS_VERSION})
message("Building Caldera DNP3 payload version ${DNP3TOOLS_VERSION}")

# Packing variables
set(CPACK_PACKAGE_NAME dnp3tools)
set(CPACK_PACKAGE_VENDOR "The MITRE Corporation")
set(CPACK_PACKAGE_DESCRIPTION "MITRE Caldera for OT DNP3 Tools")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/mitre/caldera-ot")
set(CPACK_PACKAGE_VERSION_MAJOR ${DNP3TOOLS_MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${DNP3TOOLS_MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${DNP3TOOLS_PATCH_VERSION})

# Minimum compiler version
if(MSVC_VERSION LESS 1900)
  message(
    FATAL_ERROR
      "Visual Studio earlier than 2015 does not implement std::chrono::steady_clock properly. Use a modern compiler."
  )
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set static linking flags for different platforms
if(WIN32)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
elseif(APPLE)
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
  set(BUILD_SHARED_LIBS OFF)
elseif(UNIX)
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
  set(BUILD_SHARED_LIBS OFF)
  set(CMAKE_EXE_LINKER_FLAGS
      "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")

  # The -static flag creates a fully static binary, but OpenDNP3 leverages
  # shared libraries from glibc at runtime. If compiled statically using the
  # option below, this dynamic behavior may not be handled correctly.
  # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
endif()

set(DNP3_STATIC_LIBS
    ON
    CACHE BOOL "Build OpenDNP3 statically" FORCE)

option(BUILD_TRAINER "Build virtual outstation for training and testing" ON)
option(BUILD_LOCAL "Build using local copies of dependencies" OFF)

if(BUILD_LOCAL)
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/opendnp3-3.1.2)
    add_subdirectory(third_party/opendnp3-3.1.2)
  else()
    message(
      FATAL_ERROR
        "Could not find opendnp3 source code at 'third_party/opendnp3-3.1.2', "
        "save the source to that directory or build with '-DBUILD_LOCAL=OFF'")
  endif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/opendnp3-3.1.2)
endif(BUILD_LOCAL)

set(THIRD_PARTY_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/")

# Executables
add_subdirectory(cpp)

# Packaging Cross-platform packaging tool distributed with CMake.
include(CPack)
