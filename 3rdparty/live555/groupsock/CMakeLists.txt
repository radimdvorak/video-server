cmake_minimum_required(VERSION 3.0)

project(groupsock)

set(SOURCE_FILES
    src/GroupEId.cpp
    src/Groupsock.cpp
    src/GroupsockHelper.cpp
    src/inet.c
    src/IOHandlers.cpp
    src/NetAddress.cpp
    src/NetInterface.cpp
)

include_directories(
    include
    ../UsageEnvironment/include
)

add_library(groupsock ${SOURCE_FILES})

target_include_directories(groupsock
	PUBLIC include
)
