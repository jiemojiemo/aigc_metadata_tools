//
// Created by user on 5/25/25.
//
#include <png.h>
#include <string>
#include <iostream>


// 写入带有自定义文本块的PNG
bool write_png_with_text(const std::string& input_png, const std::string& output_png,
                         const std::string& keyword, const std::string& data) {
    FILE* fp_in = fopen(input_png.c_str(), "rb");
    if (!fp_in) {
        std::cerr << "无法打开输入文件" << std::endl;
        return false;
    }

    // 检查PNG签名
    png_byte header[8];
    fread(header, 1, 8, fp_in);
    if (png_sig_cmp(header, 0, 8)) {
        std::cerr << "文件不是PNG格式" << std::endl;
        fclose(fp_in);
        return false;
    }

    // 初始化读取结构
    png_structp png_read_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_read_ptr) {
        fclose(fp_in);
        return false;
    }

    png_infop info_ptr = png_create_info_struct(png_read_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_read_ptr, NULL, NULL);
        fclose(fp_in);
        return false;
    }

    // 错误处理
    if (setjmp(png_jmpbuf(png_read_ptr))) {
        png_destroy_read_struct(&png_read_ptr, &info_ptr, NULL);
        fclose(fp_in);
        return false;
    }

    // 设置输入
    png_init_io(png_read_ptr, fp_in);
    png_set_sig_bytes(png_read_ptr, 8);

    // 读取图像信息
    png_read_info(png_read_ptr, info_ptr);

    png_uint_32 width, height;
    int bit_depth, color_type, interlace_type;

    png_get_IHDR(png_read_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                 &interlace_type, NULL, NULL);

    // 读取图像数据
    png_bytep* row_pointers = new png_bytep[height];
    for (png_uint_32 y = 0; y < height; y++) {
        row_pointers[y] = new png_byte[png_get_rowbytes(png_read_ptr, info_ptr)];
    }

    png_read_image(png_read_ptr, row_pointers);
    fclose(fp_in);

    // 创建写入结构
    FILE* fp_out = fopen(output_png.c_str(), "wb");
    if (!fp_out) {
        std::cerr << "无法创建输出文件" << std::endl;
        return false;
    }

    png_structp png_write_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_write_ptr) {
        fclose(fp_out);
        return false;
    }

    png_infop write_info_ptr = png_create_info_struct(png_write_ptr);
    if (!write_info_ptr) {
        png_destroy_write_struct(&png_write_ptr, NULL);
        fclose(fp_out);
        return false;
    }

    // 错误处理
    if (setjmp(png_jmpbuf(png_write_ptr))) {
        png_destroy_write_struct(&png_write_ptr, &write_info_ptr);
        fclose(fp_out);
        return false;
    }

    // 设置输出
    png_init_io(png_write_ptr, fp_out);

    // 添加文本块
    std::string text_data = data;
    png_text text_chunk;
    text_chunk.compression = PNG_TEXT_COMPRESSION_NONE;
    text_chunk.key = const_cast<char*>(keyword.c_str());
    text_chunk.text = const_cast<char*>(text_data.c_str());
    text_chunk.text_length = text_data.size();

    png_set_text(png_write_ptr, write_info_ptr, &text_chunk, 1);

    // 写入头部信息
    png_set_IHDR(png_write_ptr, write_info_ptr, width, height, bit_depth, color_type,
                 interlace_type, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png_write_ptr, write_info_ptr);

    // 写入数据
    png_write_image(png_write_ptr, row_pointers);
    png_write_end(png_write_ptr, NULL);

    // 清理
    for (png_uint_32 y = 0; y < height; y++) {
        delete[] row_pointers[y];
    }
    delete[] row_pointers;

    png_destroy_read_struct(&png_read_ptr, &info_ptr, NULL);
    png_destroy_write_struct(&png_write_ptr, &write_info_ptr);

    fclose(fp_out);
    return true;
}

// 读取文本块
std::string read_text_chunk(const std::string& png_path, const std::string& keyword) {
  FILE* fp = fopen(png_path.c_str(), "rb");
  if (!fp) {
    std::cerr << "无法打开文件" << std::endl;
    return "";
  }

  // 检查PNG签名
  png_byte header[8];
  fread(header, 1, 8, fp);
  if (png_sig_cmp(header, 0, 8)) {
    std::cerr << "文件不是PNG格式" << std::endl;
    fclose(fp);
    return "";
  }

  // 初始化读取结构
  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr) {
    fclose(fp);
    return "";
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    fclose(fp);
    return "";
  }

  // 错误处理
  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);
    return "";
  }

  // 设置输入
  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, 8);

  // 读取信息
  png_read_info(png_ptr, info_ptr);

  // 获取文本块
  png_textp text_ptr;
  int num_text = 0;

  if (png_get_text(png_ptr, info_ptr, &text_ptr, &num_text) > 0) {
    for (int i = 0; i < num_text; i++) {
      if (keyword == text_ptr[i].key) {
        std::string result = text_ptr[i].text;
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        return result;
      }
    }
  }

  // 清理
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  fclose(fp);
  return "";
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << "用法: " << argv[0] << " <输入PNG文件> <输出PNG文件>" << std::endl;
    return 1;
  }
  auto inputPngPath = argv[1];
  auto outputPngPath = argv[2];

  auto aigcText = R"({"Label": "value1", "ContentProducer": "value2", "ProduceID": "value3", "ReservedCode1": "value4", "ContentPropagator": "value5", "PropagateID": "value6", "ReservedCode2: "value7"})";
  write_png_with_text(inputPngPath, outputPngPath, "AIGC", aigcText);

  auto keyword = "AIGC";
  std::string text = read_text_chunk(outputPngPath, keyword);
  if (!text.empty()) {
    std::cout << "找到文本块: " << text << std::endl;
  } else {
    std::cout << "未找到指定的文本块" << std::endl;
  }
}