
add_library(projctl_githost
	githostinterface.cpp
	issues.cpp
	requests.cpp
)

target_include_directories(projctl_githost
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(projctl_githost
	PRIVATE
		projctl_git
		projctl_github
		projctl_gitlab
		projctl_parser
		projctl_warnings
)
