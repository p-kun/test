# CMakeLists.txt

set(ZLIB_LIBRARY "zlibd")
set(ZLIB_INCLUDE_DIR "./zlib")

set(BUILD_STATIC ON)
set(BUILD_EXECUTABLES OFF)
set(BUILD_TESTS OFF)

set(CMAKE_C_FLAGS "-D_CRT_SECURE_NO_WARNINGS")

link_directories("C:/Users/rsp95/WORK/test/test_libjpeg/build/zlib/Debug")
link_directories("C:/Users/rsp95/WORK/test/test_libjpeg/build/zlib/Release")
link_directories("C:/Users/rsp95/WORK/test/test_libjpeg/build/libjpeg/Debug")
link_directories("C:/Users/rsp95/WORK/test/test_libjpeg/build/libjpeg/Release")
link_directories("C:/Users/rsp95/WORK/test/test_libjpeg/build/libpng/Debug")
link_directories("C:/Users/rsp95/WORK/test/test_libjpeg/build/libpng/Release")
include_directories("./build/zlib" "./build/libpng" "./zlib" "./libjpeg" "./libpng" "./build/libjpeg")

add_executable(Main main.c)

add_subdirectory(zlib)
add_subdirectory(libpng)
add_subdirectory(libjpeg)

target_link_libraries(Main libpng16_staticd jpeg zlibstaticd)
