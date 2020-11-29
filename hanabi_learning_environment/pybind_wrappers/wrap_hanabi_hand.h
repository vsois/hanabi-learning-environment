#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../hanabi_lib/hanabi_hand.h"
#include "representations.h"

namespace py = pybind11;
namespace hle = hanabi_learning_env;


void wrap_hanabi_hand(py::module& m) {
  py::class_<hle::HanabiHand> (m, "HanabiHand")
    .def(py::init<const hle::HanabiHand&>(), py::arg("hand"))
    .def(py::init<const hle::HanabiHand&, bool, bool>(),
         py::arg("hand"),
         py::arg("hide_cards"),
         py::arg("hide_knowledge")
    )
    .def(py::init<const std::vector<hle::HanabiCard>&,
    		const std::vector<hle::HanabiHand::CardKnowledge>&>(),
         py::arg("cards"),
         py::arg("knowledge")
    )
    .def(py::init())
	.def(py::pickle(
			// __getstate__
			[](const hle::HanabiHand &hand) {
				// Return a tuple that fully encodes the state of the object
				return py::make_tuple(hand.Cards(), hand.Knowledge()); // hand.Cards(), hand.Knowledge()
  	  	  	  },
			  // __setstate__
			  [](py::tuple t) {
  	  	  		  if (t.size() != 2)
  	  	  			  throw std::runtime_error("Invalid state!");

  	  	  		  // Create a new C++ instance
  	  	  		  hle::HanabiHand hand(t[0].cast<std::vector<hle::HanabiCard>>(),
  	  	  				  t[1].cast<std::vector<hle::HanabiHand::CardKnowledge>>());

  	  	  		  return hand;
  	  	  	  }
		)
	)
    .def_property_readonly(
        "cards",
        &hle::HanabiHand::Cards,
        "Cards and corresponding card knowledge are always arranged from "
        "oldest to newest, with the oldest card or knowledge at index 0."
    )
    .def_property_readonly("knowledge", &hle::HanabiHand::Knowledge)
    .def("add_card",
         &hle::HanabiHand::AddCard,
         py::arg("card"),
         py::arg("initial_knowledge"),
         "Add card to the hand."
    )
    .def("remove_from_hand",
         &hle::HanabiHand::RemoveFromHand,
         py::arg("card_index"),
         py::arg("discard_pile") = nullptr,
         "Remove card_index card from hand. Put in discard_pile if not nullptr "
         "(pushes the card to the back of the discard_pile vector)."
    )
    .def("reveal_rank",
         &hle::HanabiHand::RevealRank,
         py::arg("rank"),
         "Make cards with the given rank visible. "
         "Returns new information bitmask, bit_i set if card_i color was "
         "revealed and was previously unknown."
    )
    .def("reveal_color",
         &hle::HanabiHand::RevealColor,
         py::arg("color"),
         "Make cards with the given color visible. "
         "Returns new information bitmask, bit_i set if card_i color was "
         "revealed and was previously unknown."
    )

    .def("__len__", [](const hle::HanabiHand& h) {return h.Cards().size();})
    .def("__str__", &hle::HanabiHand::ToString)
    .def("__repr__", &hanabi_hand_repr);

  py::class_<hle::HanabiHand::ValueKnowledge> (m, "ValueKnowledge")
    .def(py::init<int>())
	.def(py::pickle(
			// __getstate__
			[](const hle::HanabiHand::ValueKnowledge &knowledge) {
				// Return a tuple that fully encodes the state of the object
				return py::make_tuple(knowledge.Range());
  	  	  	  },
			  // __setstate__
			  [](py::tuple t) {
  	  	  		  if (t.size() != 1)
  	  	  			  throw std::runtime_error("Invalid state!");

  	  	  		  // Create a new C++ instance
  	  	  		  hle::HanabiHand::ValueKnowledge knowledge(t[0].cast<int>());

  	  	  		  return knowledge;
  	  	  	  }
		)
	)
    .def_property_readonly(
        "range",
        &hle::HanabiHand::ValueKnowledge::Range,
        "Maximum value of the plausible value range."
    )
    .def("value_hinted",
         &hle::HanabiHand::ValueKnowledge::ValueHinted,
         "Returns true if and only if the exact value was revealed. "
         "Does not perform inference to get a known value from not-value hints."
    )
    .def_property_readonly(
        "value",
        &hle::HanabiHand::ValueKnowledge::Value,
        "Hinted value. -1 if value was not hinted."
    )
    .def("is_plausible",
         &hle::HanabiHand::ValueKnowledge::IsPlausible,
         "Returns true if we have no hint saying variable "
         "is not the given value."
    )
    .def("apply_is_value_hint",
         &hle::HanabiHand::ValueKnowledge::ApplyIsValueHint,
         "Record a hint that gives the value of the variable."
    )
    .def("apply_is_not_value_hint",
         &hle::HanabiHand::ValueKnowledge::ApplyIsNotValueHint,
         "Record a hint that the variable does not have the given value."
    )
    .def("__repr__", &hanabi_valueknow_repr)
    .doc() = ""
    "Knowledge about an unknown integer variable in range 0 to value_range-1. "
    "Records hints that either reveal the exact value (no longer unknown), "
    "or reveal that the variable is not some particular value. "
    "For example, ValueKnowledge(3) tracks a variable that can be 0, 1, or 2. "
    "Initially, ValueHinted()=false, value()=-1, and ValueCouldBe(v)=true "
    "for v=0, 1, and 2. "
    "After recording that the value is not 1, we have "
    "ValueHinted()=false, value()=-1, and ValueCouldBe(1)=false. "
    "After recording that the value is 0, we have "
    "ValueHinted()=true, value()=0, and ValueCouldBe(v)=false for v=1, and 2. ";

  py::class_<hle::HanabiHand::CardKnowledge> (m, "CardKnowledge")
    .def(py::init<int, int>())
	.def(py::pickle(
			// __getstate__
			[](const hle::HanabiHand::CardKnowledge &knowledge) {
				// Return a tuple that fully encodes the state of the object
				return py::make_tuple(knowledge.NumColors(), knowledge.NumRanks());
  	  	  	  },
			  // __setstate__
			  [](py::tuple t) {
  	  	  		  if (t.size() != 2)
  	  	  			  throw std::runtime_error("Invalid state!");

  	  	  		  // Create a new C++ instance
  	  	  		  hle::HanabiHand::CardKnowledge knowledge(t[0].cast<int>(),
  	  	  				  t[1].cast<int>());

  	  	  		  return knowledge;
  	  	  	  }
		)
	)
    .def_property_readonly(
        "num_colors",
        &hle::HanabiHand::CardKnowledge::NumColors,
        "Returns number of possible colors being tracked."
    )
    .def("color_hinted",
         &hle::HanabiHand::CardKnowledge::ColorHinted,
         "Returns true if and only if the exact color was revealed. "
         "Does not perform inference to get a known color from not-color hints."
    )
    .def_property_readonly(
        "color",
        &hle::HanabiHand::CardKnowledge::Color,
        "Color of card if it was hinted, -1 if not hinted."
    )
    .def("color_plausible",
         &hle::HanabiHand::CardKnowledge::ColorPlausible,
         py::arg("color"),
         "Returns true if we have no hint saying card is not the given color."
    )
    .def("apply_is_color_hint",
         &hle::HanabiHand::CardKnowledge::ApplyIsColorHint,
         "Record a hint that gives the value of the color."
    )
    .def("apply_is_not_color_hint",
         &hle::HanabiHand::CardKnowledge::ApplyIsNotColorHint,
         "Record a hint that the color does not have the given value."
    )
    .def_property_readonly(
        "num_ranks",
        &hle::HanabiHand::CardKnowledge::NumRanks,
        "Returns number of possible ranks being tracked."
    )
    .def("rank_hinted",
         &hle::HanabiHand::CardKnowledge::RankHinted,
         "Returns true if and only if the exact rank was revealed. "
         "Does not perform inference to get a known rank from not-rank hints."
    )
    .def_property_readonly(
        "rank",
        &hle::HanabiHand::CardKnowledge::Rank,
        "Rank of card if it was hinted, -1 if not hinted."
    )
    .def("rank_plausible",
         &hle::HanabiHand::CardKnowledge::RankPlausible,
         "Returns true if we have no hint saying card is not the given rank."
    )
    .def("apply_is_rank_hint",
         &hle::HanabiHand::CardKnowledge::ApplyIsRankHint,
         "Record a hint that gives the value of the rank."
    )
    .def("apply_is_not_rank_hint",
         &hle::HanabiHand::CardKnowledge::ApplyIsNotRankHint,
         "Record a hint that the rank does not have the given value."
    )
    .def("__str__", &hle::HanabiHand::CardKnowledge::ToString)
    .def("__repr__", &hanabi_cardknow_repr)
    
    .doc() = "Hinted knowledge about color and rank "
             "of an initially unknown card.";

}
