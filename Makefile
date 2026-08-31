.PHONY: build clean run test

build: build/CMakeCache.txt
	cmake --build build

build/CMakeCache.txt: CMakeLists.txt
	cmake -B build

clean:
	rm -rf build

run: build
	./build/mamba_tests version

test: build
	ctest --output-on-failure
