# cmake 版本
cmake_minimum_required( VERSION 2.8 )
# 项目名
project (plant)

# 生成路径
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CAMKE_BINARY_DIR}/build/Lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/Bin)


SET(CMAKE_BUILD_TYPE "Release")

add_definitions("-Wall -g")

# 查找当前目标下所有的源文件
aux_source_directory(. DIR_SRCS)

# 指定生成目录
add_executable(plant ${DIR_SRCS})