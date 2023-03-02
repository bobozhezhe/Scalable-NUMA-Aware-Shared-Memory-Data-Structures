cmake_minimum_required(VERSION 3.23)
project(untitled1)

# 添加 NUMA 库和头文件
find_library(NUMA_LIBRARY numa)

# 添加源代码
add_executable(untitled1 main.cpp)

# 链接 NUMA 库
target_link_libraries(untitled1 PRIVATE ${NUMA_LIBRARY})

# 添加 NUMA 头文件路径
target_include_directories(untitled1 PRIVATE ${NUMA_INCLUDE_DIRS})