# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.31

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
CMAKE_COMMAND = /opt/homebrew/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/larrypark/Projects/PlaylistManager/PlaylistManager/backend

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/larrypark/Projects/PlaylistManager/PlaylistManager/backend/build

# Include any dependencies generated for this target.
include CMakeFiles/PlaylistManagerTests.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/PlaylistManagerTests.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/PlaylistManagerTests.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/PlaylistManagerTests.dir/flags.make

CMakeFiles/PlaylistManagerTests.dir/codegen:
.PHONY : CMakeFiles/PlaylistManagerTests.dir/codegen

CMakeFiles/PlaylistManagerTests.dir/tests/test_main.cpp.o: CMakeFiles/PlaylistManagerTests.dir/flags.make
CMakeFiles/PlaylistManagerTests.dir/tests/test_main.cpp.o: /Users/larrypark/Projects/PlaylistManager/PlaylistManager/backend/tests/test_main.cpp
CMakeFiles/PlaylistManagerTests.dir/tests/test_main.cpp.o: CMakeFiles/PlaylistManagerTests.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/larrypark/Projects/PlaylistManager/PlaylistManager/backend/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/PlaylistManagerTests.dir/tests/test_main.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/PlaylistManagerTests.dir/tests/test_main.cpp.o -MF CMakeFiles/PlaylistManagerTests.dir/tests/test_main.cpp.o.d -o CMakeFiles/PlaylistManagerTests.dir/tests/test_main.cpp.o -c /Users/larrypark/Projects/PlaylistManager/PlaylistManager/backend/tests/test_main.cpp

CMakeFiles/PlaylistManagerTests.dir/tests/test_main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/PlaylistManagerTests.dir/tests/test_main.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/larrypark/Projects/PlaylistManager/PlaylistManager/backend/tests/test_main.cpp > CMakeFiles/PlaylistManagerTests.dir/tests/test_main.cpp.i

CMakeFiles/PlaylistManagerTests.dir/tests/test_main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/PlaylistManagerTests.dir/tests/test_main.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/larrypark/Projects/PlaylistManager/PlaylistManager/backend/tests/test_main.cpp -o CMakeFiles/PlaylistManagerTests.dir/tests/test_main.cpp.s

# Object files for target PlaylistManagerTests
PlaylistManagerTests_OBJECTS = \
"CMakeFiles/PlaylistManagerTests.dir/tests/test_main.cpp.o"

# External object files for target PlaylistManagerTests
PlaylistManagerTests_EXTERNAL_OBJECTS =

PlaylistManagerTests: CMakeFiles/PlaylistManagerTests.dir/tests/test_main.cpp.o
PlaylistManagerTests: CMakeFiles/PlaylistManagerTests.dir/build.make
PlaylistManagerTests: /opt/homebrew/lib/libgtest.a
PlaylistManagerTests: /opt/homebrew/lib/libgtest_main.a
PlaylistManagerTests: /opt/homebrew/lib/libgtest.a
PlaylistManagerTests: CMakeFiles/PlaylistManagerTests.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/larrypark/Projects/PlaylistManager/PlaylistManager/backend/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable PlaylistManagerTests"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/PlaylistManagerTests.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/PlaylistManagerTests.dir/build: PlaylistManagerTests
.PHONY : CMakeFiles/PlaylistManagerTests.dir/build

CMakeFiles/PlaylistManagerTests.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/PlaylistManagerTests.dir/cmake_clean.cmake
.PHONY : CMakeFiles/PlaylistManagerTests.dir/clean

CMakeFiles/PlaylistManagerTests.dir/depend:
	cd /Users/larrypark/Projects/PlaylistManager/PlaylistManager/backend/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/larrypark/Projects/PlaylistManager/PlaylistManager/backend /Users/larrypark/Projects/PlaylistManager/PlaylistManager/backend /Users/larrypark/Projects/PlaylistManager/PlaylistManager/backend/build /Users/larrypark/Projects/PlaylistManager/PlaylistManager/backend/build /Users/larrypark/Projects/PlaylistManager/PlaylistManager/backend/build/CMakeFiles/PlaylistManagerTests.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/PlaylistManagerTests.dir/depend
