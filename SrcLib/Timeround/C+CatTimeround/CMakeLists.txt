cmake_minimum_required(VERSION 3.10)

option (_WINDOWSSYS_ OFF)
option (_SOORDLL_ OFF)

if(_WINDOWSSYS_)
    add_definitions(-D _WINDOWSSYS_)    
    link_libraries(ws2_32)
    link_libraries(wsock32)
endif(_WINDOWSSYS_)

add_definitions("-Wall -g")


# 将名称保存到 DIR_LIB_SRCS 变量
aux_source_directory(. DIR_LIB_SRCS)

# 指定生成 CatTimeround 链接库
if(_SOORDLL_)
    add_library(CatTimeround+ SHARED ${DIR_LIB_SRCS})
else()
    add_library(CatTimeround+ ${DIR_LIB_SRCS}) 
endif(_SOORDLL_)

if(_WINDOWSSYS_)
    target_link_libraries(CatTimeround+ ws2_32)
    target_link_libraries(CatTimeround+ wsock32) 
endif(_WINDOWSSYS_)
