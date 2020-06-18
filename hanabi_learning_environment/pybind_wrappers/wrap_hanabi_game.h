#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../hanabi_lib/hanabi_game.h"

namespace py = pybind11;

void wrap_hanabi_game(py::module& m) {
  py::class_<hanabi_learning_env::HanabiGame>(m, "HanabiGame")
    .def(py::init<const std::unordered_map<std::string, std::string>&>())
    .def_property_readonly("max_moves", &hanabi_learning_env::HanabiGame::MaxMoves)
    .def("get_move", &hanabi_learning_env::HanabiGame::GetMove)
    .def("get_move_uid",
        (int (hanabi_learning_env::HanabiGame::*) (hanabi_learning_env::HanabiMove) const)
        &hanabi_learning_env::HanabiGame::GetMoveUid)
    .def("get_move_uid",
        (int (hanabi_learning_env::HanabiGame::*) (hanabi_learning_env::HanabiMove::Type, int, int, int, int) const)
        &hanabi_learning_env::HanabiGame::GetMoveUid)
    .def_property_readonly("max_chance_outcomes", &hanabi_learning_env::HanabiGame::MaxChanceOutcomes)
    .def("get_chance_outcome", &hanabi_learning_env::HanabiGame::GetChanceOutcome)
    .def("get_chance_outcome_uid", &hanabi_learning_env::HanabiGame::GetChanceOutcomeUid)
    .def("pick_random_chance", &hanabi_learning_env::HanabiGame::PickRandomChance)
    .def_property_readonly("parameters", &hanabi_learning_env::HanabiGame::Parameters)
    .def_property_readonly("min_players", &hanabi_learning_env::HanabiGame::MinPlayers)
    .def_property_readonly("max_players", &hanabi_learning_env::HanabiGame::MaxPlayers)
    .def_property_readonly("min_score", &hanabi_learning_env::HanabiGame::MinScore)
    .def_property_readonly("max_score", &hanabi_learning_env::HanabiGame::MaxScore)
    .def_property_readonly("name", &hanabi_learning_env::HanabiGame::Name)
    .def_property_readonly("num_colors", &hanabi_learning_env::HanabiGame::NumColors)
    .def_property_readonly("num_ranks", &hanabi_learning_env::HanabiGame::NumRanks)
    .def_property_readonly("num_players", &hanabi_learning_env::HanabiGame::NumPlayers)
    .def_property_readonly("hand_size", &hanabi_learning_env::HanabiGame::HandSize)
    .def_property_readonly("max_information_tokens", &hanabi_learning_env::HanabiGame::MaxInformationTokens)
    .def_property_readonly("max_life_tokens", &hanabi_learning_env::HanabiGame::MaxLifeTokens)
    .def_property_readonly("cards_per_color", &hanabi_learning_env::HanabiGame::CardsPerColor)
    .def_property_readonly("max_deck_size", &hanabi_learning_env::HanabiGame::MaxDeckSize)
    .def("number_card_instances",
        (int (hanabi_learning_env::HanabiGame::*) (int, int) const)
        &hanabi_learning_env::HanabiGame::NumberCardInstances)
    .def("number_card_instances",
        (int (hanabi_learning_env::HanabiGame::*) (hanabi_learning_env::HanabiCard) const)
        &hanabi_learning_env::HanabiGame::NumberCardInstances)
    .def_property_readonly("observation_type", &hanabi_learning_env::HanabiGame::ObservationType)
    .def("get_samped_start_player", &hanabi_learning_env::HanabiGame::GetSampledStartPlayer)
    .def("__repr__",
         [](const hanabi_learning_env::HanabiGame& g) {
            return "<HanabiGame >";
         }
    );
}
