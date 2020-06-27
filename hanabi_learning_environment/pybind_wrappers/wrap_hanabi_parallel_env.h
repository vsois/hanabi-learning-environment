#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../hanabi_lib/hanabi_parallel_env.h"
#include "representations.h"

namespace py = pybind11;
namespace hle = hanabi_learning_env;

void wrap_hanabi_parallel_env(py::module& m) {
  py::class_<hle::HanabiParallelEnv>(m, "HanabiParallelEnv")
    .def(py::init<const std::unordered_map<std::string, std::string>&,
                  const int>(),
         py::arg("game_parameters"),
         py::arg("n_states")
    )
    .def("apply_batch_move",
         (void (hle::HanabiParallelEnv::*) (const std::vector<int>&, const int))
            &hle::HanabiParallelEnv::ApplyBatchMove,
         py::arg("batch_move_ids"),
         py::arg("agent_id"),
         "Apply moves specified as integers.")
    .def("apply_batch_move",
         (void (hle::HanabiParallelEnv::*) 
              (const std::vector<hle::HanabiMove>&, const int))
            &hle::HanabiParallelEnv::ApplyBatchMove,
         py::arg("batch_move"),
         py::arg("agent_id"),
         "Apply moves specified as instances of HanabiMove.")
    .def("observe_agent",
         &hle::HanabiParallelEnv::ObserveAgent,
         py::arg("agent_id"),
         "Get observations for the specified agent.",
         py::return_value_policy::reference
    )
    .def("observe_agent_encoded",
         &hle::HanabiParallelEnv::ObserveAgentEncoded,
         py::arg("agent_id"),
         "Get encoded observations for the specified agent."
    )
    .def_property_readonly(
        "parent_game",
        &hle::HanabiParallelEnv::ParentGame,
        "Access the underlying parent HanabiGame."
    )
    .def("encoded_observation_shape",
         &hle::HanabiParallelEnv::EncodedObservationShape,
         "Shape of a single (state's) encoded observation."
    )
    .def_property_readonly("states", &hle::HanabiParallelEnv::States)
    .def("get_observation_flat_length",
         &hle::HanabiParallelEnv::GetObservationFlatLength,
         "Shape of a single (state's) encoded observation flattened."
    )
    .def_property_readonly("num_states", &hle::HanabiParallelEnv::NumStates)
    .def("reset_states",
         &hle::HanabiParallelEnv::ResetStates,
         py::arg("state_ids"),
         py::arg("current_agent_id"),
         "Replace specified states with new ones with agent as current player."
    )
    .def("reset",
        &hle::HanabiParallelEnv::Reset,
        "Completely reset the environment."
        )
    .def("encode_observations",
         &hle::HanabiParallelEnv::EncodeObservations,
         py::arg("observations"),
         "Convert observations to one-hot encoded representation."
    )
    .def("get_state_statuses",
         &hle::HanabiParallelEnv::GetStateStatuses,
         "Get game status (see HanabiState.EndOfGameType) for each state."
    )
    .def("get_legal_moves",
         &hle::HanabiParallelEnv::GetLegalMoves,
         py::arg("agent_id"),
         "Get legal moves for each states for specified agent."
    )
    .def("encode_legal_moves",
         (std::vector<std::vector<int8_t>>
              (hle::HanabiParallelEnv::*) 
              (const std::vector<hle::HanabiObservation>&) const)
            &hle::HanabiParallelEnv::EncodeLegalMoves,
         py::arg("observations"),
         "One-hot encode legal moves."
    )

    .def("encode_legal_moves",
         (std::vector<std::vector<int8_t>>
              (hle::HanabiParallelEnv::*)
              (const std::vector<std::vector<hle::HanabiMove>>&) const)
            &hle::HanabiParallelEnv::EncodeLegalMoves,
         py::arg("legal_moves"),
         "One-hot encode legal moves."
    )
    .def("get_scores", &hle::HanabiParallelEnv::GetScores)
    .def("moves_are_legal",
         &hle::HanabiParallelEnv::MovesAreLegal,
         py::arg("moves"),
         "Check whether the supplied moves can be applied. "
         "Number of moves must be equal to number of states."
    )
    .def("get_moves", &hle::HanabiParallelEnv::GetMoves)
    .def("make_legal",
         &hle::HanabiParallelEnv::MakeLegal,
         py::arg("moves"),
         py::arg("agent_id"),
         "Converts illegal moves to first legal inplace"
    )
    .def("step",
         (void (hle::HanabiParallelEnv::*)
              (const std::vector<hle::HanabiMove>&, const int, const int))
            &hle::HanabiParallelEnv::Step,
         py::arg("moves"),
         py::arg("current_agent_id"),
         py::arg("next_agent_id"),
         "Make a step: catch illegal moves, apply good moves, precompute observations "
         "for the next agent. Results can be retrieved using accessor functions."
    )
    .def("step",
         (void (hle::HanabiParallelEnv::*)
              (const std::vector<int>&, const int, const int))
            &hle::HanabiParallelEnv::Step,
         py::arg("move_uids"),
         py::arg("current_agent_id"),
         py::arg("next_agent_id"),
         "Make a step: catch illegal moves, apply good moves, precompute observations "
         "for the next agent. Results can be retrieved using accessor functions."
    )
    .def_property_readonly(
        "state_observations",
        &hle::HanabiParallelEnv::StateObservations,
        py::return_value_policy::reference,
        "Retrieve state observations as reference."
    )
    .def_property_readonly(
        "illegal_moves",
        &hle::HanabiParallelEnv::IllegalMoves,
        py::return_value_policy::reference,
        "Retrieve moves as reference which were illegal in the last step."
    )
    .def_property_readonly(
        "encoded_observations",
        &hle::HanabiParallelEnv::EncodedStateObservations,
        py::return_value_policy::reference,
        "Retrieve encoded observations as reference."
    )
    .def_property_readonly(
        "encoded_legal_moves",
        &hle::HanabiParallelEnv::EncodedLegalMoves,
        py::return_value_policy::reference,
        "Retrieve encoded legal moves as reference."
    )
    .def("__repr__", &hanabi_parallel_env_repr)

    .doc() = "An environment that handles multiple HanabiStates in parallel.";
}
