# Testing SWE Solvers

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
	# where <Pattern> is *Comparisons, for example
	./test/unit_tests --gtest_filter=<Pattern>
	```

