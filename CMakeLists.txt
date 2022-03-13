cmake_minimum_required(VERSION 2.8.12)
project(tdsql)

macro(use_cxx17)
    if (CMAKE_VERSION VERSION_LESS "3.1")
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
        endif ()
    else ()
        set (CMAKE_CXX_STANDARD 17)
    endif ()
endmacro(use_cxx17)

macro(use_cxx11)
    if (CMAKE_VERSION VERSION_LESS "3.1")
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
        endif ()
    else ()
        set (CMAKE_CXX_STANDARD 11)
    endif ()
endmacro(use_cxx11)

use_cxx17()

include_directories(include)

set(Boost_INCLUDE_DIRS thirdparty/boost/include)

set(MYSQLCLIENT_INCLUDE_DIR thirdparty/mysqlclient/include)

include_directories(${Boost_INCLUDE_DIRS} thirdparty/mysql-connector-cpp/include)

find_library(MYSQL_LIBRARIES NAMES mysqlcppconn PATHS thirdparty/mysql-connector-cpp/lib/ NO_DEFAULT_PATH)

find_library(MYSQL_CLIENT_LIBRARIES NAMES mysqlclient PATHS thirdparty/mysqlclient/lib NO_DEFAULT_PATH)

find_library(SSL NAMES ssl PATHS thirdparty/openssl/lib NO_DEFAULT_PATH)
find_library(CRYPTO NAMES crypto PATHS thirdparty/openssl/lib NO_DEFAULT_PATH)

set(MYSQL_REQ_LIBRARIES ${MYSQL_CLIENT_LIBRARIES} ${MYSQL_LIBRARIES} pthread ${SSL} ${CRYPTO} dl z resolv)

#set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/mysql-connector-cpp/;${CMAKE_MODULE_PATH})
#find_package(MySQL REQUIRED)
#include_directories(${MYSQL_INCLUDE_DIR})

include_directories(${MYSQLCLIENT_INCLUDE_DIR})

add_executable(run
        src/main.cpp
        src/config.cpp
        src/descriptor.cpp
        src/logger.cpp
        src/module.cpp
        src/sqlconn.cpp
        src/binfetch.cpp
        src/event.cpp
        src/underlying/event_internal.cpp
        src/underlying/byte_op.cpp
        src/binbuffer.cpp
        src/table_map_event_metadata.cpp
        )

find_library(BOOST_FILESYSTEM NAMES boost_filesystem PATHS thirdparty/boost/lib/ NO_DEFAULT_PATH)
find_library(BOOST_SYSTEM NAMES boost_system PATHS thirdparty/boost/lib/ NO_DEFAULT_PATH)
find_library(BOOST_SERIALIZATION NAMES boost_serialization PATHS thirdparty/boost/lib/ NO_DEFAULT_PATH)
find_library(BOOST_THREAD NAMES boost_thread PATHS thirdparty/boost/lib/ NO_DEFAULT_PATH)
find_library(BOOST_CHRONO NAMES boost_chrono PATHS thirdparty/boost/lib/ NO_DEFAULT_PATH)

set(Boost_LIBRARIES  ${BOOST_FILESYSTEM} ${BOOST_SYSTEM} ${BOOST_SERIALIZATION}  ${BOOST_CHRONO} ${BOOST_THREAD} )

target_link_libraries(run stdc++ ${Boost_LIBRARIES} ${MYSQL_REQ_LIBRARIES})
