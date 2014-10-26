cmake_minimum_required(VERSION 3.0)

project(UsageEnvironment)

set(SOURCE_FILES
    src/HashTable.cpp
    src/UsageEnvironment.cpp
    src/strDup.cpp
)

include_directories(
    include
    ../groupsock/include
)

add_library(UsageEnvironment ${SOURCE_FILES})

target_include_directories(UsageEnvironment
    PUBLIC include
)
