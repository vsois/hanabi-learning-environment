#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../hanabi_lib/hanabi_observation.h"

namespace py = pybind11;

void wrap_hanabi_observation(py::module& m) {
  py::class_<hanabi_learning_env::HanabiObservation>(m, "HanabiObservation")
    .def(py::init<hanabi_learning_env::HanabiState&, const int>())
    .def("cur_player_offset", &hanabi_learning_env::HanabiObservation::CurPlayerOffset)
    .def("hands", &hanabi_learning_env::HanabiObservation::Hands)
    .def("discard_pile", &hanabi_learning_env::HanabiObservation::DiscardPile)
    .def("fireworks", &hanabi_learning_env::HanabiObservation::Fireworks) 
    .def("deck_size", &hanabi_learning_env::HanabiObservation::DeckSize)
    .def("get_game", &hanabi_learning_env::HanabiObservation::GetGame) 
    .def("last_moves", &hanabi_learning_env::HanabiObservation::LastMoves)
    .def("information_tokens", &hanabi_learning_env::HanabiObservation::InformationTokens)
    .def("life_tokens", &hanabi_learning_env::HanabiObservation::LifeTokens)
    .def("legal_moves", &hanabi_learning_env::HanabiObservation::LegalMoves)
    .def("card_playable_on_fireworks",
        (bool (hanabi_learning_env::HanabiObservation::*) (int, int) const) &hanabi_learning_env::HanabiObservation::CardPlayableOnFireworks)
    .def("card_playable_on_fireworks",
        (bool (hanabi_learning_env::HanabiObservation::*) (hanabi_learning_env::HanabiCard) const) &hanabi_learning_env::HanabiObservation::CardPlayableOnFireworks)
    .def("__str__", &hanabi_learning_env::HanabiObservation::ToString)
    .def("__repr__", &hanabi_learning_env::HanabiObservation::ToString);
}
