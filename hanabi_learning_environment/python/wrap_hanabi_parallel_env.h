#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../hanabi_lib/hanabi_parallel_env.h"

namespace py = pybind11;

void wrap_hanabi_parallel_env(py::module& m) {
  // py::class_<StateVector>(m, "StateVector");
  py::class_<hanabi_learning_env::HanabiParallelEnv>(m, "HanabiParallelEnv")
    .def(py::init<const std::unordered_map<std::string, std::string>&, const int>())
    .def("ApplyBatchMove",
         (void (hanabi_learning_env::HanabiParallelEnv::*) (const std::vector<int>&, const int)) &hanabi_learning_env::HanabiParallelEnv::ApplyBatchMove,
         "Apply moves specified as integers.")
    .def("ApplyBatchMove",
         (void (hanabi_learning_env::HanabiParallelEnv::*) (const std::vector<hanabi_learning_env::HanabiMove>&, const int)) &hanabi_learning_env::HanabiParallelEnv::ApplyBatchMove,
         "Apply moves specified as instances of HanabiMove.")
    .def("ObserveAgent", &hanabi_learning_env::HanabiParallelEnv::ObserveAgent)
    .def("ObserveAgentEncoded", &hanabi_learning_env::HanabiParallelEnv::ObserveAgentEncoded)
    .def("GetGame", &hanabi_learning_env::HanabiParallelEnv::GetGame)
    .def("GetObservationShape", &hanabi_learning_env::HanabiParallelEnv::GetObservationShape)
    .def("GetStates", &hanabi_learning_env::HanabiParallelEnv::GetStates)
    // .def("GetStates",
    //     [](const hanabi_learning_env::HanabiParallelEnv &env)
    //     { return py::make_iterator(env.GetStates().begin(), env.GetStates().end());})
    // .def_readonly("GetStates", &hanabi_learning_env::HanabiParallelEnv::GetStates)
    .def("GetObservationFlatLength", &hanabi_learning_env::HanabiParallelEnv::GetObservationFlatLength)
    .def("GetNumStates", &hanabi_learning_env::HanabiParallelEnv::GetNumStates)
    .def("ResetStates", &hanabi_learning_env::HanabiParallelEnv::ResetStates)
    .def("Reset", &hanabi_learning_env::HanabiParallelEnv::Reset);
}
