
add_library(projctl_config
	config.cpp
)

target_include_directories(projctl_config
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(projctl_config
	PRIVATE
		projctl_warnings
)
