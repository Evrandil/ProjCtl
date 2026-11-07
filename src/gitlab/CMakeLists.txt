
add_library(projctl_gitlab
	gitlabinterface.cpp
	issues.cpp
	requests.cpp
)

target_include_directories(projctl_gitlab
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(projctl_gitlab
	PRIVATE
		projctl_system
		projctl_warnings
		nlohmann_json::nlohmann_json
)
