default: build

configure:
    cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug

build: configure
    cmake --build build -- -k 0

run *args: build
    ./build/projctl {{args}}


format:
    git ls-files '*.cpp' '*.hpp' | xargs clang-format -i

format-check:
    git ls-files '*.cpp' '*.hpp' | xargs clang-format --dry-run --Werror

tidy:
    cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    run-clang-tidy -p build -quiet '{{justfile_directory()}}/src/'

includes:
    run-clang-tidy -p build -checks='-*,misc-include-cleaner' -fix '{{justfile_directory()}}/src/'
    just format

test *args: configure
    cmake --build build --target projctl_tests
    ./build/tests/projctl_tests {{args}}


profile:
    cmake -S . -B build-profile -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_CXX_FLAGS="-fno-omit-frame-pointer" -DPROJCTL_BUILD_TESTS=OFF
    cmake --build build-profile

perf-quick *args: profile
	perf stat build-profile/projctl {{args}}

perf-record *args: profile
	perf record -g build-profile/projctl {{args}}

perf-report *args: perf-record {{args}}
	perf report


release:
    cmake -S . -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release -DPROJCTL_BUILD_TESTS=OFF
    cmake --build build-release

install: release
	sudo install -m 755 -s build-release/projctl /usr/local/bin/projctl
	sudo install -Dm644 completions/projctl.fish /usr/local/share/fish/vendor_completions.d/projctl.fish

clean:
    rm -rf build build-release
