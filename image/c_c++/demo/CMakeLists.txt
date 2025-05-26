add_executable(png_demo libpng_demo.cpp)
target_link_libraries(png_demo PNG::PNG)

add_executable(exiv2_demo exiv2_demo.cpp)
target_link_libraries(exiv2_demo exiv2::exiv2)

add_executable(libexif_read_demo libexif_read_demo.cpp)
target_link_libraries(libexif_read_demo libexif::libexif)

add_executable(libexif_libjpeg_write_demo libexif_libjpeg_write_demo.cpp libjpeg_for_libexif/jpeg-data.c libjpeg_for_libexif/jpeg-marker.c)
target_link_libraries(libexif_libjpeg_write_demo JPEG::JPEG libexif::libexif)