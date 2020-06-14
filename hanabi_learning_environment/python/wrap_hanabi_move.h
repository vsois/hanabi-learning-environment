#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../hanabi_lib/hanabi_move.h"

namespace py = pybind11;

void wrap_hanabi_move(py::module& m) {
  py::class_<hanabi_learning_env::HanabiMove> hanabi_move(m, "HanabiMove");
  hanabi_move
    .def(py::init<hanabi_learning_env::HanabiMove::Type, int8_t, int8_t, int8_t, int8_t>())
    .def("__eq__", &hanabi_learning_env::HanabiMove::operator==)
    .def("MoveType", &hanabi_learning_env::HanabiMove::MoveType)
    .def("IsValid", &hanabi_learning_env::HanabiMove::IsValid)
    .def("CardIndex", &hanabi_learning_env::HanabiMove::CardIndex)
    .def("TargetOffset", &hanabi_learning_env::HanabiMove::TargetOffset)
    .def("Color", &hanabi_learning_env::HanabiMove::Color)
    .def("Rank", &hanabi_learning_env::HanabiMove::Rank)
    .def("__str__", &hanabi_learning_env::HanabiMove::ToString);

  py::enum_<hanabi_learning_env::HanabiMove::Type>(hanabi_move, "Type")
    .value("kInvalid", hanabi_learning_env::HanabiMove::Type::kInvalid)
    .value("kPlay", hanabi_learning_env::HanabiMove::Type::kPlay)
    .value("kDiscard", hanabi_learning_env::HanabiMove::Type::kDiscard)
    .value("kRevealColor", hanabi_learning_env::HanabiMove::Type::kRevealColor)
    .value("kRevealRank", hanabi_learning_env::HanabiMove::Type::kRevealRank)
    .value("kDeal", hanabi_learning_env::HanabiMove::Type::kDeal);
}
