GIT_DIR=$(git rev-parse --show-toplevel)

mkdir -p $GIT_DIR/test/BUILD

$GIT_DIR/parser -std=c++17 $GIT_DIR/test/test_code/header.hpp \
    | tee $GIT_DIR/test/BUILD/generated_pybind.cpp
