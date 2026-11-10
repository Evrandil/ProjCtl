
add_library(projctl_system
	systeminterface.cpp
	directions.cpp
	system_helper.cpp
)

target_include_directories(projctl_system
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(projctl_system
	PRIVATE
		projctl_warnings
)
