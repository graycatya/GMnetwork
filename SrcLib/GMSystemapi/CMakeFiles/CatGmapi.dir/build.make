# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/cat/code/GMnetwork/SrcLib/GMSystemapi

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/cat/code/GMnetwork/SrcLib/GMSystemapi

# Include any dependencies generated for this target.
include CMakeFiles/CatGmapi.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/CatGmapi.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/CatGmapi.dir/flags.make

CMakeFiles/CatGmapi.dir/CatGmapi.c.o: CMakeFiles/CatGmapi.dir/flags.make
CMakeFiles/CatGmapi.dir/CatGmapi.c.o: CatGmapi.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cat/code/GMnetwork/SrcLib/GMSystemapi/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/CatGmapi.dir/CatGmapi.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/CatGmapi.dir/CatGmapi.c.o   -c /home/cat/code/GMnetwork/SrcLib/GMSystemapi/CatGmapi.c

CMakeFiles/CatGmapi.dir/CatGmapi.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/CatGmapi.dir/CatGmapi.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/cat/code/GMnetwork/SrcLib/GMSystemapi/CatGmapi.c > CMakeFiles/CatGmapi.dir/CatGmapi.c.i

CMakeFiles/CatGmapi.dir/CatGmapi.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/CatGmapi.dir/CatGmapi.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/cat/code/GMnetwork/SrcLib/GMSystemapi/CatGmapi.c -o CMakeFiles/CatGmapi.dir/CatGmapi.c.s

CMakeFiles/CatGmapi.dir/CatGmapi.c.o.requires:

.PHONY : CMakeFiles/CatGmapi.dir/CatGmapi.c.o.requires

CMakeFiles/CatGmapi.dir/CatGmapi.c.o.provides: CMakeFiles/CatGmapi.dir/CatGmapi.c.o.requires
	$(MAKE) -f CMakeFiles/CatGmapi.dir/build.make CMakeFiles/CatGmapi.dir/CatGmapi.c.o.provides.build
.PHONY : CMakeFiles/CatGmapi.dir/CatGmapi.c.o.provides

CMakeFiles/CatGmapi.dir/CatGmapi.c.o.provides.build: CMakeFiles/CatGmapi.dir/CatGmapi.c.o


# Object files for target CatGmapi
CatGmapi_OBJECTS = \
"CMakeFiles/CatGmapi.dir/CatGmapi.c.o"

# External object files for target CatGmapi
CatGmapi_EXTERNAL_OBJECTS =

libCatGmapi.so: CMakeFiles/CatGmapi.dir/CatGmapi.c.o
libCatGmapi.so: CMakeFiles/CatGmapi.dir/build.make
libCatGmapi.so: CMakeFiles/CatGmapi.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/cat/code/GMnetwork/SrcLib/GMSystemapi/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C shared library libCatGmapi.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/CatGmapi.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/CatGmapi.dir/build: libCatGmapi.so

.PHONY : CMakeFiles/CatGmapi.dir/build

CMakeFiles/CatGmapi.dir/requires: CMakeFiles/CatGmapi.dir/CatGmapi.c.o.requires

.PHONY : CMakeFiles/CatGmapi.dir/requires

CMakeFiles/CatGmapi.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/CatGmapi.dir/cmake_clean.cmake
.PHONY : CMakeFiles/CatGmapi.dir/clean

CMakeFiles/CatGmapi.dir/depend:
	cd /home/cat/code/GMnetwork/SrcLib/GMSystemapi && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/cat/code/GMnetwork/SrcLib/GMSystemapi /home/cat/code/GMnetwork/SrcLib/GMSystemapi /home/cat/code/GMnetwork/SrcLib/GMSystemapi /home/cat/code/GMnetwork/SrcLib/GMSystemapi /home/cat/code/GMnetwork/SrcLib/GMSystemapi/CMakeFiles/CatGmapi.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/CatGmapi.dir/depend

