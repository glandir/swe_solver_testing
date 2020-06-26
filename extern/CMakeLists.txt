add_library(swe_solvers)
target_sources(swe_solvers
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/swe.hpp
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/extern/AugRie.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/extern/WavePropagation.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/samoa/c_bind_riemannsolvers.f90
		${CMAKE_CURRENT_SOURCE_DIR}/samoa/riemannsolvers.f90
		${CMAKE_CURRENT_SOURCE_DIR}/samoa/riemannsolvers_sp.f90
	)
target_include_directories(swe_solvers
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}
	)
target_compile_features(swe_solvers PUBLIC cxx_std_17)

add_subdirectory(googletest)
