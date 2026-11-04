default: build

configure:
    cmake -S . -B build -G Ninja

build: configure
    cmake --build build

test *args: build
	./build/tests/projctl_tests {{args}}
#    ctest --test-dir build --output-on-failure

run *args: build
    ./build/projctl {{args}}

clean:
    rm -rf build
