cmake_minimum_required(VERSION 3.16)

project(PNGAIGCTest)
set(CMAKE_CXX_STANDARD 17)

set(CONAN_DISABLE_CHECK_COMPILER true)
find_package(PNG REQUIRED)

include_directories(${PNG_INCLUDE_DIRS})

add_executable(main main.cpp)
target_link_libraries(main PNG::PNG)