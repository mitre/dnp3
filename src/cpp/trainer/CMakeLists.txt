FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v5.0.0)

set(FTXUI_QUIET
    ON
    CACHE BOOL "Silence FTXUI build" FORCE)
FetchContent_MakeAvailable(ftxui)

add_executable(dnp3-outstation
  main.cpp
  outstation/types.cpp
  outstation/io_table.cpp
  outstation/devices.cpp
  ui/main_component.cpp
  ui/system_page.cpp
  ui/io_page.cpp
  ui/log_displayer.cpp
  ui/renderer.cpp)

target_include_directories(dnp3-outstation
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
  PRIVATE "${THIRD_PARTY_INCLUDE}"
  PRIVATE "${dnp3_logger_INCLUDE}"
)

target_link_libraries(dnp3-outstation
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
  PRIVATE opendnp3)

if(WIN32)
  target_link_libraries(dnp3-outstation PRIVATE wsock32 ws2_32)
endif()

if(APPLE)
  set(NEW_EXE_NAME "dnp3-outstation_darwin")
elseif(UNIX AND NOT APPLE)
  set(NEW_EXE_NAME "dnp3-outstation")
else()
  set(NEW_EXE_NAME "dnp3-outstation.exe")
endif()

add_custom_command(
  TARGET dnp3-outstation
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:dnp3-outstation>
          ${PROJECT_SOURCE_DIR}/bin/${NEW_EXE_NAME})
