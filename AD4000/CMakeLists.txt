cmake_minimum_required(VERSION 3.5)

project(Software_AD4000 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Software_AD4000 main.cpp)

install(TARGETS Software_AD4000
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
