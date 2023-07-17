add_executable(dnp3poll ./main.cpp)
target_link_libraries(dnp3poll opendnp3 ser4cpp exe4cpp asio)
target_link_options(dnp3poll PRIVATE -static-libgcc -static-libstdc++ -static)
