
add_library(projctl_parser
	parser.cpp
	remote_url.cpp
)

target_include_directories(projctl_parser
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(projctl_parser
	PRIVATE
		projctl_warnings
)
