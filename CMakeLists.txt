cmake_minimum_required(VERSION 3.31)
project(qrtest)

set(CMAKE_CXX_STANDARD 20)

add_executable(qrtest main.cpp)

target_link_libraries(qrtest PRIVATE qrencode png)

install(TARGETS qrtest
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})