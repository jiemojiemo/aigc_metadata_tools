cmake_minimum_required(VERSION 3.16)

project(ImageAIGCMetaDataTest)
set(CMAKE_CXX_STANDARD 17)

set(CONAN_DISABLE_CHECK_COMPILER true)
find_package(PNG REQUIRED)
find_package(exiv2 REQUIRED)

add_subdirectory(demo)