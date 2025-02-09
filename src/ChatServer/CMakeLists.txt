include_directories(${CMAKE_SOURCE_DIR}/include)

set(INCLUDE_HEADERS_SERVER ${CMAKE_SOURCE_DIR}/include/Server)
set(INCLUDE_HEADERS_DATABASE ${CMAKE_SOURCE_DIR}/include/DataBase)
message(STATUS "Include Headers Server: ${INCLUDE_HEADERS_SERVER}")

set(PROJECT_SOURCES 
    # Place your source files here
    ${INCLUDE_HEADERS_SERVER}/Server.h
    ${INCLUDE_HEADERS_SERVER}/RpcServer.h
    ${INCLUDE_HEADERS_SERVER}/HttpServer.h
    ${INCLUDE_HEADERS_DATABASE}/DataBaseClient.h
    ../ChatClient/DataBaseClient.cpp
    main.cpp
    RpcServer.cpp
    HttpServer.cpp
)

set(LINK_LIBS 
SQLiteCpp
spdlog::spdlog
Boost::system
Boost::thread
rpclib::rpc
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

# 链接Qt6的Widgets和Network库
target_link_libraries(ChatServer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Gui)

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