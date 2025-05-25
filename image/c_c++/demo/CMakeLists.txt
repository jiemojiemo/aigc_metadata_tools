add_executable(png_demo libpng_demo.cpp)
target_link_libraries(png_demo PNG::PNG)

add_executable(exiv2_demo exiv2_demo.cpp)
target_link_libraries(exiv2_demo exiv2::exiv2)
