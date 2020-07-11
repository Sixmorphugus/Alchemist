cmake_minimum_required(VERSION 3.12)
project(Alchemist)

if(CMAKE_SYSTEM_NAME MATCHES "Emscripten")
	message(STATUS "We seem to be building for the web.")
	
	add_compile_definitions(IS_WEB=1)
	set(IS_WEB 1)
	
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS=['png'] -s USE_SDL_TTF=2 -std=c++17 --preload-file ${CMAKE_SOURCE_DIR}/Resources@/Resources --shell-file ${CMAKE_SOURCE_DIR}/Source/HTML/Shell.html")
	
	set(CMAKE_EXECUTABLE_SUFFIX .html)
else()
	message(STATUS "We seem to be building for the desktop.")
	
	if (MSVC_VERSION GREATER_EQUAL "1900")
		include(CheckCXXCompilerFlag)
		CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
		if (_cpp_latest_flag_supported)
			add_compile_options("/std:c++latest")
		endif()
	endif()
	
	add_compile_definitions(IS_WEB=0)
	set(IS_WEB 0)
	
	set(SDL2_INCLUDE_DIRS 
		"${CMAKE_SOURCE_DIR}/ThirdParty/SDL2/include"
		"${CMAKE_SOURCE_DIR}/ThirdParty/SDL2_image/include"
		"${CMAKE_SOURCE_DIR}/ThirdParty/SDL2_ttf/include"
	)
	
	set(SDL2_LIBRARIES
		"${CMAKE_SOURCE_DIR}/ThirdParty/SDL2/lib/x64/SDL2.lib"
		"${CMAKE_SOURCE_DIR}/ThirdParty/SDL2/lib/x64/SDL2main.lib"
		"${CMAKE_SOURCE_DIR}/ThirdParty/SDL2/lib/x64/SDL2test.lib"
		"${CMAKE_SOURCE_DIR}/ThirdParty/SDL2_image/lib/x64/SDL2_image.lib"
		"${CMAKE_SOURCE_DIR}/ThirdParty/SDL2_ttf/lib/x64/SDL2_ttf.lib"
	)
endif()

FILE(GLOB_RECURSE SOURCE CONFIGURE_DEPENDS 
	"${CMAKE_SOURCE_DIR}/Source/CPP/*.cpp"
	"${CMAKE_SOURCE_DIR}/Source/CPP/*.h"
)

message(STATUS "SOURCE FILES ${SOURCE}")

include_directories(${CMAKE_SOURCE_DIR}/Source/CPP)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(Alchemist ${SOURCE})
target_link_libraries(Alchemist ${SDL2_LIBRARIES})

if(NOT IS_WEB)
	# SDL2 DLL
	add_custom_command(TARGET Alchemist POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/ThirdParty/SDL2/lib/x64/SDL2.dll"
        $<TARGET_FILE_DIR:Alchemist>)
	
	# SDL2_image DLLs
	add_custom_command(TARGET Alchemist POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/ThirdParty/SDL2_image/lib/x64/SDL2_image.dll"
        $<TARGET_FILE_DIR:Alchemist>)
		
	add_custom_command(TARGET Alchemist POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/ThirdParty/SDL2_image/lib/x64/zlib1.dll"
        $<TARGET_FILE_DIR:Alchemist>)
	
	add_custom_command(TARGET Alchemist POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/ThirdParty/SDL2_image/lib/x64/libpng16-16.dll"
        $<TARGET_FILE_DIR:Alchemist>)
	
	# SDL2_ttf DLLs
	add_custom_command(TARGET Alchemist POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/ThirdParty/SDL2_ttf/lib/x64/SDL2_ttf.dll"
        $<TARGET_FILE_DIR:Alchemist>)
	
	add_custom_command(TARGET Alchemist POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/ThirdParty/SDL2_ttf/lib/x64/libfreetype-6.dll"
        $<TARGET_FILE_DIR:Alchemist>)
	
	add_custom_command(TARGET Alchemist POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/ThirdParty/SDL2_ttf/lib/x64/zlib1.dll"
        $<TARGET_FILE_DIR:Alchemist>)
	
	# Data
	add_custom_command(TARGET Alchemist POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory 
        "${CMAKE_SOURCE_DIR}/Resources"
        $<TARGET_FILE_DIR:Alchemist>/Resources)
	
	# Working directory fix!
	set_target_properties(
		Alchemist PROPERTIES
		VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:Alchemist>)
endif()

foreach(_source IN ITEMS ${SOURCE})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()