#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../hanabi_lib/hanabi_hand.h"

namespace py = pybind11;

void wrap_hanabi_hand(py::module& m) {
  py::class_<hanabi_learning_env::HanabiHand> (m, "HanabiHand")
    .def(py::init())
    .def(py::init<const hanabi_learning_env::HanabiHand&>())
    .def(py::init<const hanabi_learning_env::HanabiHand&, bool, bool>())
    .def("Cards", &hanabi_learning_env::HanabiHand::Cards)
    .def("Knowledge", &hanabi_learning_env::HanabiHand::Knowledge)
    .def("AddCard", &hanabi_learning_env::HanabiHand::AddCard)
    .def("RemoveFromHand", &hanabi_learning_env::HanabiHand::RemoveFromHand)
    .def("RevealRank", &hanabi_learning_env::HanabiHand::RevealRank)
    .def("RevealColor", &hanabi_learning_env::HanabiHand::RevealColor)

    .def("__str__", &hanabi_learning_env::HanabiHand::ToString);

  py::class_<hanabi_learning_env::HanabiHand::ValueKnowledge> (m, "ValueKnowledge")
    .def(py::init<int>())
    .def("Range", &hanabi_learning_env::HanabiHand::ValueKnowledge::Range)
    .def("ValueHinted", &hanabi_learning_env::HanabiHand::ValueKnowledge::ValueHinted)
    .def("Value", &hanabi_learning_env::HanabiHand::ValueKnowledge::Value)
    .def("IsPlausible", &hanabi_learning_env::HanabiHand::ValueKnowledge::IsPlausible)
    .def("ApplyIsValueHint", &hanabi_learning_env::HanabiHand::ValueKnowledge::ApplyIsValueHint)
    .def("ApplyIsNotValueHint", &hanabi_learning_env::HanabiHand::ValueKnowledge::ApplyIsNotValueHint);

  py::class_<hanabi_learning_env::HanabiHand::CardKnowledge> (m, "CardKnowledge")
    .def(py::init<int, int>())
    .def("NumColors", &hanabi_learning_env::HanabiHand::CardKnowledge::NumColors)
    .def("ColorHinted", &hanabi_learning_env::HanabiHand::CardKnowledge::ColorHinted)
    .def("Color", &hanabi_learning_env::HanabiHand::CardKnowledge::Color)
    .def("ColorPlausible", &hanabi_learning_env::HanabiHand::CardKnowledge::ColorPlausible)
    .def("ApplyIsColorHint", &hanabi_learning_env::HanabiHand::CardKnowledge::ApplyIsColorHint)
    .def("ApplyIsNotColorHint", &hanabi_learning_env::HanabiHand::CardKnowledge::ApplyIsNotColorHint)
    .def("NumRanks", &hanabi_learning_env::HanabiHand::CardKnowledge::NumRanks)
    .def("RankHinted", &hanabi_learning_env::HanabiHand::CardKnowledge::RankHinted)
    .def("Rank", &hanabi_learning_env::HanabiHand::CardKnowledge::Rank)
    .def("RankPlausible", &hanabi_learning_env::HanabiHand::CardKnowledge::RankPlausible)
    .def("ApplyIsRankHint", &hanabi_learning_env::HanabiHand::CardKnowledge::ApplyIsRankHint)
    .def("ApplyIsNotRankHint", &hanabi_learning_env::HanabiHand::CardKnowledge::ApplyIsNotRankHint)
    .def("__str__", &hanabi_learning_env::HanabiHand::CardKnowledge::ToString);

}
