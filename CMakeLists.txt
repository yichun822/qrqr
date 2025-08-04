cmake_minimum_required(VERSION 3.31)
project(qrqr)

set(CMAKE_CXX_STANDARD 20)

add_executable(qrqr main.cpp)

target_link_libraries(qrqr PRIVATE qrencode png)

install(TARGETS qrqr
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})