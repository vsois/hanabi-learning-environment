#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../hanabi_lib/hanabi_game.h"

namespace py = pybind11;

void wrap_hanabi_game(py::module& m) {
  py::class_<hanabi_learning_env::HanabiGame> (m, "HnanabiGame")
    .def(py::init<const std::unordered_map<std::string, std::string>&>())
    .def("MaxMoves", &hanabi_learning_env::HanabiGame::MaxMoves)
    .def("GetMove", &hanabi_learning_env::HanabiGame::GetMove)
    .def("GetMoveUid",
        (int (hanabi_learning_env::HanabiGame::*) (hanabi_learning_env::HanabiMove) const)
        &hanabi_learning_env::HanabiGame::GetMoveUid)
    .def("GetMoveUid",
        (int (hanabi_learning_env::HanabiGame::*) (hanabi_learning_env::HanabiMove::Type, int, int, int, int) const)
        &hanabi_learning_env::HanabiGame::GetMoveUid)
    .def("MaxChanceOutcomes", &hanabi_learning_env::HanabiGame::MaxChanceOutcomes)
    .def("GetChanceOutcome", &hanabi_learning_env::HanabiGame::GetChanceOutcome)
    .def("GetChanceOutcomeUid", &hanabi_learning_env::HanabiGame::GetChanceOutcomeUid)
    .def("PickRandomChance", &hanabi_learning_env::HanabiGame::PickRandomChance)
    .def("Parameters", &hanabi_learning_env::HanabiGame::Parameters)
    .def("MinPlayers", &hanabi_learning_env::HanabiGame::MinPlayers)
    .def("MaxPlayers", &hanabi_learning_env::HanabiGame::MaxPlayers)
    .def("MinScore", &hanabi_learning_env::HanabiGame::MinScore)
    .def("MaxScore", &hanabi_learning_env::HanabiGame::MaxScore)
    .def("Name", &hanabi_learning_env::HanabiGame::Name)
    .def("NumColors", &hanabi_learning_env::HanabiGame::NumColors)
    .def("NumRanks", &hanabi_learning_env::HanabiGame::NumRanks)
    .def("NumPlayers", &hanabi_learning_env::HanabiGame::NumPlayers)
    .def("HandSize", &hanabi_learning_env::HanabiGame::HandSize)
    .def("MaxInformationTokens", &hanabi_learning_env::HanabiGame::MaxInformationTokens)
    .def("MaxLifeTokens", &hanabi_learning_env::HanabiGame::MaxLifeTokens)
    .def("CardsPerColor", &hanabi_learning_env::HanabiGame::CardsPerColor)
    .def("MaxDeckSize", &hanabi_learning_env::HanabiGame::MaxDeckSize)
    .def("NumberCardInstances",
        (int (hanabi_learning_env::HanabiGame::*) (int, int) const)
        &hanabi_learning_env::HanabiGame::NumberCardInstances)
    .def("NumberCardInstances",
        (int (hanabi_learning_env::HanabiGame::*) (hanabi_learning_env::HanabiCard) const)
        &hanabi_learning_env::HanabiGame::NumberCardInstances)
    .def("ObservationType", &hanabi_learning_env::HanabiGame::ObservationType)
    .def("GetSampledStartPlayer", &hanabi_learning_env::HanabiGame::GetSampledStartPlayer);
}
