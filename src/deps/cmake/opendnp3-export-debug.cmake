#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "opendnp3::opendnp3" for configuration "Debug"
set_property(TARGET opendnp3::opendnp3 APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(opendnp3::opendnp3 PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libopendnp3.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS opendnp3::opendnp3 )
list(APPEND _IMPORT_CHECK_FILES_FOR_opendnp3::opendnp3 "${_IMPORT_PREFIX}/lib/libopendnp3.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
