# cmake 版本
cmake_minimum_required( VERSION 3.0 )

# 项目名
project (StressteSting)

add_definitions("-std=c++11")

# 生成路径
set (CMAKE_BUILD_RPATH ${CMAKE_BINARY_DIR}/build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CAMKE_BINARY_DIR}/build/Lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/Bin)

# 库路径
set(INC_DIR ${PROJECT_SOURCE_DIR}/../../../SrcLib/catepoll/C++Catepoll
            ${PROJECT_SOURCE_DIR}/../../../SrcLib/Network/C++Catnet
            ${PROJECT_SOURCE_DIR}/../../../SrcLib/Threadpool/C++Catthreadpool)

set(LINK_DIR ${PROJECT_SOURCE_DIR}/../../../SrcLib/catepoll/C++Catepoll
            ${PROJECT_SOURCE_DIR}/../../../SrcLib/Network/C++Catnet
            ${PROJECT_SOURCE_DIR}/../../../SrcLib/Threadpool/C++Catthreadpool)



include_directories(${INC_DIR})
link_directories(${LINK_DIR})
link_libraries(CatEpoll+)
link_libraries(Catnetwork+)
link_libraries(Catthreadpool+)

# 查找当前目标下所有的源文件
aux_source_directory(. DIR_SRCS)

# 指定生成目录
add_executable(StressteSting ${DIR_SRCS})
target_link_libraries(StressteSting CatEpoll+)
target_link_libraries(StressteSting Catnetwork+)
target_link_libraries(StressteSting Catthreadpool+)

