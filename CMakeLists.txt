cmake_minimum_required(VERSION 3.14)
project(mxChat VERSION 0.1 LANGUAGES CXX)
set(CMAKE_TOOLCHAIN_FILE "/home/cmx/vcpkg/scripts/buildsystems/vcpkg.cmake")
set(CMAKE_PREFIX_PATH
    "/home/cmx/vcpkg/installed/x64-linux"
    "/home/cmx/Qt/6.8.1/gcc_64"
    CACHE PATH "Additional directories to be searched for packages"
)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
 
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

# 设置安装前缀
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install)

# 添加网络模块
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core Gui Network Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core Gui Network Concurrent)
find_package(SQLiteCpp CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

# boost
find_package(Boost REQUIRED system regex thread)

# jsoncpp
find_package(jsoncpp CONFIG REQUIRED)

# grpc
find_package(gRPC CONFIG REQUIRED)

# redis
find_package(redis++ CONFIG REQUIRED)

# mysql connector
find_package(unofficial-mysql-connector-cpp CONFIG REQUIRED)



function(copy_config_files)
    file(COPY "config/" DESTINATION ${CMAKE_BINARY_DIR}/config)
    message(STATUS "from ${CMAKE_SOURCE_DIR}/config Copy config/ to ${CMAKE_BINARY_DIR}/config")
endfunction()


function(copy_resources)
    file(COPY "static/" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/static)
    file(COPY "templates/" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/templates)
    message(STATUS "from ${CMAKE_SOURCE_DIR}/static to ${CMAKE_SOURCE_DIR}/templates")
endfunction()

copy_resources()

copy_config_files()


add_subdirectory(src)

add_subdirectory(benchmark)


# 安装静态文件
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/static DESTINATION Bin/static)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/templates DESTINATION Bin/templates)
install(DIRECTORY ${CMAKE_BINARY_DIR}/config DESTINATION Bin/config)

# 在安装阶段执行 pack.sh 脚本
install(CODE "
    execute_process(
        COMMAND bash \"${CMAKE_CURRENT_SOURCE_DIR}/pack.sh\"
        WORKING_DIRECTORY \"${CMAKE_CURRENT_SOURCE_DIR}\"
        RESULT_VARIABLE result
    )
    if(result)
        message(FATAL_ERROR \"pack.sh failed with result: \${result}\")
    else()
        message(STATUS \"pack.sh executed successfully\")
    endif()
")