configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/config.h"
  )
# add the binary tree to the search path for include files
# so that we will find config.h
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_executable(dnp3actions ./dnp3_cli.cpp ./dnp3_actions.cpp)
target_link_libraries(dnp3actions opendnp3 ser4cpp exe4cpp asio)
target_link_options(dnp3actions PRIVATE -static-libgcc -static-libstdc++ -static)