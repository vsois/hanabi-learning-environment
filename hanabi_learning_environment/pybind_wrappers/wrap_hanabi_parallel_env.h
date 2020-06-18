#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../hanabi_lib/hanabi_parallel_env.h"

namespace py = pybind11;

void wrap_hanabi_parallel_env(py::module& m) {
  // py::class_<StateVector>(m, "StateVector");
  py::class_<hanabi_learning_env::HanabiParallelEnv>(m, "HanabiParallelEnv")
    .def(py::init<const std::unordered_map<std::string, std::string>&, const int>())
    .def("apply_batch_move",
         (void (hanabi_learning_env::HanabiParallelEnv::*) (const std::vector<int>&, const int)) &hanabi_learning_env::HanabiParallelEnv::ApplyBatchMove,
         "Apply moves specified as integers.")
    .def("apply_batch_move",
         (void (hanabi_learning_env::HanabiParallelEnv::*) (const std::vector<hanabi_learning_env::HanabiMove>&, const int)) &hanabi_learning_env::HanabiParallelEnv::ApplyBatchMove,
         "Apply moves specified as instances of HanabiMove.")
    .def("observe_agent", &hanabi_learning_env::HanabiParallelEnv::ObserveAgent)
    .def("observe_agent_encoded", &hanabi_learning_env::HanabiParallelEnv::ObserveAgentEncoded)
    .def_property_readonly("parent_game", &hanabi_learning_env::HanabiParallelEnv::ParentGame)
    .def("observation_shape", &hanabi_learning_env::HanabiParallelEnv::EncodedObservationShape)
    .def_property_readonly("states", &hanabi_learning_env::HanabiParallelEnv::States)
    // .def("GetStates",
    //     [](const hanabi_learning_env::HanabiParallelEnv &env)
    //     { return py::make_iterator(env.GetStates().begin(), env.GetStates().end());})
    // .def_readonly("GetStates", &hanabi_learning_env::HanabiParallelEnv::GetStates)
    .def("get_observation_flat_length", &hanabi_learning_env::HanabiParallelEnv::GetObservationFlatLength)
    .def_property_readonly("num_states", &hanabi_learning_env::HanabiParallelEnv::NumStates)
    .def("reset_states", &hanabi_learning_env::HanabiParallelEnv::ResetStates)
    .def("reset", &hanabi_learning_env::HanabiParallelEnv::Reset)
    .def("encode_observations", &hanabi_learning_env::HanabiParallelEnv::EncodeObservations)
    .def("get_state_statuses", &hanabi_learning_env::HanabiParallelEnv::GetStateStatuses)
    .def("get_legal_moves", &hanabi_learning_env::HanabiParallelEnv::GetLegalMoves)
    .def("encode_legal_moves",
        (std::vector<std::vector<int8_t>>
         (hanabi_learning_env::HanabiParallelEnv::*)
         (const std::vector<hanabi_learning_env::HanabiObservation>&) const)
            &hanabi_learning_env::HanabiParallelEnv::EncodeLegalMoves)
    .def("encode_legal_moves",
        (std::vector<std::vector<int8_t>>
         (hanabi_learning_env::HanabiParallelEnv::*)
         (const std::vector<std::vector<hanabi_learning_env::HanabiMove>>&) const)
        &hanabi_learning_env::HanabiParallelEnv::EncodeLegalMoves)
    .def("get_scores", &hanabi_learning_env::HanabiParallelEnv::GetScores)
    .def("moves_are_legal", &hanabi_learning_env::HanabiParallelEnv::MovesAreLegal)
    .def("__repr__",
         [](const hanabi_learning_env::HanabiParallelEnv& e) {
            return "<HanabiParallelEnv >";
         }
    );
}
