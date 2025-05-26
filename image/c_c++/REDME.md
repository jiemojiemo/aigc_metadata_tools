# 编译
项目依赖通过 [conan](https://conan.io/downloads) 安装，开始编译前请安装好 conan（>=2.0）
```bash
mkdir build
conan install . --output-folder=build --build=missing --settings=build_type=Debug
cmake -S . -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build ./build
```