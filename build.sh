/usr/local/opt/llvm/bin/clang++ \
    -Wall -Werror \
    -std=c++17 \
    -I/usr/local/opt/llvm/include \
    -L/usr/local/opt/llvm/lib \
    -lclang \
    -o parser \
    src/main.cpp
