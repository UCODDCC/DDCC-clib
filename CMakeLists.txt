cmake_minimum_required (VERSION 2.8.11)

project (DDCD-clib)

set(SOURCES
        libddcd.c
)

add_executable(client main.c ${SOURCES})


target_include_directories(client PRIVATE ${PROJECT_SOURCE_DIR})

find_package(Threads REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(client Threads::Threads)

target_compile_definitions (client PRIVATE -DDEBUG)
target_compile_definitions (client PRIVATE -DLOCAL)