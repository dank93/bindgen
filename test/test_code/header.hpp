#pragma once

#include <array>
#include <string>
#include <vector>

namespace all
{
template<typename T, size_t S>
struct TemplatedArray
{
    std::array<T, S> std_arr;
    T c_arr[S];
};

namespace aa
{
struct A
{
    int i;
    float f;
    char c;
};
}

namespace bb
{
namespace ee
{
enum class TestEnum
{
    TE0,
    TE1,
    TE2,
};
}

struct B
{
    char c;
    aa::A a;
    double d;
    all::aa::A* a2;
    ee::TestEnum test_enum;
};

struct Empty {};
}
}

/// ~~BIND~~
struct C
{
    struct CC {};
    all::aa::A a;
    all::bb::B b;
    all::bb::Empty e;
};

/// ~~BIND~~
struct D
{
    C cc;
    std::string s;
    std::array<int, 10> int_array;
    std::vector<double> double_vec;
    all::TemplatedArray<char, 3> char_temp;
    C c_arr[4];

    int d() { return priv_d; }

private:
    int priv_d;
};
