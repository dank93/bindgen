GIT_DIR=$(git rev-parse --show-toplevel)

mkdir -p $GIT_DIR/test/BUILD

$GIT_DIR/bindgen \
    -std=c++17 \
	-isystem /usr/local/include \
	-isystem /Library/Developer/CommandLineTools/usr/include/c++/v1 \
	-isystem /Library/Developer/CommandLineTools/usr/lib/clang/10.0.0/include \
	-isystem /Library/Developer/CommandLineTools/usr/include \
	-isystem /Library/Developer/CommandLineTools/SDKs/MacOSX10.14.sdk/usr/include \
	-isystem /Library/Developer/CommandLineTools/SDKs/MacOSX10.14.sdk/System/Library/Frameworks \
    $GIT_DIR/test/test_code/header.hpp \
    | tee $GIT_DIR/test/BUILD/generated_pybind.cpp
