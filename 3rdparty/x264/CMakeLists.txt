
cmake_minimum_required(VERSION 3.0)

include(ExternalProject)

set(MY_BUILD_COMMAND make)
if(WIN32)
  set(MY_BUILD_COMMAND mingw32-make)
endif()

message(STATUS "build command: " ${MY_BUILD_COMMAND})

include(${CMAKE_SOURCE_DIR}/CMakeLists.Destination.txt)
set(X264_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "x264 include dirs")
set(X264_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/${MY_PLATFORM})
set(X264_LIB ${X264_LIB_DIR}/libx264.a CACHE INTERNAL "x264 lib")

ExternalProject_Add(
  X264
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/x264
  DOWNLOAD_COMMAND ""
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND sh ./configure --bindir=${CMAKE_CURRENT_SOURCE_DIR}/bin --includedir=${X264_INCLUDE_DIRS} --libdir=${X264_LIB_DIR}
  BUILD_COMMAND ${MY_BUILD_COMMAND}
  BUILD_IN_SOURCE 1
)

ExternalProject_Add_Step(X264 installInternally
                         COMMAND cd <SOURCE_DIR> && make install-lib-static
                         DEPENDEES install
                         ALWAYS 1)

