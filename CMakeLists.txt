cmake_minimum_required(VERSION 3.20)

project(projctl
	VERSION 0.5.2
	LANGUAGES CXX
)

# Version header

configure_file(
	${PROJECT_SOURCE_DIR}/cmake/version.hpp.in
	${PROJECT_BINARY_DIR}/generated/version.hpp
	@ONLY
)

add_library(projctl_version INTERFACE)

target_include_directories(projctl_version
	INTERFACE
		${PROJECT_BINARY_DIR}/generated
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)


include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)

if(ipo_supported)
	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
else()
	message(WARNING "LTO not supported: ${ipo_error}")
endif()


# Json

find_package(nlohmann_json 3.12.0 QUIET)

if(NOT nlohmann_json_FOUND)
	include(FetchContent)

	FetchContent_Declare(
		nlohmann_json
		GIT_REPOSITORY https://github.com/nlohmann/json.git
		GIT_TAG v3.12.0
	)

	FetchContent_MakeAvailable(nlohmann_json)
endif()

# Compile flags

add_library(projctl_warnings INTERFACE)

target_compile_options(projctl_warnings INTERFACE
	-Wall
	-Wextra
	-Wpedantic
	-Wconversion
	-Wsign-conversion
	-Wshadow
)

# Sanitizers (Debug only)

target_compile_options(projctl_warnings INTERFACE
	$<$<CONFIG:Debug>:-fsanitize=address,undefined -fno-omit-frame-pointer>
)

target_link_options(projctl_warnings INTERFACE
	$<$<CONFIG:Debug>:-fsanitize=address,undefined>
)

# Libs

add_subdirectory(src/parser)
add_subdirectory(src/system)
add_subdirectory(src/config)
add_subdirectory(src/git)
add_subdirectory(src/github)
add_subdirectory(src/gitlab)
add_subdirectory(src/githost)
add_subdirectory(src/projctl)
add_subdirectory(src/helpers)

# Executable

add_executable(projctl
    src/main.cpp
)

target_link_libraries(projctl
	PRIVATE
		projctl_core
		projctl_warnings
)

# Security

include(CheckPIESupported)
check_pie_supported()
set_property(TARGET projctl PROPERTY POSITION_INDEPENDENT_CODE TRUE)


# Tests

option(PROJCTL_BUILD_TESTS "Build the test suite" ON)

if(PROJCTL_BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()
