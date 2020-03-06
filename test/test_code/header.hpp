#pragma once

namespace all
{
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

    int d() { return priv_d; }

private:
    int priv_d;
};
