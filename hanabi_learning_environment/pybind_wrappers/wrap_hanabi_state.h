#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../hanabi_lib/hanabi_state.h"

namespace py = pybind11;

void wrap_hanabi_state(py::module& m) {
  py::class_<hanabi_learning_env::HanabiState> hanabi_state(m, "HanabiState");
  hanabi_state
    .def(py::init<const hanabi_learning_env::HanabiGame*, int>())
    .def(py::init<const hanabi_learning_env::HanabiState&>())
    .def("MoveIsLegal", &hanabi_learning_env::HanabiState::MoveIsLegal)
    .def("ApplyMove", &hanabi_learning_env::HanabiState::ApplyMove)
    .def("LegalMoves", &hanabi_learning_env::HanabiState::LegalMoves)
    .def("CardPlayableOnFireworks",
         (bool (hanabi_learning_env::HanabiState::*) (int, int) const)
            &hanabi_learning_env::HanabiState::CardPlayableOnFireworks)
    .def("CardPlayableOnFireworks",
         (bool (hanabi_learning_env::HanabiState::*) (hanabi_learning_env::HanabiCard) const)
            &hanabi_learning_env::HanabiState::CardPlayableOnFireworks)
    .def("ChanceOutcomeIsLegal", &hanabi_learning_env::HanabiState::ChanceOutcomeIsLegal)
    .def("ChanceOutcomeProb", &hanabi_learning_env::HanabiState::ChanceOutcomeProb)
    .def("ApplyChanceOutcome", &hanabi_learning_env::HanabiState::ApplyChanceOutcome)
    .def("ApplyRandomChance", &hanabi_learning_env::HanabiState::ApplyRandomChance)
    .def("ChanceOutcomes", &hanabi_learning_env::HanabiState::ChanceOutcomes)
    .def("EndOfGameStatus", &hanabi_learning_env::HanabiState::EndOfGameStatus)
    .def("IsTerminal", &hanabi_learning_env::HanabiState::IsTerminal)
    .def("Score", &hanabi_learning_env::HanabiState::Score)
    .def("CurPlayer", &hanabi_learning_env::HanabiState::CurPlayer)
    .def("LifeTokens", &hanabi_learning_env::HanabiState::LifeTokens)
    .def("InformationTokens", &hanabi_learning_env::HanabiState::InformationTokens)
    .def("Hands", &hanabi_learning_env::HanabiState::Hands)
    .def("Fireworks", &hanabi_learning_env::HanabiState::Fireworks)
    .def("ParentGame", &hanabi_learning_env::HanabiState::ParentGame)
    .def("Deck", &hanabi_learning_env::HanabiState::Deck)
    .def("DiscardPile", &hanabi_learning_env::HanabiState::DiscardPile)
    .def("MoveHistory", &hanabi_learning_env::HanabiState::MoveHistory)
    
    .def("__str__", &hanabi_learning_env::HanabiState::ToString)
    .def("__repr__", &hanabi_learning_env::HanabiState::ToString);

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
