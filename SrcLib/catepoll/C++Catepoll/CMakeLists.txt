cmake_minimum_required(VERSION 3.10)

option (_WINDOWSSYS_ OFF)
option (_SOORDLL_ OFF)

if(_WINDOWSSYS_)
    add_definitions(-D _WINDOWSSYS_)    
    link_libraries(ws2_32)
    link_libraries(wsock32)
endif(_WINDOWSSYS_)

add_definitions("-Wall -g")

# 库路径
set(INC_DIR ${PROJECT_SOURCE_DIR}/../../Network/C+Catnet)
set(LINK_DIR ${PROJECT_SOURCE_DIR}/../../Network/C+Catnet)

# 将名称保存到 DIR_LIB_SRCS 变量
aux_source_directory(. DIR_LIB_SRCS)

include_directories(${INC_DIR})
link_directories(${LINK_DIR})
link_libraries(Catnetwork)


if(_SOORDLL_)
    add_library(CatEpoll+ SHARED ${DIR_LIB_SRCS})
else()
    add_library(CatEpoll+ ${DIR_LIB_SRCS})
endif(_SOORDLL_)
target_link_libraries(CatEpoll+ Catnetwork)

if(_WINDOWSSYS_)
    target_link_libraries(CatEpoll+ ws2_32)
    target_link_libraries(CatEpoll+ wsock32) 
endif(_WINDOWSSYS_)
