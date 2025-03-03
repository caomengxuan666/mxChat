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
    ${INCLUDE_HEADERS_CLIENT}/Client.h
    ${INCLUDE_HEADERS_CLIENT}/Regist.ui
    ${INCLUDE_HEADERS_CLIENT}/Regist.h
    ${INCLUDE_HEADERS_CLIENT}/../Singleton.h
    ${INCLUDE_HEADERS_CLIENT}/HttpMgr.h
    ${INCLUDE_HEADERS_CLIENT}/BubbleWidget.h
    ${INCLUDE_HEADERS_CLIENT}/TcpMgr.h
    ${INCLUDE_HEADERS_CLIENT}/UserMgr.h
    Regist.cpp
    main.cpp
    Login.cpp
    MainWidget.cpp
    HttpMgr.cpp
    BubbleWidget.cpp
    TcpMgr.cpp
    UserMgr.cpp
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
    redis++::redis++_static
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network 
    Qt${QT_VERSION_MAJOR}::Gui
    nameof::nameof
)

target_link_libraries(ChatClient PRIVATE ${LINK_LIBS})


if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ChatClient)
endif()

install(TARGETS ChatClient DESTINATION Bin)