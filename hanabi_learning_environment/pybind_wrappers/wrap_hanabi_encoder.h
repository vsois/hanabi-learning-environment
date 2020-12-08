#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../hanabi_lib/canonical_encoders.h"
#include "../hanabi_lib/observation_encoder.h"
#include "representations.h"

namespace py = pybind11;
namespace hle = hanabi_learning_env;

void wrap_hanabi_encoder(py::module& m) {

	py::class_<hle::CanonicalObservationEncoder> canonical_encoder (m, "CanonicalObservationEncoder");

	py::enum_<hle::ObservationEncoder::Type>(canonical_encoder, "Type")
	    .value("kCanonical", hle::ObservationEncoder::Type::kCanonical)
	    .export_values();

	canonical_encoder
    .def(py::init<const hle::HanabiGame*>(),
         py::arg("parent_game")
    )
    .def("encode",
         (std::vector<int> (hle::CanonicalObservationEncoder::*) (hle::HanabiObservation&) const)
		 &hle::CanonicalObservationEncoder::Encode,
         "vectorize observation object"
    )
	.def_property_readonly("shape", &hle::CanonicalObservationEncoder::Shape)
	.def_property_readonly("type", &hle::CanonicalObservationEncoder::type)
	.def("__repr__", &canonical_encoder_repr)
	.doc() = "Canonical Encoder to convert observation object into numeric vector";
}
