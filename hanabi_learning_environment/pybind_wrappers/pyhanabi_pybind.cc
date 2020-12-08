#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "wrap_hanabi_card.h"
#include "wrap_hanabi_move.h"
#include "wrap_hanabi_hand.h"
#include "wrap_hanabi_game.h"
#include "wrap_hanabi_history_item.h"
#include "wrap_hanabi_state.h"
#include "wrap_hanabi_observation.h"
#include "wrap_hanabi_parallel_env.h"
#include "wrap_hanabi_encoder.h"

PYBIND11_MODULE(pyhanabi_pybind, m) {
  wrap_hanabi_card(m);
  wrap_hanabi_move(m);
  wrap_hanabi_hand(m);
  wrap_hanabi_game(m);
  wrap_hanabi_history_item(m);
  wrap_hanabi_state(m);
  wrap_hanabi_observation(m);
  wrap_hanabi_parallel_env(m);
  wrap_hanabi_encoder(m);
  m.doc() = "Python interface to C++ Hanabi library using pybind11";
}
