#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../hanabi_lib/hanabi_history_item.h"
#include "representations.h"

namespace py = pybind11;
namespace hle = hanabi_learning_env;

void wrap_hanabi_history_item(py::module& m) {
  py::class_<hle::HanabiHistoryItem> (m, "HanabiHistoryItem")
    .def(py::init<hle::HanabiMove>(), py::arg("move_made"))
    .def(py::init<const hle::HanabiHistoryItem&>(), py::arg("past_move"))
	.def(py::pickle(
			// __getstate__
			[](const hle::HanabiHistoryItem &hist) {
				// Return a tuple that fully encodes the state of the object
				return py::make_tuple(hist.move,
						hist.player,
						hist.scored,
						hist.information_token,
						hist.color,
						hist.rank,
						hist.reveal_bitmask,
						hist.newly_revealed_bitmask,
						hist.deal_to_player);
  	  	  	  },
			  // __setstate__
			  [](py::tuple t) {
  	  	  		  if (t.size() != 9)
  	  	  			  throw std::runtime_error("Invalid state!");

  	  	  		  // Create a new C++ instance
  	  	  		  hle::HanabiHistoryItem hist(t[0].cast<hle::HanabiMove>());

  	  	  		  // add additional parameters
  	  	  		  hist.player = t[1].cast<int8_t>();
  	  	  		  hist.scored = t[2].cast<bool>();
  	  	  		  hist.information_token = t[3].cast<bool>();
  	  	  		  hist.color = t[4].cast<int8_t>();
  	  	  		  hist.rank = t[5].cast<int8_t>();
  	  	  		  hist.reveal_bitmask = t[6].cast<uint8_t>();
  	  	  		  hist.newly_revealed_bitmask = t[7].cast<uint8_t>();
  	  	  		  hist.deal_to_player = t[8].cast<int8_t>();

  	  	  		  return hist;
  	  	  	  }
		)
	)
    .def_readwrite("move", &hle::HanabiHistoryItem::move, "Move that was made")
    .def_readwrite(
        "player",
        &hle::HanabiHistoryItem::player,
        "Index of player who made the move."
    )
    .def_readwrite(
        "scored",
        &hle::HanabiHistoryItem::scored,
        "Indicator of whether a Play move was successful."
    )
    .def_readwrite(
        "information_token",
        &hle::HanabiHistoryItem::information_token,
        "Indicator of whether a Play/Discard move added an information token"
    )
    .def_readwrite(
        "color",
        &hle::HanabiHistoryItem::color,
        "Color of card that was played or discarded. Valid if color_ >= 0."
    )
    .def_readwrite(
        "rank",
        &hle::HanabiHistoryItem::rank,
        "Rank of card that was played or discarded. Valid if rank_ >= 0."
    )
    .def_readwrite(
        "reveal_bitmask",
        &hle::HanabiHistoryItem::reveal_bitmask,
        "Bitmask indicating whether a card was targeted by a RevealX move. "
        "Bit_i=1 if color/rank of card_i matches X in a RevealX move. "
        "For example, if cards 0 and 3 had rank 2, a RevealRank 2 move "
        "would result in a reveal_bitmask of 9  (2^0+2^3)."
    )
    .def_readwrite(
        "newly_revealed_bitmask",
        &hle::HanabiHistoryItem::newly_revealed_bitmask,
        "Bitmask indicating whether a card was newly revealed by a RevealX move. "
        "Bit_i=1 if color/rank of card_i was not known before RevealX move. "
        "For example, if cards 1, 2, and 4 had color 'R', and the color of "
        "card 1 had previously been revealed to be 'R', a RevealRank 'R' move "
        "would result in a newly_revealed_bitmask of 20  (2^2+2^4)."
    )
    .def_readwrite(
        "deal_to_player",
        &hle::HanabiHistoryItem::deal_to_player,
        "Player that received a card from a Deal move."
    )

    .def("__str__", &hle::HanabiHistoryItem::ToString)
    .def("__repr__", &hanabi_history_item_repr)

    .doc() = "A move that has been made within a Hanabi game, "
             "along with the side-effects of making that move.";
}
