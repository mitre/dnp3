cmake_minimum_required (VERSION 3.11)

# Check if we are the root project
set(is_root OFF)
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    set(is_root ON)
endif()

# Project declaration
set(DNP3TOOLS_MAJOR_VERSION 1)
set(DNP3TOOLS_MINOR_VERSION 0)
set(DNP3TOOLS_PATCH_VERSION 0)
set(DNP3TOOLS_VERSION ${DNP3TOOLS_MAJOR_VERSION}.${DNP3TOOLS_MINOR_VERSION}.${DNP3TOOLS_PATCH_VERSION})
project(dnp3tools VERSION ${DNP3TOOLS_VERSION})
message("Building tooling version ${DNP3TOOLS_VERSION}") 

# Packing variable
set(CPACK_PACKAGE_NAME dnp3tools)
set(CPACK_PACKAGE_VENDOR "The MITRE Corporation")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "DNP3 Tooling")
set(CPACK_PACKAGE_DESCRIPTION "Tooling using OpenDNP3")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/mitre/caldera-ot")
set(CPACK_PACKAGE_VERSION_MAJOR ${DNP3TOOLS_MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${DNP3TOOLS_MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${DNP3TOOLS_PATCH_VERSION})

# Minimum compiler version
if(MSVC_VERSION LESS 1900)
    message(FATAL_ERROR "Visual Studio earlier than 2015 does not implement std::chrono::steady_clock properly. Use a modern compiler.")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Clang utilities
include(./utils/cmake/ClangFormat.cmake)
include(./utils/cmake/ClangTidy.cmake)

set(DNP3_STATIC_LIBS_BY_DEFAULT ON)

set(BUILD_SHARED_LIBS OFF)

# External dependencies
include(./deps/cmake/asio.cmake)
include(./deps/cmake/exe4cpp.cmake)
include(./deps/cmake/ser4cpp.cmake)
include_directories(./deps/include)

# Library
#add_subdirectory(./cpp/lib)
add_library(opendnp3 STATIC IMPORTED)
set_target_properties(opendnp3 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/deps/lib/libopendnp3.a)
set_target_properties(opendnp3 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/deps/include)
add_subdirectory(./src/dnp3-actions)
add_subdirectory(./src/dnp3-poll)
add_subdirectory(./src/dnp3-server)

# Define utility targets
if(is_root)
    define_clang_format()
    define_clang_tidy()
endif()

# Packaging
# Cross-platform packaging tool distributed with CMake.
include(CPack)