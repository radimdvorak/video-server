
cmake_minimum_required(VERSION 3.0)

if(UNIX)
   set(PVAPI_LIB_NAME libPvAPI.so)
else()
   set(PVAPI_LIB_NAME PvAPI.dll)
endif(UNIX)

set(PVAPI_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "pvapi include dirs")
set(PVAPI_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/${MY_PLATFORM})
set(PVAPI_LIB ${PVAPI_LIB_DIR}/${PVAPI_LIB_NAME} CACHE INTERNAL "pvapi lib")

add_library(pvapi SHARED IMPORTED)
set_property(TARGET pvapi PROPERTY IMPORTED_LOCATION ${PVAPI_LIB})
