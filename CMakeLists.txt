cmake_minimum_required(VERSION 3.12)
project(Alchemist)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS=['png'] -std=c++11 --preload-file ${CMAKE_SOURCE_DIR}/Resources@/Resources --shell-file ${CMAKE_SOURCE_DIR}/Source/HTML/Shell.html")

FILE(GLOB_RECURSE SOURCE CONFIGURE_DEPENDS 
	"${CMAKE_SOURCE_DIR}/Source/CPP/*.cpp"
	"${CMAKE_SOURCE_DIR}/Source/CPP/*.h"
)

message(STATUS "SOURCE FILES ${SOURCE}")

include_directories(${SDL2_INCLUDE_DIRS})

add_executable(Alchemist ${SOURCE})
set(CMAKE_EXECUTABLE_SUFFIX .html)
target_link_libraries(Alchemist ${SDL2_LIBRARIES})
