#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../hanabi_lib/hanabi_history_item.h"

namespace py = pybind11;

void wrap_hanabi_history_item(py::module& m) {
  py::class_<hanabi_learning_env::HanabiHistoryItem> (m, "HanabiHistoryItem")
    .def(py::init<hanabi_learning_env::HanabiMove>())
    .def(py::init<const hanabi_learning_env::HanabiHistoryItem&>())
    .def_readonly("player", &hanabi_learning_env::HanabiHistoryItem::player)
    .def_readonly("scored", &hanabi_learning_env::HanabiHistoryItem::scored)
    .def_readonly("information_token", &hanabi_learning_env::HanabiHistoryItem::information_token)
    .def_readonly("color", &hanabi_learning_env::HanabiHistoryItem::color)
    .def_readonly("rank", &hanabi_learning_env::HanabiHistoryItem::rank)
    .def_readonly("reveal_bitmask", &hanabi_learning_env::HanabiHistoryItem::reveal_bitmask)
    .def_readonly("newly_revealed_bitmask", &hanabi_learning_env::HanabiHistoryItem::newly_revealed_bitmask)
    .def_readonly("deal_to_player", &hanabi_learning_env::HanabiHistoryItem::deal_to_player)

    .def("__str__", &hanabi_learning_env::HanabiHistoryItem::ToString);
}
