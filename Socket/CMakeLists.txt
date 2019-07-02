option (_WINDOWSSYS_ OFF)


if(_WINDOWSSYS_)
    add_definitions(-D _WINDOWSSYS_)    
    link_libraries(ws2_32)
    link_libraries(wsock32)
endif(_WINDOWSSYS_)

# 将名称保存到 DIR_LIB_SRCS 变量
aux_source_directory(. DIR_LIB_SRCS)

# 指定生成 Catnetwork 链接库
add_library(Catnetwork ${DIR_LIB_SRCS})
if(_WINDOWSSYS_)
    target_link_libraries(Catnetwork ws2_32)
    target_link_libraries(Catnetwork wsock32)
else
    
endif(_WINDOWSSYS_)
