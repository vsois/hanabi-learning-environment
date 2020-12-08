#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../hanabi_lib/hanabi_state.h"
#include "representations.h"

namespace py = pybind11;
namespace hle = hanabi_learning_env;

void wrap_hanabi_state(py::module& m) {
  py::class_<hle::HanabiState> hanabi_state(m, "HanabiState");
  hanabi_state
     .def(py::init<const hle::HanabiGame*, int>(),
          py::arg("parent_game"),
          py::arg("start_player") = -1
     )
    .def(py::init<const hle::HanabiState&>(), py::arg("state"))
	.def(py::pickle(
			// __getstate__
			[](const hle::HanabiState &state) {
				// Return a tuple that fully encodes the state of the object
				return py::make_tuple(state.ParentGame(), state.MoveHistory());
  	  	  	  },
			  // __setstate__
			  [](py::tuple t) {
  	  	  		  if (t.size() != 2)
  	  	  			  throw std::runtime_error("Invalid state!");

  	  	  		  // Create a new C++ instance
  	  	  		  hle::HanabiState state(t[0].cast<hle::HanabiGame*>());
  	  	  		  std::vector<hle::HanabiHistoryItem> move_history = t[1].cast<std::vector<hle::HanabiHistoryItem>>();

  	  	  		  for (const hle::HanabiHistoryItem& item : move_history) {
  	  	  			  state.ApplyMove(item.move);
  	  	  		  }

  	  	  		  return state;
  	  	  	  }
		)
	)
    .def("move_is_legal", &hle::HanabiState::MoveIsLegal, py::arg("move"))
    .def("apply_move", &hle::HanabiState::ApplyMove, py::arg("move"))
    .def("legal_moves",
         &hle::HanabiState::LegalMoves,
         py::arg("player"),
         "Legal moves for state. Moves point into an unchanging list "
         "in parent_game."
    )
    .def("card_playable_on_fireworks",
         (bool (hle::HanabiState::*) (int, int) const)
            &hle::HanabiState::CardPlayableOnFireworks,
         py::arg("color"),
         py::arg("rank"),
         "Returns true if card with color and rank can be played on fireworks pile."
    )
    .def("card_playable_on_fireworks",
         (bool (hle::HanabiState::*) (hle::HanabiCard) const)
            &hle::HanabiState::CardPlayableOnFireworks,
         py::arg("card"),
         "Returns true if the card can be played on fireworks pile."
    )
    .def("chance_outcome_is_legal",
         &hle::HanabiState::ChanceOutcomeIsLegal,
         py::arg("move")
    )
    .def("chance_outcome_prob",
         &hle::HanabiState::ChanceOutcomeProb,
         py::arg("move")
    )
    .def("apply_chance_outcome",
         &hle::HanabiState::ApplyChanceOutcome,
         py::arg("move")
    )
    .def("apply_random_chance", &hle::HanabiState::ApplyRandomChance)
    .def_property_readonly(
        "chance_outcomes",
        &hle::HanabiState::ChanceOutcomes,
        "Get the valid chance moves, and associated probabilities. "
        "Guaranteed that moves.size() == probabilities.size()."
    )
    .def_property_readonly(
        "end_of_game_status",
        &hle::HanabiState::EndOfGameStatus
    )
    .def("is_terminal", &hle::HanabiState::IsTerminal)
    .def_property_readonly("score", &hle::HanabiState::Score)
    .def_property_readonly("current_player", &hle::HanabiState::CurPlayer)
    .def_property_readonly("life_tokens", &hle::HanabiState::LifeTokens)
    .def_property_readonly(
        "information_tokens",
        &hle::HanabiState::InformationTokens
    )
    .def_property_readonly("hands", &hle::HanabiState::Hands)
    .def_property_readonly("fireworks", &hle::HanabiState::Fireworks)
    // .def_property_readonly("parent_game", &hle::HanabiState::ParentGame)
    .def_property_readonly("deck", &hle::HanabiState::Deck)
    .def_property_readonly(
        "discard_pile",
        &hle::HanabiState::DiscardPile,
        "Get the discard pile (the element at the back is the most recent discard)."
    )
    .def_property_readonly(
        "move_history",
        &hle::HanabiState::MoveHistory,
        "Sequence of moves from beginning of game. Stored as <move, actor>."
    )
    
    .def("__str__", &hle::HanabiState::ToString)
    .def("__repr__", &hanabi_state_repr) 

    .doc() =
    "Construct a HanabiState, initialised to the start of the game. "
    "If start_player >= 0, the game-provided start player is overridden "
    "and the first player after chance is start_player.";

//  TODO: wrap deck 
//   py::class_<hle::HanabiState::HanabiDeck>(m, "HanabiDeck")
//     .def(py::init<hle::HanabiGame&, const int>())
//     .def("DealCard",

  py::enum_<hle::HanabiState::EndOfGameType>(hanabi_state, "EndOfGameType")
    .value("kNotFinished", hle::HanabiState::EndOfGameType::kNotFinished)
    .value("kOutOfLifeTokens",
           hle::HanabiState::EndOfGameType::kOutOfLifeTokens)
    .value("kOutOfCards", hle::HanabiState::EndOfGameType::kOutOfCards)
    .value("kCompletedFireworks",
           hle::HanabiState::EndOfGameType::kCompletedFireworks)
    .export_values();
}
