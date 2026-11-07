
add_library(projctl_remote
	remote_url.cpp
)

target_include_directories(projctl_remote
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(projctl_remote
	PUBLIC
		projctl_system
	PRIVATE
		projctl_warnings
)
