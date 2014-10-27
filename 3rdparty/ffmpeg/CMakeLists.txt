
cmake_minimum_required(VERSION 3.0)

include(ExternalProject)

include(${CMAKE_SOURCE_DIR}/CMakeLists.Destination.txt)
set(FFMPEG_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "ffmpeg include dirs")
set(FFMPEG_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/${MY_PLATFORM})
set(FFMPEG_LIBS ${FFMPEG_LIB_DIR}/libswscale.a ${FFMPEG_LIB_DIR}/libavutil.a
	CACHE INTERNAL "ffmpeg libs"
)

execute_process(
	COMMAND nproc
	OUTPUT_VARIABLE MY_NPROC
)
set(MY_BUILD_COMMAND_FFMPEG ${MY_BUILD_COMMAND} -j ${MY_NPROC})

ExternalProject_Add(
  FFMPEG
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-2.4.2
  DOWNLOAD_COMMAND ""
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND ./configure --prefix=${CMAKE_CURRENT_SOURCE_DIR} --bindir=${CMAKE_CURRENT_SOURCE_DIR}/bin --incdir=${FFMPEG_INCLUDE_DIRS} --libdir=${FFMPEG_LIB_DIR} --disable-programs --disable-doc
  BUILD_COMMAND ${MY_BUILD_COMMAND_FFMPEG}
  BUILD_IN_SOURCE 1
)

ExternalProject_Add_Step(FFMPEG installInternally
                         COMMAND cd <SOURCE_DIR> && make install
                         DEPENDEES install
                         ALWAYS 1
)

