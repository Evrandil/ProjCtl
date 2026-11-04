default: build

configure:
    cmake -S . -B build -G Ninja

build: configure
    cmake --build build

test *args: configure
    cmake --build build --target projctl_tests
    ./build/tests/projctl_tests {{args}}

run *args: build
    ./build/projctl {{args}}

clean:
    rm -rf build
