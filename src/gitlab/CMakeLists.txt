
add_library(projctl_gitlab
	gitlabinterface.cpp
)

target_include_directories(projctl_gitlab
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(projctl_gitlab
	PUBLIC
		projctl_system
	PRIVATE
		projctl_warnings
		nlohmann_json::nlohmann_json
)
