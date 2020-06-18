#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../hanabi_lib/hanabi_state.h"

namespace py = pybind11;

void wrap_hanabi_state(py::module& m) {
  py::class_<hanabi_learning_env::HanabiState> hanabi_state(m, "HanabiState");
  hanabi_state
    .def(py::init<const hanabi_learning_env::HanabiGame*, int>())
    .def(py::init<const hanabi_learning_env::HanabiState&>())
    .def("move_is_legal", &hanabi_learning_env::HanabiState::MoveIsLegal)
    .def("apply_move", &hanabi_learning_env::HanabiState::ApplyMove)
    .def("legal_moves", &hanabi_learning_env::HanabiState::LegalMoves)
    .def("card_playable_on_fireworks",
         (bool (hanabi_learning_env::HanabiState::*) (int, int) const)
            &hanabi_learning_env::HanabiState::CardPlayableOnFireworks)
    .def("card_playable_on_fireworks",
         (bool (hanabi_learning_env::HanabiState::*) (hanabi_learning_env::HanabiCard) const)
            &hanabi_learning_env::HanabiState::CardPlayableOnFireworks)
    .def("chance_outcome_is_legal", &hanabi_learning_env::HanabiState::ChanceOutcomeIsLegal)
    .def("chance_outcome_prob", &hanabi_learning_env::HanabiState::ChanceOutcomeProb)
    .def("apply_chance_outcome", &hanabi_learning_env::HanabiState::ApplyChanceOutcome)
    .def("apply_random_chance", &hanabi_learning_env::HanabiState::ApplyRandomChance)
    .def_property_readonly("chance_outcomes", &hanabi_learning_env::HanabiState::ChanceOutcomes)
    .def_property_readonly("end_of_game_status", &hanabi_learning_env::HanabiState::EndOfGameStatus)
    .def("is_terminal", &hanabi_learning_env::HanabiState::IsTerminal)
    .def_property_readonly("score", &hanabi_learning_env::HanabiState::Score)
    .def_property_readonly("current_player", &hanabi_learning_env::HanabiState::CurPlayer)
    .def_property_readonly("life_tokens", &hanabi_learning_env::HanabiState::LifeTokens)
    .def_property_readonly("information_tokens", &hanabi_learning_env::HanabiState::InformationTokens)
    .def_property_readonly("hands", &hanabi_learning_env::HanabiState::Hands)
    .def_property_readonly("fireworks", &hanabi_learning_env::HanabiState::Fireworks)
    // .def_property_readonly("parent_game", &hanabi_learning_env::HanabiState::ParentGame)
    .def_property_readonly("deck", &hanabi_learning_env::HanabiState::Deck)
    .def_property_readonly("discard_pile", &hanabi_learning_env::HanabiState::DiscardPile)
    .def_property_readonly("move_history", &hanabi_learning_env::HanabiState::MoveHistory)
    
    .def("__str__", &hanabi_learning_env::HanabiState::ToString)
    .def("__repr__", 
         [](const hanabi_learning_env::HanabiState& s) {
            return "<HanabiState " + s.ToString() + ">";
         }
    );

  py::enum_<hanabi_learning_env::HanabiState::EndOfGameType>(hanabi_state, "EndOfGameType")
    .value("kNotFinished", hanabi_learning_env::HanabiState::EndOfGameType::kNotFinished)
    .value("kOutOfLifeTokens", hanabi_learning_env::HanabiState::EndOfGameType::kOutOfLifeTokens)
    .value("kOutOfCards", hanabi_learning_env::HanabiState::EndOfGameType::kOutOfCards)
    .value("kCompletedFireworks", hanabi_learning_env::HanabiState::EndOfGameType::kCompletedFireworks)
    .export_values();
}

// void advertize_hanabi_deck(py::module& m) {
//   py::class_<hanabi_learning_env::HanabiState::HanabiDeck>(m, "HanabiDeck")
//     .def(py::init<hanabi_learning_env::HanabiGame&, const int>())
//     .def("DealCard",
// }
