#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include "../hanabi_lib/hanabi_observation.h"
#include "representations.h"
#include <bits/stdc++.h>

namespace py = pybind11;
namespace hle = hanabi_learning_env;
using HanabiObservationVector = std::vector<hle::HanabiObservation>;
PYBIND11_MAKE_OPAQUE(HanabiObservationVector);

void wrap_hanabi_observation(py::module& m) {

  py::class_<HanabiObservationVector>(m, "HanabiObservationVector")
    .def("__len__", [](const HanabiObservationVector &v) { return v.size(); })
    .def("__iter__", [](HanabiObservationVector &v) {
       return py::make_iterator(v.begin(), v.end());
    }, py::keep_alive<0, 1>()) /* Keep vector alive while iterator is used */
    .def_property_readonly("information_tokens",
    	[](HanabiObservationVector& v)
		{
		  std::vector<int> it(v.size());
		  for(int i=0;i<v.size();i++)
			  it[i] = v[i].InformationTokens();
		  return it;
		}
    )
    .def_property_readonly("life_tokens",
    	[](HanabiObservationVector& v)
		{
		  std::vector<int> lt(v.size());
		  for(int i=0;i<v.size();i++)
			  lt[i] = v[i].LifeTokens();
		  return lt;
		}
    )
	.def_property_readonly("score",
		[](HanabiObservationVector& v)
		{
		  std::vector<int> score(v.size());
		  for(int i=0;i<v.size();i++)
			  score[i] = v[i].Score();
		  return score;
		}
	)
	.def_property_readonly("max_score",
		[](HanabiObservationVector& v)
		{
		  std::vector<int> max_score(v.size());
		  for(int i=0;i<v.size();i++)
			  max_score[i] = v[i].MaximumScore();
		  return max_score;
		}
	)
	.def_property_readonly("card_knowledge_indicator",
		[](HanabiObservationVector& v)
		{
		  std::vector<double> cki(v.size());
		  for(int i=0;i<v.size();i++)
			  cki[i] = v[i].CardKnowledgeIndicator();
		  return cki;
		}
	);

  py::class_<hle::HanabiObservation>(m, "HanabiObservation")
    .def(py::init<hle::HanabiState&, const int>(),
         py::arg("state"),
         py::arg("observing_player")
    )
	.def(py::pickle(
			// __getstate__
			[](const hle::HanabiObservation &obs) {
				// Return a tuple that fully encodes the state of the object
				return py::make_tuple(obs.ParentState(), obs.ObservingPlayer());
  	  	  	  },
			  // __setstate__
			  [](py::tuple t) {
  	  	  		  if (t.size() != 2)
  	  	  			  throw std::runtime_error("Invalid state!");

  	  	  		  // Create a new C++ instance

  	  	  		  hle::HanabiObservation obs(t[0].cast<hle::HanabiState>(),
  	  	  				  t[1].cast<int>());

  	  	  		  return obs;
  	  	  	  }
		)
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
	.def("playable_percent",
		&hle::HanabiObservation::PlayablePercent,
		"Probability of card being playable on fireworks"
	)
	.def("average_playability",
		&hle::HanabiObservation::AveragePlayability,
		"Playability of card not including car knowledge and other players cards"
	)
	.def("common_playability",
		&hle::HanabiObservation::CommonPlayability,
		"Playability of all cards in hand not including knowledge about other players cards"
	)
	.def("hand_possible",
		(bool (hle::HanabiObservation::*) (const std::vector<hle::HanabiCard>&))
		&hle::HanabiObservation::HandPossible,
		py::arg("cards"),
		"Probability of card being playable on fireworks"
	)
    .def("card_to_discard",
		(hle::HanabiCard (hle::HanabiObservation::*) (int) const)
		&hle::HanabiObservation::GetCardToDiscard,
		py::arg("index"),
		"get card to discard by index"
	)
//    .def("information_tokens",
//		(hle::HanabiCard (hle::HanabiObservation::*) () const)
//		&hle::HanabiObservation::InformationTokens,
//		"get number of information tokens"
//	)
//    .def("life_tokens",
//		(hle::HanabiCard (hle::HanabiObservation::*) () const)
//		&hle::HanabiObservation::LifeTokens,
//		"get number of life tokens"
//	)
    .def_property_readonly("score",
		&hle::HanabiObservation::Score,
		"get score"
	)
    .def_property_readonly("max_score",
		&hle::HanabiObservation::MaximumScore,
		"get maximum score"
	)
    .def_property_readonly("card_knowledge_indicator",
		&hle::HanabiObservation::CardKnowledgeIndicator,
		"get card knowledge indicator"
	)
    .def("__str__", &hle::HanabiObservation::ToString)
    .def("__repr__", &hanabi_observation_repr)
    .doc() = "Agent observation of a HanabiState.";
}
