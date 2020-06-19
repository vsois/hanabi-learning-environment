#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../hanabi_lib/hanabi_observation.h"
#include "representations.h"

namespace py = pybind11;
namespace hle = hanabi_learning_env;

void wrap_hanabi_observation(py::module& m) {
  py::class_<hle::HanabiObservation>(m, "HanabiObservation")
    .def(py::init<hle::HanabiState&, const int>(),
         py::arg("state"),
         py::arg("observing_player")
    )
    .def_property_readonly(
        "current_player_offset",
        &hle::HanabiObservation::CurPlayerOffset,
        "Offset of current player from observing player."
    )
    .def_property_readonly(
        "hands",
        &hle::HanabiObservation::Hands,
        "Observed hands are in relative order, with index 1 being the "
        "first player clock-wise from observing_player. hands[0][] has "
        "invalid cards as players don't see their own cards."
    )
    .def_property_readonly(
        "discard_pile",
        &hle::HanabiObservation::DiscardPile,
        "The element at the back is the most recent discard."
    )
    .def_property_readonly("fireworks", &hle::HanabiObservation::Fireworks) 
    .def_property_readonly("deck_size", &hle::HanabiObservation::DeckSize)
    .def_property_readonly(
        "parent_game",
        &hle::HanabiObservation::ParentGameRef
    )
    .def_property_readonly(
        "last_moves",
        &hle::HanabiObservation::LastMoves,
        "Moves made since observing_player's last action, most recent to oldest "
        "(that is, last_moves[0] is the most recent move.) "
        "Move targets are relative to observing_player not acting_player. "
        "Note that the deal moves are included in this vector."
    )
    .def_property_readonly(
        "information_tokens",
        &hle::HanabiObservation::InformationTokens)
    .def_property_readonly("life_tokens", &hle::HanabiObservation::LifeTokens)
    .def_property_readonly("legal_moves", &hle::HanabiObservation::LegalMoves)
    .def("card_playable_on_fireworks",
         (bool (hle::HanabiObservation::*) (int, int) const)
            &hle::HanabiObservation::CardPlayableOnFireworks,
         "Can a card with color and rank be played on fireworks pile?"
    )
    .def("card_playable_on_fireworks",
         (bool (hle::HanabiObservation::*) (hle::HanabiCard) const)
            &hle::HanabiObservation::CardPlayableOnFireworks,
         "Can the card be played on fireworks pile?"
    )
    .def("__str__", &hle::HanabiObservation::ToString)
    .def("__repr__", &hanabi_observation_repr)
    .doc() = "Agent observation of a HanabiState.";
}
