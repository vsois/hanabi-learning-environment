#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../hanabi_lib/hanabi_game.h"
#include "representations.h"

namespace py = pybind11;
namespace hle = hanabi_learning_env;

void wrap_hanabi_game(py::module& m) {
  py::class_<hle::HanabiGame>(m, "HanabiGame")
    .def(py::init<const std::unordered_map<std::string, std::string>&>(),
         py::arg("params")
    )
	.def(py::pickle(
			// __getstate__
			[](const hle::HanabiGame &game) {
				// Return a tuple that fully encodes the state of the object
				return py::make_tuple(game.Parameters());
  	  	  	  },
			  // __setstate__
			  [](py::tuple t) {
  	  	  		  if (t.size() != 1)
  	  	  			  throw std::runtime_error("Invalid state!");

  	  	  		  // Create a new C++ instance
  	  	  		  hle::HanabiGame game(t[0].cast<std::unordered_map<std::string, std::string>>());

  	  	  		  return game;
  	  	  	  }
		)
	)
    .def_property_readonly("max_moves", &hle::HanabiGame::MaxMoves)
    .def("get_move",
         &hle::HanabiGame::GetMove,
         py::arg("uid"),
         "Get a HanabiMove by unique id."
    )
    .def("get_move_uid",
         (int (hle::HanabiGame::*) (hle::HanabiMove) const)
            &hle::HanabiGame::GetMoveUid,
         py::arg("move"),
         "Get unique id for a move. Returns -1 for invalid move."
    )
    .def("get_move_uid",
        (int (hle::HanabiGame::*) (hle::HanabiMove::Type, int, int, int, int) const)
            &hle::HanabiGame::GetMoveUid,
         py::arg("move_type"),
         py::arg("card_index"),
         py::arg("target_offset"),
         py::arg("color"),
         py::arg("rank"),
         "Get unique id for a move. Returns -1 for invalid move."
    )
    .def_property_readonly("max_chance_outcomes",
                           &hle::HanabiGame::MaxChanceOutcomes)
    .def("get_chance_outcome",
         &hle::HanabiGame::GetChanceOutcome,
         py::arg("uid"),
         "Get a chance-outcome HanabiMove by unique id."
    )
    .def("get_chance_outcome_uid",
         &hle::HanabiGame::GetChanceOutcomeUid,
         py::arg("move"),
         "Get unique id for a chance-outcome move. Returns -1 for invalid move."
    )
    .def("pick_random_chance",
         &hle::HanabiGame::PickRandomChance,
         py::arg("chance_outcomes"),
         "Randomly sample a random chance-outcome move from list of moves and "
         "associated probability distribution."
    )
    .def_property_readonly("parameters", &hle::HanabiGame::Parameters)
    .def_property_readonly("min_players", &hle::HanabiGame::MinPlayers)
    .def_property_readonly("max_players", &hle::HanabiGame::MaxPlayers)
    .def_property_readonly("min_score", &hle::HanabiGame::MinScore)
    .def_property_readonly("max_score", &hle::HanabiGame::MaxScore)
    .def_property_readonly("name", &hle::HanabiGame::Name)
    .def_property_readonly("num_colors", &hle::HanabiGame::NumColors)
    .def_property_readonly("num_ranks", &hle::HanabiGame::NumRanks)
    .def_property_readonly("num_players", &hle::HanabiGame::NumPlayers)
    .def_property_readonly("hand_size", &hle::HanabiGame::HandSize)
    .def_property_readonly("max_information_tokens",
                           &hle::HanabiGame::MaxInformationTokens)
    .def_property_readonly("max_life_tokens", &hle::HanabiGame::MaxLifeTokens)
    .def_property_readonly("cards_per_color", &hle::HanabiGame::CardsPerColor)
    .def_property_readonly("max_deck_size", &hle::HanabiGame::MaxDeckSize)
    .def("number_card_instances",
         (int (hle::HanabiGame::*) (int, int) const)
            &hle::HanabiGame::NumberCardInstances,
         py::arg("color"),
         py::arg("rank"),
         "Number of instances of the given card in the deck."
    )
    .def("number_card_instances",
         (int (hle::HanabiGame::*) (hle::HanabiCard) const)
            &hle::HanabiGame::NumberCardInstances,
         py::arg("card"),
         "Number of instances of the given card in the deck."
    )
    .def_property_readonly("observation_type",
                           &hle::HanabiGame::ObservationType)
    .def("get_sampled_start_player",
         &hle::HanabiGame::GetSampledStartPlayer,
         "Get the first player to act. "
         "Might be randomly generated at each call."
    )
    .def("__repr__", &hanabi_game_repr)

    .doc() = ""
    "An agent's observation of a state does include all state knowledge. "
    "For example, observations never include an agent's own cards. "
    "A kMinimal observation is similar to what a human sees, and does not "
    "include any memory of past RevalColor/RevealRank hints. A CardKnowledge "
    "observation includes per-card knowledge of past hints, as well as simple "
    "inferred knowledge of the form 'this card is not red, because it was "
    "not revealed as red in a past <RevealColor Red> move'. A Seer observation "
    "shows all cards, including the player's own cards, regardless of what "
    "hints have been given.";
}
