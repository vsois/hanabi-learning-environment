#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../hanabi_lib/hanabi_hand.h"

namespace py = pybind11;

void wrap_hanabi_hand(py::module& m) {
  py::class_<hanabi_learning_env::HanabiHand> (m, "HanabiHand")
    .def(py::init())
    .def(py::init<const hanabi_learning_env::HanabiHand&>())
    .def(py::init<const hanabi_learning_env::HanabiHand&, bool, bool>())
    .def_property_readonly("cards", &hanabi_learning_env::HanabiHand::Cards)
    .def_property_readonly("knowledge", &hanabi_learning_env::HanabiHand::Knowledge)
    .def("add_card", &hanabi_learning_env::HanabiHand::AddCard)
    .def("remove_from_hand", &hanabi_learning_env::HanabiHand::RemoveFromHand)
    .def("reveal_rank", &hanabi_learning_env::HanabiHand::RevealRank)
    .def("reveal_color", &hanabi_learning_env::HanabiHand::RevealColor)

    .def("__len__", [](const hanabi_learning_env::HanabiHand& h) {return h.Cards().size();})
    .def("__str__", &hanabi_learning_env::HanabiHand::ToString)
    .def("__repr__",
         [](const hanabi_learning_env::HanabiHand& h) {
            return "<HanabiHand " + h.ToString() + ">";
         }
    );

  py::class_<hanabi_learning_env::HanabiHand::ValueKnowledge> (m, "ValueKnowledge")
    .def(py::init<int>())
    .def_property_readonly("range", &hanabi_learning_env::HanabiHand::ValueKnowledge::Range)
    .def("value_hinted", &hanabi_learning_env::HanabiHand::ValueKnowledge::ValueHinted)
    .def_property_readonly("value", &hanabi_learning_env::HanabiHand::ValueKnowledge::Value)
    .def("is_plausible", &hanabi_learning_env::HanabiHand::ValueKnowledge::IsPlausible)
    .def("apply_is_valuehint", &hanabi_learning_env::HanabiHand::ValueKnowledge::ApplyIsValueHint)
    .def("apply_is_not_value_hint", &hanabi_learning_env::HanabiHand::ValueKnowledge::ApplyIsNotValueHint)
    .def("__repr__",
         [](const hanabi_learning_env::HanabiHand::ValueKnowledge& v) {
            return "<HanabiHand.ValueKnowledge " + std::to_string(v.Value()) + ">";
         }
    );

  py::class_<hanabi_learning_env::HanabiHand::CardKnowledge> (m, "CardKnowledge")
    .def(py::init<int, int>())
    .def_property_readonly("num_colors", &hanabi_learning_env::HanabiHand::CardKnowledge::NumColors)
    .def("color_hinted", &hanabi_learning_env::HanabiHand::CardKnowledge::ColorHinted)
    .def_property_readonly("color", &hanabi_learning_env::HanabiHand::CardKnowledge::Color)
    .def("color_plausible", &hanabi_learning_env::HanabiHand::CardKnowledge::ColorPlausible)
    .def("apply_is_color_hint", &hanabi_learning_env::HanabiHand::CardKnowledge::ApplyIsColorHint)
    .def("apply_is_not_color_hint", &hanabi_learning_env::HanabiHand::CardKnowledge::ApplyIsNotColorHint)
    .def_property_readonly("num_ranks", &hanabi_learning_env::HanabiHand::CardKnowledge::NumRanks)
    .def("rank_hinted", &hanabi_learning_env::HanabiHand::CardKnowledge::RankHinted)
    .def_property_readonly("rank", &hanabi_learning_env::HanabiHand::CardKnowledge::Rank)
    .def("rank_plausible", &hanabi_learning_env::HanabiHand::CardKnowledge::RankPlausible)
    .def("apply_is_rank_hint", &hanabi_learning_env::HanabiHand::CardKnowledge::ApplyIsRankHint)
    .def("apply_is_not_rank_hint", &hanabi_learning_env::HanabiHand::CardKnowledge::ApplyIsNotRankHint)
    .def("__str__", &hanabi_learning_env::HanabiHand::CardKnowledge::ToString)
    .def("__repr__",
         [](const hanabi_learning_env::HanabiHand::CardKnowledge& c) {
            return "<HanabiHand.CardKnowledge " + c.ToString() + ">";
         }
    );

}
