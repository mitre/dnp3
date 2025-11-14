include(FetchContent)

if(NOT BUILD_LOCAL)
  FetchContent_Declare(
    opendnp3
    GIT_REPOSITORY https://github.com/dnp3/opendnp3.git
    GIT_TAG "release")
  FetchContent_MakeAvailable(opendnp3)
endif(NOT BUILD_LOCAL)

set(dnp3_logger_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/logger")

add_subdirectory(actions)

if(BUILD_TRAINER)
  message("Building DNP3 training outstation")
  add_subdirectory(trainer)
endif(BUILD_TRAINER)