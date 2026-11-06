include(FetchContent)


FetchContent_Declare(
	doctest
	GIT_REPOSITORY https://github.com/doctest/doctest.git
	GIT_TAG v2.4.12
)
FetchContent_MakeAvailable(doctest)

add_executable(projctl_tests
	parser_test.cpp
	remote_test.cpp
)

target_link_libraries(projctl_tests
	PRIVATE
		projctl_parser
		doctest::doctest
)

add_test(NAME projctl_tests
	COMMAND projctl_tests
	--force-colors
)
