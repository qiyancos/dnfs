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
CMAKE_SOURCE_DIR = /home/iecas/rpc_test/nfs-ganesha/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/iecas/rpc_test/nfs-ganesha/src

# Include any dependencies generated for this target.
include libntirpc/tests/CMakeFiles/rpcping.dir/depend.make

# Include the progress variables for this target.
include libntirpc/tests/CMakeFiles/rpcping.dir/progress.make

# Include the compile flags for this target's objects.
include libntirpc/tests/CMakeFiles/rpcping.dir/flags.make

libntirpc/tests/CMakeFiles/rpcping.dir/rpcping.c.o: libntirpc/tests/CMakeFiles/rpcping.dir/flags.make
libntirpc/tests/CMakeFiles/rpcping.dir/rpcping.c.o: libntirpc/tests/rpcping.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/iecas/rpc_test/nfs-ganesha/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object libntirpc/tests/CMakeFiles/rpcping.dir/rpcping.c.o"
	cd /home/iecas/rpc_test/nfs-ganesha/src/libntirpc/tests && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/rpcping.dir/rpcping.c.o   -c /home/iecas/rpc_test/nfs-ganesha/src/libntirpc/tests/rpcping.c

libntirpc/tests/CMakeFiles/rpcping.dir/rpcping.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/rpcping.dir/rpcping.c.i"
	cd /home/iecas/rpc_test/nfs-ganesha/src/libntirpc/tests && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/iecas/rpc_test/nfs-ganesha/src/libntirpc/tests/rpcping.c > CMakeFiles/rpcping.dir/rpcping.c.i

libntirpc/tests/CMakeFiles/rpcping.dir/rpcping.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/rpcping.dir/rpcping.c.s"
	cd /home/iecas/rpc_test/nfs-ganesha/src/libntirpc/tests && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/iecas/rpc_test/nfs-ganesha/src/libntirpc/tests/rpcping.c -o CMakeFiles/rpcping.dir/rpcping.c.s

libntirpc/tests/CMakeFiles/rpcping.dir/rpcping.c.o.requires:

.PHONY : libntirpc/tests/CMakeFiles/rpcping.dir/rpcping.c.o.requires

libntirpc/tests/CMakeFiles/rpcping.dir/rpcping.c.o.provides: libntirpc/tests/CMakeFiles/rpcping.dir/rpcping.c.o.requires
	$(MAKE) -f libntirpc/tests/CMakeFiles/rpcping.dir/build.make libntirpc/tests/CMakeFiles/rpcping.dir/rpcping.c.o.provides.build
.PHONY : libntirpc/tests/CMakeFiles/rpcping.dir/rpcping.c.o.provides

libntirpc/tests/CMakeFiles/rpcping.dir/rpcping.c.o.provides.build: libntirpc/tests/CMakeFiles/rpcping.dir/rpcping.c.o


# Object files for target rpcping
rpcping_OBJECTS = \
"CMakeFiles/rpcping.dir/rpcping.c.o"

# External object files for target rpcping
rpcping_EXTERNAL_OBJECTS =

libntirpc/tests/rpcping: libntirpc/tests/CMakeFiles/rpcping.dir/rpcping.c.o
libntirpc/tests/rpcping: libntirpc/tests/CMakeFiles/rpcping.dir/build.make
libntirpc/tests/rpcping: libntirpc/src/libntirpc.so.5.0
libntirpc/tests/rpcping: /usr/lib/x86_64-linux-gnu/libnfsidmap.so
libntirpc/tests/rpcping: /usr/lib/libacl.so
libntirpc/tests/rpcping: /usr/lib/x86_64-linux-gnu/libblkid.so
libntirpc/tests/rpcping: /usr/lib/x86_64-linux-gnu/libuuid.so
libntirpc/tests/rpcping: /usr/lib/x86_64-linux-gnu/liburcu-bp.so
libntirpc/tests/rpcping: /usr/lib/x86_64-linux-gnu/libnsl.so
libntirpc/tests/rpcping: libntirpc/tests/CMakeFiles/rpcping.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/iecas/rpc_test/nfs-ganesha/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable rpcping"
	cd /home/iecas/rpc_test/nfs-ganesha/src/libntirpc/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/rpcping.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
libntirpc/tests/CMakeFiles/rpcping.dir/build: libntirpc/tests/rpcping

.PHONY : libntirpc/tests/CMakeFiles/rpcping.dir/build

libntirpc/tests/CMakeFiles/rpcping.dir/requires: libntirpc/tests/CMakeFiles/rpcping.dir/rpcping.c.o.requires

.PHONY : libntirpc/tests/CMakeFiles/rpcping.dir/requires

libntirpc/tests/CMakeFiles/rpcping.dir/clean:
	cd /home/iecas/rpc_test/nfs-ganesha/src/libntirpc/tests && $(CMAKE_COMMAND) -P CMakeFiles/rpcping.dir/cmake_clean.cmake
.PHONY : libntirpc/tests/CMakeFiles/rpcping.dir/clean

libntirpc/tests/CMakeFiles/rpcping.dir/depend:
	cd /home/iecas/rpc_test/nfs-ganesha/src && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/iecas/rpc_test/nfs-ganesha/src /home/iecas/rpc_test/nfs-ganesha/src/libntirpc/tests /home/iecas/rpc_test/nfs-ganesha/src /home/iecas/rpc_test/nfs-ganesha/src/libntirpc/tests /home/iecas/rpc_test/nfs-ganesha/src/libntirpc/tests/CMakeFiles/rpcping.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : libntirpc/tests/CMakeFiles/rpcping.dir/depend

