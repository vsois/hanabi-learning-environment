#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../hanabi_lib/hanabi_observation.h"

namespace py = pybind11;

void wrap_hanabi_observation(py::module& m) {
  py::class_<hanabi_learning_env::HanabiObservation>(m, "HanabiObservation")
    .def(py::init<hanabi_learning_env::HanabiState&, const int>())
    .def_property_readonly("current_player_offset", &hanabi_learning_env::HanabiObservation::CurPlayerOffset)
    .def_property_readonly("hands", &hanabi_learning_env::HanabiObservation::Hands)
    .def_property_readonly("discard_pile", &hanabi_learning_env::HanabiObservation::DiscardPile)
    .def_property_readonly("fireworks", &hanabi_learning_env::HanabiObservation::Fireworks) 
    .def_property_readonly("deck_size", &hanabi_learning_env::HanabiObservation::DeckSize)
    .def_property_readonly("parent_game", &hanabi_learning_env::HanabiObservation::ParentGameRef) 
    .def_property_readonly("last_moves", &hanabi_learning_env::HanabiObservation::LastMoves)
    .def_property_readonly("information_tokens", &hanabi_learning_env::HanabiObservation::InformationTokens)
    .def_property_readonly("life_tokens", &hanabi_learning_env::HanabiObservation::LifeTokens)
    .def_property_readonly("legal_moves", &hanabi_learning_env::HanabiObservation::LegalMoves)
    .def("card_playable_on_fireworks",
        (bool (hanabi_learning_env::HanabiObservation::*) (int, int) const) &hanabi_learning_env::HanabiObservation::CardPlayableOnFireworks)
    .def("card_playable_on_fireworks",
        (bool (hanabi_learning_env::HanabiObservation::*) (hanabi_learning_env::HanabiCard) const) &hanabi_learning_env::HanabiObservation::CardPlayableOnFireworks)
    .def("__str__", &hanabi_learning_env::HanabiObservation::ToString)
    .def("__repr__",
         [](const hanabi_learning_env::HanabiObservation& o) {
            return "<HanabiObservation " + o.ToString() + ">";
         }
    );
}
