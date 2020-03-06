GIT_DIR=$(git rev-parse --show-toplevel)

/usr/local/opt/llvm/bin/clang++ \
    -Wall -Werror \
    -std=c++17 \
    -I/usr/local/opt/llvm/include \
    -L/usr/local/opt/llvm/lib \
    -lclang \
    -o $GIT_DIR/parser \
    $GIT_DIR/src/main.cpp
