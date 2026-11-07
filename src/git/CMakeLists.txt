
add_library(projctl_git
	branch.cpp
	changes.cpp
	commit.cpp
	diff.cpp
	fetch.cpp
	host.cpp
	is_repo.cpp
	log.cpp
	merge.cpp
	pull.cpp
	push.cpp
	remote.cpp
)


target_include_directories(projctl_git PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(projctl_git
	PUBLIC
		projctl_system
	PRIVATE
		projctl_parser
		projctl_warnings
)
