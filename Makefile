.PHONY: all clean build run pack

all: build

build:
	cmake -S . -B build
	cmake --build build

clean:
	rm -rf build

run: build
	./build/pack

pack: build
	cd build && cpack
