# 使用 file(GLOB) 自动包含所有 .cpp, .h, .hpp 文件
file(GLOB SRC_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp" "${CMAKE_SOURCE_DIR}/include/*.cc")
file(GLOB SRC_COMPLEMENTS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

find_package(Crow)
find_package(yaml-cpp CONFIG REQUIRED)

set(EXECUTABLE_NAME webserver)

set(SRC_COMPLEMENTS ${SRC_COMPLEMENTS} ${CMAKE_SOURCE_DIR}/src/ChatServer/RedisMgr.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)

# 添加可执行文件
add_executable(${EXECUTABLE_NAME} ${SRC_HEADERS} ${SRC_COMPLEMENTS})

# 链接所需的库
# 链接顺序非常讲究，不能乱改
target_link_libraries(${EXECUTABLE_NAME} PRIVATE 
    spdlog::spdlog
    yaml-cpp::yaml-cpp
    redis++::redis++_static
    gRPC::gpr
    gRPC::grpc
    gRPC::grpc++
    gRPC::grpc++_alts
    unofficial::mysql-connector-cpp::connector
    /lib/x86_64-linux-gnu/libzstd.so
    resolv
    lz4::lz4
)

# 包含头文件目录
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# 安装规则
install(TARGETS webserver DESTINATION Bin)

