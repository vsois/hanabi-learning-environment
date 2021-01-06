#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include "../hanabi_lib/hanabi_parallel_env.h"
#include "representations.h"

namespace py = pybind11;
namespace hle = hanabi_learning_env;
using Int8Vector = std::vector<int8_t>;
PYBIND11_MAKE_OPAQUE(Int8Vector)

void wrap_hanabi_parallel_env(py::module& m) {

	py::class_<hle::HanabiParallelEnv>(m, "HanabiParallelEnv", py::buffer_protocol())
	    .def(py::init<const std::unordered_map<std::string, std::string>&,
	                  const int>(),
	         py::arg("game_parameters"),
	         py::arg("n_states")
	    )
	    .def("reset",
	        &hle::HanabiParallelEnv::Reset,
	        "Completely reset the environment."
		)
	    .def("reset_state",
	         &hle::HanabiParallelEnv::ResetState,
	         py::arg("state_ids"),
	         py::arg("current_agent_id"),
	         "Replace specified states with new ones with agent as current player."
	    )
		.def("apply_batch_move",
			(void (hle::HanabiParallelEnv::*) (const std::vector<int>&, const int))
			   &hle::HanabiParallelEnv::ApplyBatchMove,
			py::arg("batch_move_ids"),
			py::arg("agent_id"),
			"Apply moves specified as integers."
		)
	    .def("apply_batch_move",
			(void (hle::HanabiParallelEnv::*) (const std::vector<hle::HanabiMove>&, const int))
			   &hle::HanabiParallelEnv::ApplyBatchMove,
			py::arg("batch_move"),
			py::arg("agent_id"),
			"Apply moves specified as instances of HanabiMove."
		)
	    .def("step",
	         (const std::vector<bool>& (hle::HanabiParallelEnv::*)
	        	(const std::vector<hle::HanabiMove>&, const int))
	            &hle::HanabiParallelEnv::Step,
	         py::arg("moves"),
	         py::arg("current_agent_id"),
	         "Make a step: catch illegal moves, apply good moves.",
			 py::return_value_policy::reference
	    )
	    .def("step",
	         (const std::vector<bool>& (hle::HanabiParallelEnv::*)
	        	(const std::vector<int>&, const int))
	            &hle::HanabiParallelEnv::Step,
	         py::arg("move_uids"),
	         py::arg("current_agent_id"),
	         "Make a step: catch illegal moves, apply good moves.",
			 py::return_value_policy::reference
	    )
	    .def("observe_agent",
			 (const std::vector<hle::HanabiObservation>& (hle::HanabiParallelEnv::*)
				(const int))
				&hle::HanabiParallelEnv::ObserveAgent,
			 py::arg("agent_id"),
			 "Get observations for the specified agent.",
			 py::return_value_policy::copy
	    )
	    .def("observe_agent",
			 (const hle::HanabiObservation& (hle::HanabiParallelEnv::*)
				(int, int))
				&hle::HanabiParallelEnv::ObserveAgent,
			 py::arg("agent_id"),
			 py::arg("index"),
			 "Get observations for the specified agent.",
			 py::return_value_policy::copy
	    )
		.def("get_score",
			&hle::HanabiParallelEnv::GetScore
		)
	    .def("encode_observation",
			[](hle::HanabiParallelEnv& e, const std::vector<hle::HanabiObservation>& obs)
			{
				e.EncodeObservation(obs);
				return py::array(
						{e.NumStates(), e.GetEncodedObservationFlatLength()},
						e.EncodedStateObservations());
			},
			"Convert observations to one-hot encoded representation."
	    )
	    .def("encode_observation",
			[](hle::HanabiParallelEnv& e, py::list lobj)
			{
				std::vector<hle::HanabiObservation> obs_vec;
				for (py::handle obj : lobj) {
					hle::HanabiObservation obs = obj.cast<hle::HanabiObservation>();
					obs_vec.push_back(obs);
				}
				e.EncodeObservation(obs_vec);
				int size = static_cast<int>(obs_vec.size());
				return py::array(
						{size, e.GetEncodedObservationFlatLength()},
						e.EncodedStateObservations());
			},
			"Convert observations to one-hot encoded representation."
	    )
	    .def("encode_legal_moves",
			[](hle::HanabiParallelEnv& e, const std::vector<hle::HanabiObservation>& obs)
			{
				e.EncodeLegalMoves(obs);
				return py::array(
						{e.NumStates(), e.ParentGame().MaxMoves()},
						e.EncodedLegalMoves());
			},
			"Convert observations to one-hot encoded representation."
	    )
	    .def("encode_legal_moves",
			[](hle::HanabiParallelEnv& e, py::list lobj)
			{
				std::vector<hle::HanabiObservation> obs_vec;
				for (py::handle obj : lobj) {
					hle::HanabiObservation obs = obj.cast<hle::HanabiObservation>();
					obs_vec.push_back(obs);
				}
				e.EncodeLegalMoves(obs_vec);
				int size = static_cast<int>(obs_vec.size());
				return py::array(
						{size, e.ParentGame().MaxMoves()},
						e.EncodedLegalMoves());
			},
			"Convert observations to one-hot encoded representation."
	    )
		.def("encode",
			[](hle::HanabiParallelEnv& e, const std::vector<hle::HanabiObservation>& obs)
			{
				e.EncodeLegalMoves(obs);
				e.EncodeObservation(obs);

				return py::make_tuple(
						py::array({e.NumStates(), e.GetEncodedObservationFlatLength()},
								e.EncodedStateObservations()),
						py::array({e.NumStates(), e.ParentGame().MaxMoves()},
								e.EncodedLegalMoves())
				);
			},
			"Convert observations to one-hot encoded representation."
		)
	    .def("encode",
			[](hle::HanabiParallelEnv& e, py::list lobj)
			{
				std::vector<hle::HanabiObservation> obs_vec;
				for (py::handle obj : lobj) {
					hle::HanabiObservation obs = obj.cast<hle::HanabiObservation>();
					obs_vec.push_back(obs);
				}
				e.EncodeLegalMoves(obs_vec);
				e.EncodeObservation(obs_vec);
				int size = static_cast<int>(obs_vec.size());

				return py::make_tuple(
						py::array({size, e.GetEncodedObservationFlatLength()},
								e.EncodedStateObservations()),
						py::array({size, e.ParentGame().MaxMoves()},
								e.EncodedLegalMoves())
				);
			},
			"Convert observations to one-hot encoded representation."
	    )
		.def("get_move",
			&hle::HanabiParallelEnv::GetMove
		)
	    .def("get_state_status",
	         &hle::HanabiParallelEnv::GetStateStatus,
	         "Get game status (see HanabiState.EndOfGameType) for each state."
	    )
	    .def("encoded_observation_shape",
	         &hle::HanabiParallelEnv::GetEncodedObservationShape,
	         "Shape of a single (state's) encoded observation."
	    )
	    .def("encoded_observation_flat_length",
	         &hle::HanabiParallelEnv::GetEncodedObservationFlatLength,
	         "Shape of a single (state's) encoded observation flattened."
	    )
	    .def_property_readonly(
	        "parent_game",
	        &hle::HanabiParallelEnv::ParentGame,
	        "Access the underlying parent HanabiGame."
	    )
		.def_property_readonly(
			"states",
			&hle::HanabiParallelEnv::States
		)
		.def_property_readonly(
			"num_states",
			&hle::HanabiParallelEnv::NumStates
		)
	    .def("__repr__", &hanabi_parallel_env_repr)
	    .doc() = "An environment that handles multiple HanabiStates in parallel.";

}
