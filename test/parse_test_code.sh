GIT_DIR=$(git rev-parse --show-toplevel)

$GIT_DIR/parser -std=c++17 $GIT_DIR/test/test_code/header.hpp
