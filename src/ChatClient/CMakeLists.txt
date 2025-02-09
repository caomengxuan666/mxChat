set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(INCLUDE_HEADERS_CLIENT ${CMAKE_SOURCE_DIR}/include/Client)
set(INCLUDE_HEADERS_DATABASE ${CMAKE_SOURCE_DIR}/include/DataBase)
set(PROJECT_SOURCES 
    # Place your source files here
    ${INCLUDE_HEADERS_CLIENT}/Login.h
    ${INCLUDE_HEADERS_CLIENT}/MainWidget.h
    ${INCLUDE_HEADERS_CLIENT}/SessionItem.hpp
    ${INCLUDE_HEADERS_DATABASE}/DataBaseClient.h
    ${INCLUDE_HEADERS_CLIENT}/Client.h
    ${INCLUDE_HEADERS_CLIENT}/RpcClient.h
    ${INCLUDE_HEADERS_CLIENT}/MessageWidget.h
    RpcClient.cpp
    DataBaseClient.cpp
    main.cpp
    Login.cpp
    MainWidget.cpp
    MessageWidget.cpp
    ${CMAKE_SOURCE_DIR}/resources/Login.qrc
)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ChatClient
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(ChatClient
        ${PROJECT_SOURCES}
    )
endif()

#spdlog
set(LINK_LIBS
SQLiteCpp
spdlog::spdlog
Boost::system
Boost::thread
rpclib::rpc
)

# 链接Qt6的Widgets和Network库
target_link_libraries(ChatClient PRIVATE
Qt${QT_VERSION_MAJOR}::Widgets
Qt${QT_VERSION_MAJOR}::Network 
Qt${QT_VERSION_MAJOR}::Gui
)

target_link_libraries(ChatClient PRIVATE ${LINK_LIBS})

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.FileTransAssistant)
endif()
set_target_properties(ChatClient PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ChatClient)
endif()