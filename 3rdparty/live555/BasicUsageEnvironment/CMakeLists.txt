cmake_minimum_required(VERSION 3.0)

project(BasicUsageEnvironment)

set(SOURCE_FILES
    src/BasicHashTable.cpp
    src/BasicTaskScheduler.cpp
    src/BasicTaskScheduler0.cpp
    src/BasicUsageEnvironment.cpp
    src/BasicUsageEnvironment0.cpp
    src/DelayQueue.cpp
)

include_directories(
    include
    ../UsageEnvironment/include
    ../groupsock/include
)

add_library(BasicUsageEnvironment ${SOURCE_FILES})

target_include_directories(BasicUsageEnvironment
	PUBLIC include
)
