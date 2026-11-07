
add_library(projctl_core
	helper.cpp
)

add_subdirectory(core)
add_subdirectory(gh)
add_subdirectory(git)
add_subdirectory(issues)
add_subdirectory(projects)
add_subdirectory(requests)

target_include_directories(projctl_core
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(projctl_core
	PUBLIC
		projctl_parser
	PRIVATE
		projctl_config
		projctl_git
		projctl_githost
		projctl_warnings
)
