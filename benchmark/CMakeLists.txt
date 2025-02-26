include_directories(${CMAKE_SOURCE_DIR}/include)


set(INCLUDE_HEADERS_CLIENT ${CMAKE_SOURCE_DIR}/include/Client)
set(INCLUDE_HEADERS_DATABASE ${CMAKE_SOURCE_DIR}/include/DataBase)

set(PROJECT_SOURCES 
    # Place your source files here
    ${INCLUDE_HEADERS_CLIENT}/../Singleton.h
    ${INCLUDE_HEADERS_CLIENT}/HttpMgr.h
)

set(BENCHMARK_SOURCE
    ${PROJECT_SOURCES}
    benchmark4StatusServer.cc
    /home/cmx/QtProjects/mxChat/src/ChatClient/HttpMgr.cpp
)

set(LINK_LIBS
    spdlog::spdlog
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Concurrent 
)



add_executable(benchmark4StatusServer ${BENCHMARK_SOURCE})
target_link_libraries(benchmark4StatusServer PRIVATE ${LINK_LIBS})
