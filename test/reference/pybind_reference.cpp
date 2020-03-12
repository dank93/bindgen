#include "header.hpp"

#include <array>
#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(pybind_reference_pkg, _module_)
{
	py::module all__ = _module_.def_submodule("all");
	py::module all__bb__ = all__.def_submodule("bb");
	py::module all__bb__ee__ = all__bb__.def_submodule("ee");
	py::module all__aa__ = all__.def_submodule("aa");

	py::class_<all::TemplatedArray<char, 3>>(all__, "_TemplatedArray_char__3_")
		.def(py::init<>())
		.def_readwrite("std_arr", &all::TemplatedArray<char, 3>::std_arr)
        .def_property("c_arr",
            [](const all::TemplatedArray<char, 3>& self) {
                std::array<char, 3> out;
                std::copy(std::begin(self.c_arr),
                          std::end(self.c_arr),
                          std::begin(out));
                return out;
            },
            [](all::TemplatedArray<char, 3>& self, std::array<char, 4>& in) {
                std::copy(std::begin(in),
                          std::end(in),
                          std::begin(self.c_arr));
            });

	py::class_<all::bb::Empty>(all__bb__, "Empty")
		.def(py::init<>());

	py::class_<all::bb::B>(all__bb__, "B")
		.def(py::init<>())
		.def_readwrite("c", &all::bb::B::c)
		.def_readwrite("a", &all::bb::B::a)
		.def_readwrite("d", &all::bb::B::d)
		.def_readwrite("a2", &all::bb::B::a2)
		.def_readwrite("test_enum", &all::bb::B::test_enum);

	py::enum_<all::bb::ee::TestEnum>(all__bb__ee__, "TestEnum")
		.value("TE0", all::bb::ee::TestEnum::TE0)
		.value("TE1", all::bb::ee::TestEnum::TE1)
		.value("TE2", all::bb::ee::TestEnum::TE2);

	py::class_<all::aa::A>(all__aa__, "A")
		.def(py::init<>())
		.def_readwrite("i", &all::aa::A::i)
		.def_readwrite("f", &all::aa::A::f)
		.def_readwrite("c", &all::aa::A::c);

	py::class_<D>(_module_, "D")
		.def(py::init<>())
		.def_readwrite("cc", &D::cc)
		.def_readwrite("s", &D::s)
		.def_readwrite("int_array", &D::int_array)
		.def_readwrite("double_vec", &D::double_vec)
		.def_readwrite("char_temp", &D::char_temp)
        .def_property("c_arr",
            [](const D& self) {
                std::array<C,4> out;
                std::copy(std::begin(self.c_arr),
                          std::end(self.c_arr),
                          std::begin(out));
                return out;
            },
            [](D& self, std::array<C, 4>& in) {
                std::copy(std::begin(in),
                          std::end(in),
                          std::begin(self.c_arr));
            });

	py::class_<C>(_module_, "C")
		.def(py::init<>())
		.def_readwrite("a", &C::a)
		.def_readwrite("b", &C::b)
		.def_readwrite("e", &C::e);

}
