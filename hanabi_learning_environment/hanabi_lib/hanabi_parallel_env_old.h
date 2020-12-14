// Copyright 2020 Anton Komissarov (anton.v.komissarov@gmail.com)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __HANABI_PARALLEL_ENV_H__
#define __HANABI_PARALLEL_ENV_H__

#include <algorithm>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#include "hanabi_card.h"
#include "hanabi_move.h"
#include "hanabi_game.h"
#include "hanabi_state.h"
#include "hanabi_observation.h"
#include "canonical_encoders.h"
#include <iostream>

namespace hanabi_learning_env {

/** \brief An environment that handles multiple HanabiStates in parallel.
 */
class HanabiParallelEnv {
 public:

  /** \brief Struct for batched observations.
   */
//  struct HanabiEncodedBatchObservation {
//
//    /** \brief Construct a batched observation of given shape.
//     *
//     *  \param n_states Number of states.
//     *  \param observation_len Length of a single flat encoded observation.
//     *  \param max_moves Total number of possible moves.
//     */
//    HanabiEncodedBatchObservation(const int n_states, const int observation_len,
//        const int max_moves)
//      : observation(n_states * observation_len, 0),
//        legal_moves(n_states * max_moves, 0),
//        scores(n_states, 0),
//        done(n_states, false),
//        observation_shape({n_states, observation_len}),
//        legal_moves_shape({n_states, max_moves}) {
//    }
//    std::vector<int> observation; //< Concatenated flat encoded observations.
//    std::vector<int> legal_moves; //< Concatenated legal moves.
//    std::vector<int> scores;      //< Concatenated scores.
//    std::vector<bool> done;       //< Concatenated termination statuses.
//    std::array<int, 2> observation_shape{0, 0}; //< Shape of batched observation (n_states x encoded_observation_length).
//    std::array<int, 2> legal_moves_shape{0, 0}; //< Shape of legal moves (n_states x max_moves).
//  };

//  /** \brief Construct an environment with a single game with several parallel states.
//   *
//   *  \param game_params Parameters of the game. See HanabiGame.
//   *  \param n_states Number of parallel states.
//   */
//  explicit HanabiParallelEnv(
//      const std::unordered_map<std::string, std::string>& game_params,
//      const int n_states);

//  /** \brief Make a step: apply moves to states.
//   *
//   *  \param batch_move Moves, one for each state.
//   */
//  void ApplyBatchMove(
//      const std::vector<HanabiMove>& batch_move, const int agent_id);
//
//  /** \overload with moves encoded as move ids.
//   */
//  void ApplyBatchMove(
//      const std::vector<int>& batch_move_ids, const int agent_id);

//  /** \brief Get observations for the specified agent.
//   */
//  const std::vector<HanabiObservation>& ObserveAgent(const int agent_id);

  /** \brief Get encoded observations for the specified agent.
   */
  HanabiEncodedBatchObservation ObserveAgentEncoded(const int agent_id);

  /** \brief Get a reference to the HanabiGame game.
   */
  const HanabiGame& ParentGame() const {return game_;}

  /** \brief Get a pointer to the HanabiGame game.
   */
  HanabiGame* GetParentGamePtr() {return &game_;}
  const HanabiGame* GetParentGamePtr() const {return &game_;}

  /** \brief Get shape of a single encoded observation.
   */
  std::vector<int> EncodedObservationShape() const {return observation_encoder_.Shape();};

  /** \brief Get a const reference to the parallel states.
   */
  const std::vector<HanabiState>& States() const {return parallel_states_;}

  /** \brief Get length of a single flattened encoded observation.
   */
  int GetObservationFlatLength() const;

//  /** \brief Number of parallel states in this environment.
//   */
//  int NumStates() const {return n_states_;};

//  /** \brief eplace specified states with new ones with agent as current player.
//   *
//   *  \param states           States to be reset.
//   *  \param current_agent_id Id of the agent whose turn it is now.
//   */
//  void ResetStates(const std::vector<int>& states, const int current_agent_id);

//  /** \brief Reset the environment, i.e. reset all states to initial.
//   */
//  void Reset();

//  /** \brief Convert observations to one-hot encoded representation.
//   */
//  std::vector<std::vector<int8_t>> EncodeObservations(
//      const std::vector<HanabiObservation>& observations) const;

//  /** \brief Get game status for each state.
//   */
//  std::vector<HanabiState::EndOfGameType> GetStateStatuses() const;

  /** \brief Get legal moves for each states for specified agent.
   *
   *  \param agent_id Id of the agent.
   */
  std::vector<std::vector<HanabiMove>> GetLegalMoves(const int agent_id) const;

  /** \brief One-hot encode legal moves.
   *
   *  \param legal_moves Legal moves to encode.
   */
  std::vector<std::vector<int8_t>> EncodeLegalMoves(
      const std::vector<std::vector<HanabiMove>>& legal_moves) const;
  std::vector<std::vector<int8_t>> EncodeLegalMoves(
      const std::vector<HanabiObservation>& observations) const;

//  /** \brief Get the current score for each state.
//   */
//  std::vector<int> GetScores() const;

  /** \brief Check whether the supplied moves can be applied.
   *  Number of moves must be equal to number of states.
   *
   *  \param moves moves.
   */
  std::vector<bool> MovesAreLegal(const std::vector<HanabiMove>& moves) const;

  const std::vector<HanabiObservation>& StateObservations() const {
    return observations_;
  }

//  /** \brief Get moves based on their unique ids.
//   */
//  std::vector<HanabiMove> GetMoves(const std::vector<int>& move_uids) const;

  /** \brief Replace illegal moves inplace (!) and return replace status.
   *  
   *  \param moves Moves.
   *  \param agent_id Agent id.
   *
   *  \return Boolean array where True means move was legal.
   */
  std::vector<bool> MakeLegal(
      std::vector<HanabiMove>& moves, const int agent_id) const;

//  /** \brief Make a step (smart version of apply batch moves).
//   *  The function catches all illegal moves and marks corresponding states as terminal.
//   *  Then it precomputes the outcome of the move.
//   *
//   *  The outcomes can be then be retrieved using accessor functions.
//   *
//   *  The function's goal is to do as much as possible in one call to minimize data
//   *  traffic and squeeze most benefit from parallel execution.
//   */
//  void Step(const std::vector<HanabiMove>& moves,
//            const int agent_id,
//            const int next_agent_id);
//  void Step(const std::vector<int>& moves_uids,
//            const int agent_id,
//            const int next_agent_id);

  // const std::vector<std::vector<int8_t>>& EncodedStateObservations() const {
  //   return encoded_observations_; }
  // const std::vector<std::vector<int8_t>>& EncodedLegalMoves() const {
  //   return encoded_legal_moves_; }
  const int8_t* EncodedStateObservations() const {
    return encoded_observations_.data(); }
  const int8_t* EncodedLegalMoves() const {
    return encoded_legal_moves_.data(); }
  const std::vector<bool>& IllegalMoves() const { return illegal_moves_; }

 private:
  /** \brief Create a new state and deal the cards.
   *
   *  \return New HanabiState with cards dealt to players.
   */
  HanabiState NewState();

  HanabiGame game_;                                     //< Underlying instance of HanabiGame.
  std::vector<HanabiState> parallel_states_;            //< List with game states.
  std::vector<HanabiObservation> observations_;         //< List with game state observations.
  std::vector<std::vector<int>> agent_player_mapping_;  //< List of players associated with each agent.
  std::vector<bool> illegal_moves_;                     //< List of moves which were illegal in the last step.
  CanonicalObservationEncoder observation_encoder_;     //< Observation encoder.
  const int n_states_ = 1;                              //< Number of parallel states.
  std::vector<int8_t> encoded_observations_;            //< Flattened encoded batch observation
  std::vector<int8_t> encoded_legal_moves_;             //< Flattened encoded legal moves
};

}  // namespace hanabi_learning_env

#endif // __HANABI_PARALLEL_ENV_H__
