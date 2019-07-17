# cmake 版本
cmake_minimum_required( VERSION 3.1 )
# 项目名
project (libevent)

link_libraries(event)
# 查找当前目标下所有的源文件
aux_source_directory(. DIR_SRCS)

# 指定生成目录
add_executable(libevent ${DIR_SRCS})