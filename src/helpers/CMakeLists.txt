
add_library(projctl_helpers
	text.cpp
	ini.cpp
	projecttype.cpp
)

target_include_directories(projctl_helpers
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(projctl_helpers
	PRIVATE
		projctl_warnings
)
