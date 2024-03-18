cmake_minimum_required(VERSION 3.10)
project(COMP7402_Project)

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SOURCE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/src")


set(COMMON_LIST
        "${INCLUDE_DIR}/common.h"
        "${INCLUDE_DIR}/brainpool.h"
        "${INCLUDE_DIR}/key.h"

        "${SOURCE_DIR}/common.cpp"
        "${SOURCE_DIR}/brainpool.cpp"
        "${SOURCE_DIR}/key.cpp"
)


set(SERVER_LIST
        "${INCLUDE_DIR}/Server.h"
        "${INCLUDE_DIR}/ServerHelper.h"
        "${INCLUDE_DIR}/User.h"

        "${SOURCE_DIR}/Server.cpp"
        "${SOURCE_DIR}/ServerHelper.cpp"
        "${SOURCE_DIR}/User.cpp"
)


set(CLIENT_LIST
        "${INCLUDE_DIR}/Client.h"

        "${SOURCE_DIR}/Client.cpp"
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


add_executable(server ${COMMON_LIST} ${SERVER_LIST} ${HEADER_LIST})
add_executable(client ${COMMON_LIST} ${CLIENT_LIST} ${HEADER_LIST})

target_link_libraries(server PRIVATE ssl crypto)
target_compile_features(server PRIVATE cxx_std_17)
target_include_directories(server PRIVATE ${INCLUDE_DIR})
target_link_libraries(client PRIVATE ssl crypto)
target_compile_features(client PRIVATE cxx_std_17)
target_include_directories(client PRIVATE ${INCLUDE_DIR})


set_target_properties(server PROPERTIES OUTPUT_NAME "server")
set_target_properties(client PROPERTIES OUTPUT_NAME "client")

