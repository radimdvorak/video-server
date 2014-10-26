cmake_minimum_required(VERSION 3.0)

project(video-server)

set(SOURCE_FILES
  src/Utils/Conversion.cpp
  src/Utils/AVPixFormatHelper.cpp
  src/Application.cpp
  src/CvVideoCapture.cpp
  src/H264LiveServerMediaSession.cpp
  src/LiveSourceWithX264.cpp
  src/PvVideoCapture.cpp
  src/TerminateHandler.cpp
  src/X264Encoder.cpp
  src/main.cpp
)

include_directories(
    ../3rdparty/include
    ../3rdparty/include/ffmpeg
    ${X264_INCLUDE_DIRS}
    ${PVAPI_INCLUDE_DIRS}
)

add_executable(video-server ${SOURCE_FILES})

include(${CMAKE_SOURCE_DIR}/CMakeLists.Destination.txt)

if(WIN32)
    set(FFMPEG_LIBRARIES
        ${MY_3RDPARTY_LIB_DEST}/swscale-2.dll
        ${MY_3RDPARTY_LIB_DEST}/ws2_32.dll
        ${MY_3RDPARTY_LIB_DEST}/avutil-52.dll
    )
else()
    set(FFMPEG_LIBRARIES
        ${MY_3RDPARTY_LIB_DEST}/libswscale-2.so
        ${MY_3RDPARTY_LIB_DEST}/libws2_32.so
        ${MY_3RDPARTY_LIB_DEST}/libavutil-52.so
    )
endif()

message(STATUS "FFMPEG_LIBRARIES: " ${FFMPEG_LIBRARIES})

target_link_libraries(video-server
	PUBLIC liveMedia
	PUBLIC UsageEnvironment
	PUBLIC BasicUsageEnvironment
	PUBLIC groupsock
	${X264_LIB}
	${PVAPI_LIB}
	${FFMPEG_LIBRARIES}
	pthread
	opencv_core246
	opencv_highgui246
	PocoFoundation
	PocoUtil
)

install(
	TARGETS video-server
	DESTINATION ${MY_DEST}
)
