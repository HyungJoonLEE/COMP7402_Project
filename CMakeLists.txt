cmake_minimum_required(VERSION 3.10)
project(COMP7402_Project)

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SOURCE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(HEADER_LIST
        "${INCLUDE_DIR}/ServerHelper.h"
        "${INCLUDE_DIR}/ClientHelper.h"
        "${INCLUDE_DIR}/common.h"
#        "${PROJECT_SOURCE_DIR}/include/key.h"
)


set(COMMON_SOURCE_LIST
        "${SOURCE_DIR}/common.cpp"
)


set(SERVER_SOURCE_LIST
        "${SOURCE_DIR}/Server.cpp"
        "${SOURCE_DIR}/ServerHelper.cpp"
)


set(CLIENT_SOURCE_LIST
        "${SOURCE_DIR}/Client.cpp"
        "${SOURCE_DIR}/ClientHelper.cpp"
)


set(OPENSSL_USE_STATIC_LIBS TRUE)
set(CMAKE_THREAD_LIBS_INIT "-lpthread")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_USE_WIN32_THREADS_INIT 0)
set(CMAKE_USE_PTHREADS_INIT 1)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_library(LIBM m REQUIRED)
find_library(LIBSOCKET socket)


add_executable(server ${COMMON_SOURCE_LIST} ${SERVER_SOURCE_LIST} ${HEADER_LIST})
add_executable(client ${COMMON_SOURCE_LIST} ${CLIENT_SOURCE_LIST} ${HEADER_LIST})

target_link_libraries(server PRIVATE ssl crypto)
target_compile_features(server PRIVATE cxx_std_17)
target_include_directories(server PRIVATE ${INCLUDE_DIR})
target_link_libraries(client PRIVATE ssl crypto)
target_compile_features(client PRIVATE cxx_std_17)
target_include_directories(client PRIVATE ${INCLUDE_DIR})


set_target_properties(server PROPERTIES OUTPUT_NAME "server")
set_target_properties(client PROPERTIES OUTPUT_NAME "client")

