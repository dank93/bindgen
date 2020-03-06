GIT_DIR=$(git rev-parse --show-toplevel)

c++ -Wall \
    -Werror \
    -shared \
    -undefined dynamic_lookup \
    -std=c++17 \
    -fPIC \
    -I \\ \
    `python3 -m pybind11 --includes` \
    -o $GIT_DIR/test/BUILD/generated_pybind`python3-config --extension-suffix` \
    $GIT_DIR/test/BUILD/generated_pybind.cpp
