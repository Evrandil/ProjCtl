
add_library(projctl_github
	githubinterface.cpp
	issues.cpp
	requests.cpp
)

target_include_directories(projctl_github
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(projctl_github
	PRIVATE
		projctl_system
		projctl_warnings
		nlohmann_json::nlohmann_json
)
