default: build

configure:
    cmake -S . -B build -G Ninja

build: configure
    cmake --build build

test: build
	./build/tests/projctl_tests
#    ctest --test-dir build --output-on-failure

run *args: build
    ./build/projctl {{args}}

clean:
    rm -rf build
