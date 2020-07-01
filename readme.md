# Testing SWE Solvers

* Clone with

	```
	git clone https://github.com/glandir/swe_solver_testing
	cd swe_solver_testing
	# No need for ExaHyPE submodules; we don't build it.
	git submodule update --init extern
	```

* Check out relevant commit in `extern/ExaHyPE-Engine` (Current state is in luca_dbg)
* Run

	```
	mkdir -p build
	cd build
	cmake ..
	make

	# to run all tests
	make test

	# to run a specific test,
	# where <Pattern> is *Simple, for example
	./test/unit_tests --gtest_filter=<Pattern>
	```

