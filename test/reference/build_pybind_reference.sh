mkdir -p ../BUILD

c++ -Wall -Werror -shared -undefined dynamic_lookup -std=c++17 -fPIC \
    `python3 -m pybind11 --includes` pybind_reference.cpp \
    -o ../BUILD/pybind_reference_pkg`python3-config --extension-suffix`
