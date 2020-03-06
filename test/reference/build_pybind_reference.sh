GIT_DIR=$(git rev-parse --show-toplevel)

mkdir -p $GIT_DIR/test/BUILD

c++ -Wall \
    -Werror \
    -shared \
    -undefined dynamic_lookup \
    -std=c++17 \
    -fPIC \
    `python3 -m pybind11 --includes` \
    -o $GIT_DIR/test/BUILD/pybind_reference_pkg`python3-config --extension-suffix` \
    $GIT_DIR/test/reference/pybind_reference.cpp
