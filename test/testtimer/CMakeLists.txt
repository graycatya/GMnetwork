# cmake 版本
cmake_minimum_required( VERSION 2.8 )
# 项目名
project (test)

# 生成路径
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CAMKE_BINARY_DIR}/build/Lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/Bin)

option (_WINDOWSSYS_ OFF)

#SET(CMAKE_BUILD_TYPE "Release")

add_definitions("-Wall -g")

if(_WINDOWSSYS_)
    add_definitions(-D _WINDOWSSYS_)    
endif(_WINDOWSSYS_)

# 库路径
set(INC_DIR ${PROJECT_SOURCE_DIR}/../../SrcLib/Timeround)
set(LINK_DIR ${PROJECT_SOURCE_DIR}/../../SrcLib/Timeround)

include_directories(${INC_DIR})
link_directories(${LINK_DIR})
link_libraries(CatTimeround)
#link_libraries(Catnetwork)

#link_libraries(pthread)


# 查找当前目标下所有的源文件
aux_source_directory(. DIR_SRCS)

# 指定生成目录
add_executable(test ${DIR_SRCS})
target_link_libraries(test CatTimeround)
#target_link_libraries(test catepoll)
#target_link_libraries(test Catnetwork)

