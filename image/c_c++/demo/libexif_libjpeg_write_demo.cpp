//
// Created by user on 5/26/25.
//
extern "C"{
#include "libjpeg_for_libexif/jpeg-data.h"
#include "libjpeg_for_libexif/jpeg-marker.h"
}

#include <cassert>
#include <stdio.h>
#include <string.h>
#include <string>

/* comment to write into the EXIF block */
// #define FILE_COMMENT "{"AIGC":{"Label":"value1","ContentProducer":"value2","ProduceID":"value3","ReservedCode1":"value4","ContentPropagator":"value5","PropagateID":"value6","ReservedCode2: ":"value7"}}"
#define FILE_COMMENT "libexif demonstration image"

/* special header required for EXIF_TAG_USER_COMMENT */
#define ASCII_COMMENT "ASCII\0\0\0"

static ExifEntry *create_tag(ExifData *exif, ExifIfd ifd, ExifTag tag, size_t len)
{
  void *buf;
  ExifEntry *entry;

  /* Create a memory allocator to manage this ExifEntry */
  ExifMem *mem = exif_mem_new_default();
  assert(mem != NULL); /* catch an out of memory condition */

  /* Create a new ExifEntry using our allocator */
  entry = exif_entry_new_mem (mem);
  assert(entry != NULL);

  /* Allocate memory to use for holding the tag data */
  buf = exif_mem_alloc(mem, len);
  assert(buf != NULL);

  /* Fill in the entry */
  entry->data = (unsigned char*)buf;
  entry->size = len;
  entry->tag = tag;
  entry->components = len;
  entry->format = EXIF_FORMAT_UNDEFINED;

  /* Attach the ExifEntry to an IFD */
  exif_content_add_entry (exif->ifd[ifd], entry);

  /* The ExifMem and ExifEntry are now owned elsewhere */
  exif_mem_unref(mem);
  exif_entry_unref(entry);

  return entry;
}

int main(int argc, char* argv[])
{
  if (argc < 3) {
    printf("Usage: %s <input.jpg> <output.jpg>\n", argv[0]);
    return 1;
  }

  JPEGData* jpegData = jpeg_data_new_from_file(argv[1]);
  if (!jpegData) {
    printf("open JPEG file %s failed\n", argv[1]);
    return -1;
  }

  ExifData* exif = jpeg_data_get_exif_data(jpegData);
  if (!exif) {
    printf("no exif data, let's create new one");
    exif = exif_data_new();
    exif_data_set_option(exif, EXIF_DATA_OPTION_FOLLOW_SPECIFICATION);
    exif_data_set_data_type(exif, EXIF_DATA_TYPE_COMPRESSED);
    exif_data_set_byte_order(exif, EXIF_BYTE_ORDER_INTEL);
  }

  std::string aigcText = R"({"AIGC":{"Label":"value1","ContentProducer":"value2","ProduceID":"value3","ReservedCode1":"value4","ContentPropagator":"value5","PropagateID":"value6","ReservedCode2: ":"value7"}} )";

  ExifEntry* entry = create_tag(exif, EXIF_IFD_EXIF, EXIF_TAG_USER_COMMENT, sizeof(ASCII_COMMENT) + aigcText.length() - 1);
  /* Write the special header needed for a comment tag */
  memcpy(entry->data, ASCII_COMMENT, sizeof(ASCII_COMMENT)-1);
  /* Write the actual comment text, without the trailing NUL character */
  memcpy(entry->data+8, aigcText.c_str(), aigcText.length());
  /* create_tag() happens to set the format and components correctly for
   * EXIF_TAG_USER_COMMENT, so there is nothing more to do. */

  jpeg_data_set_exif_data(jpegData, exif);

  if (jpeg_data_save_file(jpegData, argv[2]) == 0) {
    printf("jpeg_data_save_file error!\n");
    return -1;
  }

  return 0;
}