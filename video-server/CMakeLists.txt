cmake_minimum_required(VERSION 3.0)

project(video-server)

if(UNIX)
	add_definitions(-D_LINUX -D_x86 -D__STDINT_MACROS -D__STDC_CONSTANT_MACROS)
endif()

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
    ${FFMPEG_INCLUDE_DIRS}
    ${X264_INCLUDE_DIRS}
    ${PVAPI_INCLUDE_DIRS}
)

add_executable(video-server ${SOURCE_FILES})

include(${CMAKE_SOURCE_DIR}/CMakeLists.Destination.txt)

set(OPENCV_LIBS opencv_core opencv_highgui)
if(WIN32)
	set(OPENCV_LIBS opencv_core246 opencv_highgui246)
endif()

set(WIN32_LIBS "")
if(WIN32)
	set(WIN32_LIBS ws2_32)
endif()

target_link_libraries(video-server
	PUBLIC liveMedia
	PUBLIC UsageEnvironment
	PUBLIC BasicUsageEnvironment
	PUBLIC groupsock
	${X264_LIB}
	${PVAPI_LIB}
	${FFMPEG_LIBS}
	pthread
	${OPENCV_LIBS}
	PocoFoundation
	PocoUtil
  ${WIN32_LIBS}
)

install(
	TARGETS video-server
	DESTINATION ${MY_DEST}
)

