cmake_minimum_required(VERSION 3.7)
project(l3database)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        Headers/dbwr.h
        Headers/editor.h
        Headers/history.h
        Headers/history_test.h
        Headers/instance.h
        Headers/joinAlgorithms.h
        Headers/LRUCache.h
        Headers/outils.h
        Headers/parser.h
        Headers/tableOperations.h
        Headers/timestamp.h
        Headers/typedef.h
        Sources/Parser/alter.c
        Sources/Parser/create.c
        Sources/Parser/delete.c
        Sources/Parser/insert.c
        Sources/Parser/parser.c
        Sources/Parser/select.c
        Sources/Parser/update.c
        Sources/dbwr.c
        Sources/editor.c
        Sources/history.c
        Sources/history_test.c
        Sources/instance.c
        Sources/joinAlgorithms.c
        Sources/LRUCache.c
        Sources/outils.c
        Sources/tableOperations.c
        Sources/test.c
        Sources/testUnit.c
        data.dbf)

add_executable(l3database ${SOURCE_FILES})

find_package (Threads)
target_link_libraries (l3database ${CMAKE_THREAD_LIBS_INIT})
