include_directories(${CMAKE_SOURCE_DIR}/include)

set(INCLUDE_HEADERS_SERVER ${CMAKE_SOURCE_DIR}/include/Server)
set(INCLUDE_HEADERS_DATABASE ${CMAKE_SOURCE_DIR}/include/DataBase)
message(STATUS "Include Headers Server: ${INCLUDE_HEADERS_SERVER}")
message(STATUS "Include Headers Database: ${INCLUDE_HEADERS_DATABASE}")

find_package(YAML-CPP REQUIRED)
find_package(lz4 REQUIRED) 

set(HEADERS 
    ${INCLUDE_HEADERS_SERVER}/CServer.h
    ${INCLUDE_HEADERS_SERVER}/HttpConnection.h
    ${INCLUDE_HEADERS_SERVER}/LogicSystem.h
    ${INCLUDE_HEADERS_SERVER}/GateLogic.h
    ${INCLUDE_HEADERS_SERVER}/VarifyGrpcClient.h
    ${CMAKE_SOURCE_DIR}/include/message.grpc.pb.cc
    ${CMAKE_SOURCE_DIR}/include/message.pb.cc
    ${CMAKE_SOURCE_DIR}/include/message.grpc.pb.h
    ${CMAKE_SOURCE_DIR}/include/message.pb.h
)

set(CHAT_SERVER_PROJECT_SOURCES 
    ${HEADERS}
    RedisMgr.cpp
    CServer.cpp
    ChatServer.cpp
    HttpConnection.cpp
    LogicSystem.cpp
    yamlReader.hpp
    AsioIOServicePool.cpp
    MysqlMgr.cpp
    CSession.cpp
    UserMgr.cpp
    ../GateServer/GateLogic.cpp
)

set(STATUS_SERVER_PROJECT_SOURCES
    ${CMAKE_SOURCE_DIR}/include/message.grpc.pb.cc
    ${CMAKE_SOURCE_DIR}/include/message.pb.cc
    StatusServer.cpp
    StatusServiceImpl.cpp
    StatusMonitor.cpp
    RedisMgr.cpp
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

set(LINK_LIBS_STATUS_SERVER
    spdlog::spdlog
    yaml-cpp::yaml-cpp
    gRPC::gpr
    gRPC::grpc
    gRPC::grpc++
    gRPC::grpc++_alts
    redis++::redis++_static
    nameof::nameof
)


add_executable(ChatServer
    ${CHAT_SERVER_PROJECT_SOURCES}
)

add_executable(StatusServer
    ${STATUS_SERVER_PROJECT_SOURCES}
)

target_link_libraries(ChatServer PRIVATE ${LINK_LIBS_CHAT_SERVER})
target_link_libraries(StatusServer PRIVATE ${LINK_LIBS_STATUS_SERVER})

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.FileTransAssistant)
endif()
set_target_properties(ChatServer PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ChatServer)
endif()

# 安装规则
install(TARGETS ChatServer DESTINATION Bin)

install(TARGETS StatusServer DESTINATION Bin)