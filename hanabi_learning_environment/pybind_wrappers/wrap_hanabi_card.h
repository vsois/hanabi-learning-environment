#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../hanabi_lib/hanabi_card.h"
#include "representations.h"

namespace py = pybind11;
namespace hle = hanabi_learning_env;

void wrap_hanabi_card(py::module& m) {
  py::class_<hle::HanabiCard> hanabi_card(m, "HanabiCard");
  hanabi_card
    .def(py::init<hle::HanabiCard::ColorType,
                  hle::HanabiCard::RankType>(),
         py::arg("color"),
         py::arg("rank")
    )
    .def(py::init())
    .def("__eq__", &hle::HanabiCard::operator==)
    .def("is_valid",
         &hle::HanabiCard::IsValid,
         "True if card's color and rank are valid"
    )
    .def_property_readonly("color", &hle::HanabiCard::Color)
    .def_property_readonly("rank", &hle::HanabiCard::Rank)

    .def("__repr__", &hanabi_card_repr)
    .def("__str__", &hle::HanabiCard::ToString);

  py::enum_<hle::HanabiCard::ColorType>(
      hanabi_card, "ColorType")
    .value("kRed", hle::HanabiCard::ColorType::kRed)
    .value("kYellow", hle::HanabiCard::ColorType::kYellow)
    .value("kGreen", hle::HanabiCard::ColorType::kGreen)
    .value("kWhite", hle::HanabiCard::ColorType::kWhite)
    .value("kBlue", hle::HanabiCard::ColorType::kBlue)
    .value("kUnknownColor",
           hle::HanabiCard::ColorType::kUnknownColor
    )
    .export_values();

  py::enum_<hle::HanabiCard::RankType>(hanabi_card, "RankType")
    .value("k1", hle::HanabiCard::RankType::k1)
    .value("k2", hle::HanabiCard::RankType::k2)
    .value("k3", hle::HanabiCard::RankType::k3)
    .value("k4", hle::HanabiCard::RankType::k4)
    .value("k5", hle::HanabiCard::RankType::k5)
    .value("kUnknownRank",
           hle::HanabiCard::RankType::kUnknownRank
    )
    .export_values();
}
