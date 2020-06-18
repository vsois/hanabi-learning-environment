#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../hanabi_lib/hanabi_card.h"

namespace py = pybind11;

void wrap_hanabi_card(py::module& m) {
  py::class_<hanabi_learning_env::HanabiCard> hanabi_card(m, "HanabiCard");
  hanabi_card
    .def(py::init<hanabi_learning_env::HanabiCard::ColorType,
                  hanabi_learning_env::HanabiCard::RankType>()
    )
    .def(py::init())
    .def("__eq__", &hanabi_learning_env::HanabiCard::operator==)
    .def("is_valid", &hanabi_learning_env::HanabiCard::IsValid)
    .def_property_readonly("color", &hanabi_learning_env::HanabiCard::Color)
    .def_property_readonly("rank", &hanabi_learning_env::HanabiCard::Rank)

    .def("__repr__",
        [](const hanabi_learning_env::HanabiCard& c) {
          return "<HanabiCard " + c.ToString() + ">";
        }
    )
    .def("__str__", &hanabi_learning_env::HanabiCard::ToString);

  py::enum_<hanabi_learning_env::HanabiCard::ColorType>(
      hanabi_card, "ColorType")
    .value("kRed", hanabi_learning_env::HanabiCard::ColorType::kRed)
    .value("kYellow", hanabi_learning_env::HanabiCard::ColorType::kYellow)
    .value("kGreen", hanabi_learning_env::HanabiCard::ColorType::kGreen)
    .value("kWhite", hanabi_learning_env::HanabiCard::ColorType::kWhite)
    .value("kBlue", hanabi_learning_env::HanabiCard::ColorType::kBlue)
    .value("kUnknownColor",
           hanabi_learning_env::HanabiCard::ColorType::kUnknownColor
    )
    .export_values();

  py::enum_<hanabi_learning_env::HanabiCard::RankType>(hanabi_card, "RankType")
    .value("k1", hanabi_learning_env::HanabiCard::RankType::k1)
    .value("k2", hanabi_learning_env::HanabiCard::RankType::k2)
    .value("k3", hanabi_learning_env::HanabiCard::RankType::k3)
    .value("k4", hanabi_learning_env::HanabiCard::RankType::k4)
    .value("k5", hanabi_learning_env::HanabiCard::RankType::k5)
    .value("kUnknownRank",
           hanabi_learning_env::HanabiCard::RankType::kUnknownRank
    )
    .export_values();
}
