//
// Created by user on 5/25/25.
//
#include <exiv2/exiv2.hpp>
#include <iostream>
int main(int argc, char *const argv[]) {
  try {
    Exiv2::XmpParser::initialize();
    ::atexit(Exiv2::XmpParser::terminate);

    if (argc != 2) {
      std::cout << "Usage: " << argv[0] << " file\n";
      return EXIT_FAILURE;
    }

    auto image = Exiv2::ImageFactory::open(argv[1]);
    image->readMetadata();
    Exiv2::ExifData &exifData = image->exifData();

    /*
      Exiv2 uses a CommentValue for Exif user comments. The format of the
      comment string includes an optional charset specification at the
      beginning:

      [charset=[Ascii|Jis|Unicode|Undefined]] comment

      Undefined is used as a default if the comment doesn't start with a charset
      definition.

      Following are a few examples of valid comments. The last one is written to
      the file.
     */
    std::string aigcText = R"({"AIGC":{"Label":"value1","ContentProducer":"value2","ProduceID":"value3","ReservedCode1":"value4","ContentPropagator":"value5","PropagateID":"value6","ReservedCode2: ":"value7"}} )";
    auto commentText = "charset=Undefined " + aigcText;

    // exifData["Exif.Photo.UserComment"] = "charset=Unicode A Unicode Exif comment added with Exiv2";
    // exifData["Exif.Photo.UserComment"] = "charset=Undefined An undefined Exif comment added with Exiv2";
    // exifData["Exif.Photo.UserComment"] = "Another undefined Exif comment added with Exiv2";
    exifData["Exif.Photo.UserComment"] = aigcText;

    std::cout << "Writing user comment '" << exifData["Exif.Photo.UserComment"]
              << "' back to the image\n";

    image->writeMetadata();
    return EXIT_SUCCESS;
  } catch (Exiv2::Error &e) {
    std::cout << "Caught Exiv2 exception '" << e << "'\n";
    return EXIT_FAILURE;
  }
}