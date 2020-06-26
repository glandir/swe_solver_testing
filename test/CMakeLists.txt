add_executable(unit_tests)
target_sources(unit_tests
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/swe_solvers.cpp
	)
target_link_libraries(unit_tests
	PRIVATE
		swe_solvers
		gtest gtest_main gmock
	)

include(GoogleTest)
gtest_discover_tests(unit_tests)

