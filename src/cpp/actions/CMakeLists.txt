configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/config.h")

add_executable(dnp3-actions actions.cpp cli.cpp)

target_include_directories(
  dnp3-actions
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
  PRIVATE ${THIRD_PARTY_INCLUDE}
  PRIVATE ${dnp3_logger_INCLUDE})

target_link_libraries(dnp3-actions PRIVATE opendnp3)

if(WIN32)
  target_link_libraries(dnp3-actions PRIVATE wsock32 ws2_32)
endif()

if(APPLE)
  set(NEW_EXE_NAME "dnp3-actions_darwin")
elseif(UNIX AND NOT APPLE)
  set(NEW_EXE_NAME "dnp3-actions")
else()
  set(NEW_EXE_NAME "dnp3-actions.exe")
endif()

add_custom_command(
  TARGET dnp3-actions
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:dnp3-actions>
          ${PROJECT_SOURCE_DIR}/bin/${NEW_EXE_NAME})
