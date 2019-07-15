# cmake 版本
cmake_minimum_required( VERSION 2.8 )
# 项目名
project (tcptest)

# 生成路径
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CAMKE_BINARY_DIR}/build/Lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/Bin)

option (_WINDOWSSYS_ OFF)

SET(CMAKE_BUILD_TYPE "Release")
add_definitions("-Wall -g")
add_definitions("-std=c++11")


add_definitions(-D _WINDOWSSYS_)    


#include_directories("${PROJECT_SOURCE_DIR}/../../SrcLib/Network")
#add_subdirectory(${PROJECT_SOURCE_DIR}/../../SrcLib/Network Network.out)
#set (EXTRA_LIBS0 ${EXTRA_LIBS0} Catnetwork)


# 查找当前目标下所有的源文件
aux_source_directory(. DIR_SRCS)

# 指定生成目录
add_executable(tcptest ${DIR_SRCS})
target_link_libraries(tcptest ws2_32)
target_link_libraries(tcptest wsock32) 
