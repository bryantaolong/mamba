.PHONY: build clean run

build: build/CMakeCache.txt
	cmake --build build

build/CMakeCache.txt: CMakeLists.txt
	cmake -B build

clean:
	rm -rf build

run: build
	./build/mamba version