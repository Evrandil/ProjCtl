default: build

configure:
    cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug

build: configure
    cmake --build build -- -k 0

test *args: configure
    cmake --build build --target projctl_tests
    ./build/tests/projctl_tests {{args}}

run *args: build
    ./build/projctl {{args}}

release:
    cmake -S . -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release -DPROJCTL_BUILD_TESTS=OFF
    cmake --build build-release

install: release
	sudo install -m 755 -s build-release/projctl /usr/local/bin/projctl
	sudo install -Dm644 completions/projctl.fish /usr/local/share/fish/vendor_completions.d/projctl.fish

clean:
    rm -rf build build-release
