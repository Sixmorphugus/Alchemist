cmake_minimum_required(VERSION 3.12)
project(Alchemist)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 --embed-file ${CMAKE_SOURCE_DIR}/Resources --shell-file ${CMAKE_SOURCE_DIR}/Source/HTML/Shell.html")

FILE(GLOB_RECURSE SOURCE CONFIGURE_DEPENDS 
	"${CMAKE_SOURCE_DIR}/Source/CPP/*.cpp"
	"${CMAKE_SOURCE_DIR}/Source/CPP/*.h"
)

message(STATUS "SOURCE FILES ${SOURCE}")

add_executable(Alchemist ${SOURCE})
set(CMAKE_EXECUTABLE_SUFFIX .html)
target_link_libraries(Alchemist SDL)