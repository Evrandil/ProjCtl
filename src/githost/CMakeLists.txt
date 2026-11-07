
add_library(projctl_githost
	githostinterface.cpp
)

target_include_directories(projctl_githost
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(projctl_githost
	PUBLIC
		projctl_git
		projctl_github
		projctl_gitlab
	PRIVATE
		projctl_warnings
)
