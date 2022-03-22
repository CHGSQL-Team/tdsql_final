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

use_cxx17()

include_directories(include)

set(Boost_INCLUDE_DIRS thirdparty/boost/include)

set(MYSQLCLIENT_INCLUDE_DIR thirdparty/mysqlclient/include)

include_directories(${Boost_INCLUDE_DIRS} thirdparty/mysql-connector-cpp/include)
include_directories(/usr/lib/jvm/java-1.8.0/include /usr/lib/jvm/java-1.8.0/include/linux)

find_library(MYSQL_LIBRARIES NAMES mysqlcppconn PATHS thirdparty/mysql-connector-cpp/lib/ NO_DEFAULT_PATH)

find_library(MYSQL_CLIENT_LIBRARIES NAMES mysqlclient PATHS thirdparty/mysqlclient/lib NO_DEFAULT_PATH)

find_library(SSL NAMES ssl PATHS thirdparty/openssl/lib NO_DEFAULT_PATH)
find_library(CRYPTO NAMES crypto PATHS thirdparty/openssl/lib NO_DEFAULT_PATH)

find_library(JVM NAMES jvm PATHS /usr/lib/jvm/java-1.8.0/jre/lib/amd64/server)

set(MYSQL_REQ_LIBRARIES ${MYSQL_CLIENT_LIBRARIES} ${MYSQL_LIBRARIES} pthread ${SSL} ${CRYPTO} dl z resolv)

include_directories(${MYSQLCLIENT_INCLUDE_DIR})

add_executable(run
        src/main.cpp
        src/structure/config.cpp
        src/structure/descriptor.cpp
        src/utils/logger.cpp
        src/structure/module.cpp
        src/external/sqlconn.cpp
        src/external/extfetcher.cpp
        src/utils/timed.cpp
        src/external/jvmconn.cpp
        src/utils/workloader.cpp
        src/structure/statement.cpp
        src/worker_impl/worker.cpp
        src/worker_impl/subworker.cpp
        src/utils/iohelper.cpp
        src/structure/table.cpp
        src/utils/datparser.cpp
        src/utils/uniformlog.cpp
        src/worker_impl/pusher.cpp
        )

find_library(BOOST_FILESYSTEM NAMES boost_filesystem PATHS thirdparty/boost/lib/ NO_DEFAULT_PATH)
find_library(BOOST_SYSTEM NAMES boost_system PATHS thirdparty/boost/lib/ NO_DEFAULT_PATH)
find_library(BOOST_SERIALIZATION NAMES boost_serialization PATHS thirdparty/boost/lib/ NO_DEFAULT_PATH)
find_library(BOOST_THREAD NAMES boost_thread PATHS thirdparty/boost/lib/ NO_DEFAULT_PATH)
find_library(BOOST_CHRONO NAMES boost_chrono PATHS thirdparty/boost/lib/ NO_DEFAULT_PATH)

set(Boost_LIBRARIES  ${BOOST_FILESYSTEM} ${BOOST_SYSTEM} ${BOOST_SERIALIZATION}  ${BOOST_CHRONO} ${BOOST_THREAD} )

target_link_libraries(run stdc++ ${Boost_LIBRARIES} ${MYSQL_REQ_LIBRARIES} ${JVM})
