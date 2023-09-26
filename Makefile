all: cbuild
cbuild:
	cmake -S . -B build && cmake --build build
test:
	cd build && ctest -V && cd ../t
