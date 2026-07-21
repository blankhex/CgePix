cmake_minimum_required(VERSION 3.10)
project(CgePix LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(SOURCES
    Color.c
    Pix.c
)

set(HEADERS
    CgePix.h
)

add_library(CgePix STATIC ${SOURCES} ${HEADERS})

target_include_directories(CgePix PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS CgePix
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES ${HEADERS} DESTINATION include)
