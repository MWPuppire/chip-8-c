cmake_minimum_required(VERSION 3.10)

project(chip-8 VERSION 0.1.0)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported)
if(ipo_supported AND CMAKE_BUILD_TYPE STREQUAL "Release")
	set_property(GLOBAL PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	message(STATUS "Interprocedural optimization not supported")
endif(ipo_supported AND CMAKE_BUILD_TYPE STREQUAL "Release")

string(APPEND CMAKE_C_FLAGS_DEBUG " -g -fsanitize=address ")
string(APPEND CMAKE_C_FLAGS_RELEASE " -O2 ")
string(APPEND CMAKE_C_FLAGS " -Wall -Wextra -Werror -fsigned-char ")

find_package(SDL2 REQUIRED)

add_library(chip8)
target_sources(chip8 PRIVATE src/cpu.c src/input.c src/memory.c src/opcodes.c
	src/registers.c src/save-state.c src/screen.c
	src/instructions/arithmetic.c src/instructions/assignment.c
	src/instructions/bitwise.c src/instructions/branches.c
	src/instructions/display.c src/instructions/jumps.c
	src/instructions/misc.c src/instructions/timers.c)
target_include_directories(chip8 PUBLIC include)
target_include_directories(chip8 PRIVATE src/include)

add_executable(chip8_run src/frontend.c)
add_dependencies(chip8_run chip8)
target_include_directories(chip8_run PRIVATE chip8 ${SDL2_INCLUDE_DIRS})
target_link_libraries(chip8_run chip8 ${SDL2_LIBRARIES})

add_executable(chip8_debug src/frontend.c)
add_dependencies(chip8_debug chip8)
target_compile_definitions(chip8_debug PRIVATE DEBUG_REPL)
target_include_directories(chip8_debug PRIVATE chip8 ${SDL2_INCLUDE_DIRS} ext/include)
target_link_libraries(chip8_debug chip8 ${SDL2_LIBRARIES} readline)
