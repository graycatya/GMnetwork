# cmake 版本
cmake_minimum_required( VERSION 3.0 )

# 项目名
project (cputest)

# 生成路径
set (CMAKE_BUILD_RPATH ${CMAKE_BINARY_DIR}/build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CAMKE_BINARY_DIR}/build/Lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/Bin)


# 查找当前目标下所有的源文件
aux_source_directory(. DIR_SRCS)

# 指定生成目录
add_executable(cputest ${DIR_SRCS})

