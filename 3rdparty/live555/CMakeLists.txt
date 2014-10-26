
cmake_minimum_required(VERSION 3.0)

if(WIN32)
  set(LIVE555_COMPILER_FLAGS "-DSOCKLEN_T=int -DLOCALE_NOT_USED")
  set(CMAKE_C_FLAGS "${LIVE555_COMPILER_FLAGS}")
  set(CMAKE_CXX_FLAGS "${LIVE555_COMPILER_FLAGS}")
endif()

add_subdirectory(groupsock)
add_subdirectory(BasicUsageEnvironment)
add_subdirectory(UsageEnvironment)
add_subdirectory(liveMedia)
