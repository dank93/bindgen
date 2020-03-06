#include "../test_code/header.hpp"

#include <pybind11/pybind11.h>

#include <iostream>

namespace py = pybind11;

void print_A(const all::aa::A& a)
{
    std::cout << "A::i: " << a.i << std::endl;
    std::cout << "A::f: " << a.f << std::endl;
    std::cout << "A::c: " << a.c << std::endl;
}

void print_A_from_D(const D& d)
{
    print_A(d.cc.a);
}


PYBIND11_MODULE(pybind_reference_pkg, m)
{
    m.doc() = "Reference bindings";

    py::module all = m.def_submodule("all");
    py::module all_aa = all.def_submodule("aa");
    py::module all_bb = all.def_submodule("bb");
    py::module all_bb_ee = all_bb.def_submodule("ee");

	py::class_<all::aa::A>(all_aa, "A")
        .def(py::init<>())
	    .def_readwrite("i", &all::aa::A::i)
	    .def_readwrite("f", &all::aa::A::f)
        .def_readwrite("c", &all::aa::A::c);

	py::enum_<all::bb::ee::TestEnum>(all_bb_ee, "TestEnum")
        .value("TE0", all::bb::ee::TestEnum::TE0)
        .value("TE1", all::bb::ee::TestEnum::TE1)
        .value("TE2", all::bb::ee::TestEnum::TE2);

	py::class_<all::bb::B>(all_bb, "B")
        .def(py::init<>())
	    .def_readwrite("c", &all::bb::B::c)
	    .def_readwrite("a", &all::bb::B::a)
	    .def_readwrite("d", &all::bb::B::d)
	    .def_readwrite("a2", &all::bb::B::a2)
	    .def_readwrite("test_enum", &all::bb::B::test_enum);

	py::class_<all::bb::Empty>(all_bb, "Empty")
        .def(py::init<>());

	py::class_<C>(m, "C")
        .def(py::init<>())
	    .def_readwrite("a", &C::a)
	    .def_readwrite("b", &C::b)
	    .def_readwrite("e", &C::e);

	py::class_<D>(m, "D")
        .def(py::init<>())
	    .def_readwrite("cc", &D::cc);

    m.def("print_A", &print_A);
    m.def("print_A_from_D", &print_A_from_D);
}
