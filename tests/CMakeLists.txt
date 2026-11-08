include(FetchContent)


FetchContent_Declare(
	doctest
	GIT_REPOSITORY https://github.com/doctest/doctest.git
	GIT_TAG v2.4.12
)
set(DOCTEST_WITH_MAIN_IN_STATIC_LIB OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(doctest)

add_executable(projctl_tests
	parser_test.cpp
	remote_test.cpp
	config_loader_test.cpp
)

target_link_libraries(projctl_tests
	PRIVATE
		projctl_config
		projctl_parser
		doctest::doctest
)

add_test(NAME projctl_tests
	COMMAND projctl_tests
	--force-colors
)
