cmake_minimum_required(VERSION 3.20)

project(projctl
	VERSION 0.5.0
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)


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


# Parser (no dependencies on the rest of projctl)

add_library(projctl_parser
	src/parser.cpp
)

target_include_directories(projctl_parser
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
)

target_compile_options(projctl_parser PRIVATE
	-Wall
	-Wextra
	-Wpedantic
	-Wconversion
	-Wsign-conversion
	-Wshadow
)


# Projctl core

add_library(projctl_core
	src/projctl/core.cpp
	src/projctl/handle.cpp
	src/projctl/helper.cpp
	src/projctl/projects.cpp
	src/projctl/git.cpp
	src/projctl/hosting.cpp
	src/projctl/issues.cpp
	src/projctl/requests.cpp
	src/systeminterface.cpp
	src/gitinterface/branch.cpp
	src/gitinterface/changes.cpp
	src/gitinterface/commit.cpp
	src/gitinterface/fetch.cpp
	src/gitinterface/host.cpp
	src/gitinterface/is_repo.cpp
	src/gitinterface/merge.cpp
	src/gitinterface/pull.cpp
	src/gitinterface/push.cpp
	src/gitinterface/remote.cpp
	src/githostinterface.cpp
	src/githubinterface.cpp
	src/gitlabinterface.cpp
	src/config.cpp
)

target_include_directories(projctl_core
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
)

target_compile_options(projctl_core PRIVATE
	-Wall
	-Wextra
	-Wpedantic
	-Wconversion
	-Wsign-conversion
	-Wshadow
#	-O3
)

target_link_libraries(projctl_core
	PRIVATE
		nlohmann_json::nlohmann_json
)

target_link_libraries(projctl_core
	PUBLIC
		projctl_parser
	PRIVATE
		nlohmann_json::nlohmann_json
)

# Executable

add_executable(projctl
    src/main.cpp
)

target_link_libraries(projctl
	PRIVATE
		projctl_core
)

target_compile_options(projctl PRIVATE
	-Wall
	-Wextra
	-Wpedantic
	-Wshadow
	-Wconversion
	-Wsign-conversion
#	-O3
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
