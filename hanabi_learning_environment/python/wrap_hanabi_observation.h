#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../hanabi_lib/hanabi_observation.h"

namespace py = pybind11;

void wrap_hanabi_observation(py::module& m) {
  py::class_<hanabi_learning_env::HanabiObservation>(m, "HanabiObservation")
    .def(py::init<hanabi_learning_env::HanabiState&, const int>())
    .def("CurPlayerOffset", &hanabi_learning_env::HanabiObservation::CurPlayerOffset)
    .def("Hands", &hanabi_learning_env::HanabiObservation::Hands)
    .def("DiscardPile", &hanabi_learning_env::HanabiObservation::DiscardPile)
    .def("Fireworks", &hanabi_learning_env::HanabiObservation::Fireworks) 
    .def("DeckSize", &hanabi_learning_env::HanabiObservation::DeckSize)
    .def("ParentGame", &hanabi_learning_env::HanabiObservation::ParentGame) 
    .def("LastMoves", &hanabi_learning_env::HanabiObservation::LastMoves)
    .def("InformationTokens", &hanabi_learning_env::HanabiObservation::InformationTokens)
    .def("LifeTokens", &hanabi_learning_env::HanabiObservation::LifeTokens)
    .def("LegalMoves", &hanabi_learning_env::HanabiObservation::LegalMoves)
    .def("CardPlayableOnFireworks",
        (bool (hanabi_learning_env::HanabiObservation::*) (int, int) const) &hanabi_learning_env::HanabiObservation::CardPlayableOnFireworks)
    .def("CardPlayableOnFireworks",
        (bool (hanabi_learning_env::HanabiObservation::*) (hanabi_learning_env::HanabiCard) const) &hanabi_learning_env::HanabiObservation::CardPlayableOnFireworks)
    .def("__str__", &hanabi_learning_env::HanabiObservation::ToString)
    .def("__repr__", &hanabi_learning_env::HanabiObservation::ToString);
}
