include_directories(${CMAKE_SOURCE_DIR}/include)

set(INCLUDE_HEADERS_SERVER ${CMAKE_SOURCE_DIR}/include/Server)
set(INCLUDE_HEADERS_DATABASE ${CMAKE_SOURCE_DIR}/include/DataBase)
message(STATUS "Include Headers Server: ${INCLUDE_HEADERS_SERVER}")
message(STATUS "Include Headers Database: ${INCLUDE_HEADERS_DATABASE}")

find_package(YAML-CPP REQUIRED)
find_package(lz4 REQUIRED) 
set(HEADERS 
    ${INCLUDE_HEADERS_SERVER}/Server.h
    ${INCLUDE_HEADERS_SERVER}/RpcServer.h
    ${INCLUDE_HEADERS_SERVER}/CServer.h
    ${INCLUDE_HEADERS_SERVER}/HttpConnection.h
    ${INCLUDE_HEADERS_DATABASE}/ChatDataBase.h
    ${INCLUDE_HEADERS_SERVER}/LogicSystem.h
    ${INCLUDE_HEADERS_SERVER}/VarifyGrpcClient.h
    ${CMAKE_SOURCE_DIR}/include/message.grpc.pb.cc
    ${CMAKE_SOURCE_DIR}/include/message.pb.cc
    ${CMAKE_SOURCE_DIR}/include/message.grpc.pb.h
    ${CMAKE_SOURCE_DIR}/include/message.pb.h
)

set(PROJECT_SOURCES 
    ${HEADERS}
    RedisMgr.cpp
    ChatDataBase.cpp
    CServer.cpp
    main.cpp
    RpcServer.cpp
    HttpConnection.cpp
    LogicSystem.cpp
    yamlReader.hpp
    AsioIOServicePool.cpp
    MysqlMgr.cpp
)

set(LINK_LIBS 
    SQLiteCpp
    spdlog::spdlog
    rpclib::rpc
    yaml-cpp::yaml-cpp
    JsonCpp::JsonCpp
    gRPC::gpr
    gRPC::grpc
    gRPC::grpc++
    gRPC::grpc++_alts
    redis++::redis++_static
    unofficial::mysql-connector-cpp::connector
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Gui
    /lib/x86_64-linux-gnu/libzstd.so
    resolv
    lz4::lz4
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ChatServer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(ChatServer
        ${PROJECT_SOURCES}
    )
endif()

target_link_libraries(ChatServer PRIVATE ${LINK_LIBS})

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