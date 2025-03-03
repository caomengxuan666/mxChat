include_directories(${CMAKE_SOURCE_DIR}/include)

set(INCLUDE_HEADERS_SERVER ${CMAKE_SOURCE_DIR}/include/Server)
set(INCLUDE_HEADERS_DATABASE ${CMAKE_SOURCE_DIR}/include/DataBase)
set(CHAT_SERVER_PROJECT_SOURCES
    ../ChatServer/RedisMgr.cpp
    ../ChatServer/CServer.cpp
    ../ChatServer/HttpConnection.cpp
    ../ChatServer/LogicSystem.cpp
    ../ChatServer/yamlReader.hpp
    ../ChatServer/AsioIOServicePool.cpp
    ../ChatServer/MysqlMgr.cpp
    ../ChatServer/CSession.cpp
    ../ChatServer/UserMgr.cpp
)
message(STATUS "Include Headers Server: ${INCLUDE_HEADERS_SERVER}")
message(STATUS "Include Headers Database: ${INCLUDE_HEADERS_DATABASE}")

find_package(YAML-CPP REQUIRED)
find_package(lz4 REQUIRED) 

set(HEADERS 
    ${INCLUDE_HEADERS_SERVER}/CServer.h
    ${INCLUDE_HEADERS_SERVER}/HttpConnection.h
    ${INCLUDE_HEADERS_SERVER}/LogicSystem.h
    ${INCLUDE_HEADERS_SERVER}/VarifyGrpcClient.h
    ${CMAKE_SOURCE_DIR}/include/message.grpc.pb.cc
    ${CMAKE_SOURCE_DIR}/include/message.pb.cc
    ${CMAKE_SOURCE_DIR}/include/message.grpc.pb.h
    ${CMAKE_SOURCE_DIR}/include/message.pb.h
)

set(GATE_SERVER_PROJECT_SOURCES 
    ${HEADERS}
    Gate.cpp
    GateServer.cpp
    GateLogic.cpp
    ${CHAT_SERVER_PROJECT_SOURCES}
)


set(LINK_LIBS_CHAT_SERVER 
    SQLiteCpp
    spdlog::spdlog
    yaml-cpp::yaml-cpp
    JsonCpp::JsonCpp
    gRPC::gpr
    gRPC::grpc
    gRPC::grpc++
    gRPC::grpc++_alts
    redis++::redis++_static
    unofficial::mysql-connector-cpp::connector
    nameof::nameof
    /lib/x86_64-linux-gnu/libzstd.so
    resolv
    lz4::lz4
)

add_executable(GateServer
${GATE_SERVER_PROJECT_SOURCES}
)

target_link_libraries(GateServer PRIVATE ${LINK_LIBS_CHAT_SERVER})


# 安装规则
install(TARGETS GateServer DESTINATION Bin)