
cmake_minimum_required(VERSION 3.0)

include(ExternalProject)

include(${CMAKE_SOURCE_DIR}/CMakeLists.Destination.txt)
set(X264_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "x264 include dirs")
set(X264_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/${MY_PLATFORM})
set(X264_LIB ${X264_LIB_DIR}/libx264.a CACHE INTERNAL "x264 lib")
set(X264_CONFIGURE_PARAMS
	--bindir=${CMAKE_CURRENT_SOURCE_DIR}/bin --includedir=${X264_INCLUDE_DIRS} --libdir=${X264_LIB_DIR}
)

execute_process(
	COMMAND yasm --version
	OUTPUT_VARIABLE YASM_VERSION
)
string(SUBSTRING ${YASM_VERSION} 5 10 YASM_VERSION)

if(${YASM_VERSION} LESS "1.2.0")
	set(X264_CONFIGURE_PARAMS ${X264_CONFIGURE_PARAMS} --disable-asm)
	message(WARNING "X264: yasm version too old: ${YASM_VERSION} (it shall be at least 1.2.0) -> disabling asm optimizations.")
endif()


ExternalProject_Add(
  X264
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/x264-0.142.X
  DOWNLOAD_COMMAND ""
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND bash ./configure ${X264_CONFIGURE_PARAMS}
  BUILD_COMMAND ${MY_BUILD_COMMAND}
  BUILD_IN_SOURCE 1
)

ExternalProject_Add_Step(X264 installInternally
                         COMMAND cd <SOURCE_DIR> && make install-lib-static
                         DEPENDEES install
                         ALWAYS 1
)

