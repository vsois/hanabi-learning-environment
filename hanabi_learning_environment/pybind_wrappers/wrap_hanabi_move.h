#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../hanabi_lib/hanabi_move.h"

namespace py = pybind11;

void wrap_hanabi_move(py::module& m) {
  py::class_<hanabi_learning_env::HanabiMove> hanabi_move(m, "HanabiMove");
  hanabi_move
    .def(py::init<hanabi_learning_env::HanabiMove::Type, int8_t, int8_t, int8_t, int8_t>())
    .def(py::init<hanabi_learning_env::HanabiMove::Type, int8_t, int8_t, hanabi_learning_env::HanabiCard::ColorType, hanabi_learning_env::HanabiCard::RankType>())
    .def(py::init<hanabi_learning_env::HanabiMove::Type, int8_t, int8_t, hanabi_learning_env::HanabiCard>())
    .def("__eq__", &hanabi_learning_env::HanabiMove::operator==)
    .def_property_readonly("move_type", &hanabi_learning_env::HanabiMove::MoveType)
    .def("is_valid", &hanabi_learning_env::HanabiMove::IsValid)
    .def_property_readonly("card_index", &hanabi_learning_env::HanabiMove::CardIndex)
    .def_property_readonly("target_offset", &hanabi_learning_env::HanabiMove::TargetOffset)
    .def_property_readonly("color", &hanabi_learning_env::HanabiMove::Color)
    .def_property_readonly("rank", &hanabi_learning_env::HanabiMove::Rank)
    .def("__str__", &hanabi_learning_env::HanabiMove::ToString)
    .def("__repr__",
         [](const hanabi_learning_env::HanabiMove& m) {
            return "<HanabiMove " + m.ToString() + ">";
         }
    );

  py::enum_<hanabi_learning_env::HanabiMove::Type>(hanabi_move, "Type")
    .value("kInvalid", hanabi_learning_env::HanabiMove::Type::kInvalid)
    .value("kPlay", hanabi_learning_env::HanabiMove::Type::kPlay)
    .value("kDiscard", hanabi_learning_env::HanabiMove::Type::kDiscard)
    .value("kRevealColor", hanabi_learning_env::HanabiMove::Type::kRevealColor)
    .value("kRevealRank", hanabi_learning_env::HanabiMove::Type::kRevealRank)
    .value("kDeal", hanabi_learning_env::HanabiMove::Type::kDeal)
    .export_values();
}
