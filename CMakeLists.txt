cmake_minimum_required(VERSION 3.14)
project(mxChat VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 添加网络模块
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core Gui Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core Gui Network)
find_package(SQLiteCpp CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

# boost
find_package(Boost REQUIRED system regex thread)

# rpclib
find_package(rpclib REQUIRED)

# jsoncpp
find_package(jsoncpp CONFIG REQUIRED)

# grpc
find_package(gRPC CONFIG REQUIRED)

add_subdirectory(src)
