# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.19

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/toufupi/CLionProjects/Text-bird

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/toufupi/CLionProjects/Text-bird/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/Text_bird.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Text_bird.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Text_bird.dir/flags.make

CMakeFiles/Text_bird.dir/game.c.o: CMakeFiles/Text_bird.dir/flags.make
CMakeFiles/Text_bird.dir/game.c.o: ../game.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/toufupi/CLionProjects/Text-bird/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/Text_bird.dir/game.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/Text_bird.dir/game.c.o -c /Users/toufupi/CLionProjects/Text-bird/game.c

CMakeFiles/Text_bird.dir/game.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Text_bird.dir/game.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/toufupi/CLionProjects/Text-bird/game.c > CMakeFiles/Text_bird.dir/game.c.i

CMakeFiles/Text_bird.dir/game.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Text_bird.dir/game.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/toufupi/CLionProjects/Text-bird/game.c -o CMakeFiles/Text_bird.dir/game.c.s

CMakeFiles/Text_bird.dir/playaudio.c.o: CMakeFiles/Text_bird.dir/flags.make
CMakeFiles/Text_bird.dir/playaudio.c.o: ../playaudio.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/toufupi/CLionProjects/Text-bird/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/Text_bird.dir/playaudio.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/Text_bird.dir/playaudio.c.o -c /Users/toufupi/CLionProjects/Text-bird/playaudio.c

CMakeFiles/Text_bird.dir/playaudio.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Text_bird.dir/playaudio.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/toufupi/CLionProjects/Text-bird/playaudio.c > CMakeFiles/Text_bird.dir/playaudio.c.i

CMakeFiles/Text_bird.dir/playaudio.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Text_bird.dir/playaudio.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/toufupi/CLionProjects/Text-bird/playaudio.c -o CMakeFiles/Text_bird.dir/playaudio.c.s

# Object files for target Text_bird
Text_bird_OBJECTS = \
"CMakeFiles/Text_bird.dir/game.c.o" \
"CMakeFiles/Text_bird.dir/playaudio.c.o"

# External object files for target Text_bird
Text_bird_EXTERNAL_OBJECTS =

Text_bird: CMakeFiles/Text_bird.dir/game.c.o
Text_bird: CMakeFiles/Text_bird.dir/playaudio.c.o
Text_bird: CMakeFiles/Text_bird.dir/build.make
Text_bird: CMakeFiles/Text_bird.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/toufupi/CLionProjects/Text-bird/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable Text_bird"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Text_bird.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Text_bird.dir/build: Text_bird

.PHONY : CMakeFiles/Text_bird.dir/build

CMakeFiles/Text_bird.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Text_bird.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Text_bird.dir/clean

CMakeFiles/Text_bird.dir/depend:
	cd /Users/toufupi/CLionProjects/Text-bird/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/toufupi/CLionProjects/Text-bird /Users/toufupi/CLionProjects/Text-bird /Users/toufupi/CLionProjects/Text-bird/cmake-build-debug /Users/toufupi/CLionProjects/Text-bird/cmake-build-debug /Users/toufupi/CLionProjects/Text-bird/cmake-build-debug/CMakeFiles/Text_bird.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Text_bird.dir/depend

