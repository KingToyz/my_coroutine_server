cmake_minimum_required(VERSION 3.20)
# include(FetchContent)

project(server)
# 当前使用的c++编译器
message("-- CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER_ID}")
# 当前使用的c编译器
message("-- CMAKE_C_COMPILER: ${CMAKE_C_COMPILER_ID}")
# 当前使用的c编译器的版本
message("-- CMAKE_C_COMPILER_VERSION: ${CMAKE_C_COMPILER_VERSION}")
# 当前使用的c++编译器的版本
message("-- CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")

set (CMAKE_CXX_FLAGS "-Werror")

set (CMAKE_CXX_FLAGS "-g")

set (CMAKE_CXX_FLAGS "-fcoroutines")


add_subdirectory(jsoncpp)
include_directories(jsoncpp/include)
add_library(jsoncpp STATIC jsoncpp/src/lib_json/json_reader.cpp jsoncpp/src/lib_json/json_value.cpp jsoncpp/src/lib_json/json_writer.cpp)

# FetchContent_Declare(llhttp
#   URL "https://github.com/nodejs/llhttp/archive/refs/tags/release/v8.1.0.tar.gz")

# set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
# set(BUILD_STATIC_LIBS ON CACHE INTERNAL "")
# FetchContent_MakeAvailable(llhttp)

find_library(HTTP_PARSER_LIB http_parser PATHS ./lib NO_DEFAULT_PATH)
if(NOT HTTP_PARSER_LIB)
    message(FATAL_ERROR "Cannot find http_parser library")
endif()

add_executable(server message.cpp parse.cpp socket.cpp socket_accept_operation.cpp socket_recv_operation.cpp socket_write_operation.cpp epoll_agent.cpp main.cpp)   
# Link with the llhttp_static target
target_link_libraries(server ${HTTP_PARSER_LIB} jsoncpp)