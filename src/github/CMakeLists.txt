
add_library(projctl_github
	githubinterface.cpp
)

target_include_directories(projctl_github
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(projctl_github
	PUBLIC
		projctl_system
	PRIVATE
		projctl_warnings
)
