#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../hanabi_lib/hanabi_card.h"

namespace py = pybind11;

void wrap_hanabi_card(py::module& m) {
  py::class_<hanabi_learning_env::HanabiCard> (m, "HanabiCard")
    .def(py::init<int, int>())
    .def(py::init())
    .def("__eq__", &hanabi_learning_env::HanabiCard::operator==)
    .def("IsValid()", &hanabi_learning_env::HanabiCard::IsValid)
    .def("Color", &hanabi_learning_env::HanabiCard::Color)
    .def("Rank", &hanabi_learning_env::HanabiCard::Rank)

    .def("__str__", &hanabi_learning_env::HanabiCard::ToString);
}
