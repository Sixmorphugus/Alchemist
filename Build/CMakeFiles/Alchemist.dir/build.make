# CMAKE generated file: DO NOT EDIT!
# Generated by "NMake Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF
SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:\Alchemist

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\Alchemist\Build

# Include any dependencies generated for this target.
include CMakeFiles\Alchemist.dir\depend.make

# Include the progress variables for this target.
include CMakeFiles\Alchemist.dir\progress.make

# Include the compile flags for this target's objects.
include CMakeFiles\Alchemist.dir\flags.make

CMakeFiles\Alchemist.dir\Source\CPP\Alchemist.cpp.o: CMakeFiles\Alchemist.dir\flags.make
CMakeFiles\Alchemist.dir\Source\CPP\Alchemist.cpp.o: ..\Source\CPP\Alchemist.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\Alchemist\Build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Alchemist.dir/Source/CPP/Alchemist.cpp.o"
	C:\emsdk\upstream\emscripten\em++.bat  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\Alchemist.dir\Source\CPP\Alchemist.cpp.o -c D:\Alchemist\Source\CPP\Alchemist.cpp

CMakeFiles\Alchemist.dir\Source\CPP\Alchemist.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Alchemist.dir/Source/CPP/Alchemist.cpp.i"
	C:\emsdk\upstream\emscripten\em++.bat $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\Alchemist\Source\CPP\Alchemist.cpp > CMakeFiles\Alchemist.dir\Source\CPP\Alchemist.cpp.i

CMakeFiles\Alchemist.dir\Source\CPP\Alchemist.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Alchemist.dir/Source/CPP/Alchemist.cpp.s"
	C:\emsdk\upstream\emscripten\em++.bat $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\Alchemist\Source\CPP\Alchemist.cpp -o CMakeFiles\Alchemist.dir\Source\CPP\Alchemist.cpp.s

# Object files for target Alchemist
Alchemist_OBJECTS = \
"CMakeFiles\Alchemist.dir\Source\CPP\Alchemist.cpp.o"

# External object files for target Alchemist
Alchemist_EXTERNAL_OBJECTS =

Alchemist.html: CMakeFiles\Alchemist.dir\Source\CPP\Alchemist.cpp.o
Alchemist.html: CMakeFiles\Alchemist.dir\build.make
Alchemist.html: CMakeFiles\Alchemist.dir\linklibs.rsp
Alchemist.html: CMakeFiles\Alchemist.dir\objects1.rsp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\Alchemist\Build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Alchemist.html"
	C:\emsdk\upstream\emscripten\em++.bat   -std=c++11 --embed-file D:/Alchemist/Resources --shell-file D:/Alchemist/Source/HTML/Shell.html -O2  -O2 -g @CMakeFiles\Alchemist.dir\objects1.rsp  -o Alchemist.html @CMakeFiles\Alchemist.dir\linklibs.rsp

# Rule to build all files generated by this target.
CMakeFiles\Alchemist.dir\build: Alchemist.html

.PHONY : CMakeFiles\Alchemist.dir\build

CMakeFiles\Alchemist.dir\clean:
	$(CMAKE_COMMAND) -P CMakeFiles\Alchemist.dir\cmake_clean.cmake
.PHONY : CMakeFiles\Alchemist.dir\clean

CMakeFiles\Alchemist.dir\depend:
	$(CMAKE_COMMAND) -E cmake_depends "NMake Makefiles" D:\Alchemist D:\Alchemist D:\Alchemist\Build D:\Alchemist\Build D:\Alchemist\Build\CMakeFiles\Alchemist.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles\Alchemist.dir\depend
