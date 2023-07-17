add_executable(dnp3server ./main.cpp)
target_link_libraries(dnp3server opendnp3 ser4cpp exe4cpp asio)
target_link_options(dnp3server PRIVATE -static-libgcc -static-libstdc++ -static)