GIT_DIR=$(git rev-parse --show-toplevel)

/usr/local/opt/llvm/bin/clang++ \
    -Wall -Werror \
    -std=c++17 \
    -isystem $GIT_DIR/external \
    -isystem /usr/local/opt/llvm/include \
    -L/usr/local/opt/llvm/lib \
    -lclang \
    -o $GIT_DIR/bindgen \
    $GIT_DIR/src/main.cpp
