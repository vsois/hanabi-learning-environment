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
  struct HanabiEncodedBatchObservation {

	/** \brief Construct a batched observation of given shape.
	 *
	 *  \param n_states Number of states.
	 *  \param observation_len Length of a single flat encoded observation.
	 *  \param max_moves Total number of possible moves.
	 */
	HanabiEncodedBatchObservation(const int n_states, const int observation_len,
		const int max_moves)
	  : observation(n_states * observation_len, 0),
		legal_moves(n_states * max_moves, 0),
		scores(n_states, 0),
		done(n_states, false),
		observation_shape({n_states, observation_len}),
		legal_moves_shape({n_states, max_moves}) {
	}
	std::vector<int> observation; //< Concatenated flat encoded observations.
	std::vector<int> legal_moves; //< Concatenated legal moves.
	std::vector<int> scores;      //< Concatenated scores.
	std::vector<bool> done;       //< Concatenated termination statuses.
	std::array<int, 2> observation_shape{0, 0}; //< Shape of batched observation (n_states x encoded_observation_length).
	std::array<int, 2> legal_moves_shape{0, 0}; //< Shape of legal moves (n_states x max_moves).
  };

  /** \brief Construct an environment with a single game with several parallel states.
   *
   *  \param game_params Parameters of the game. See HanabiGame.
   *  \param n_states Number of parallel states.
   */
  explicit HanabiParallelEnv(
      const std::unordered_map<std::string, std::string>& game_params,
      const int n_states);

  /** \brief Reset the environment, i.e. reset all states to initial.
   */
  void Reset();

  /** \brief replace specified states with new ones with agent as current player.
   *
   *  \param states           States to be reset.
   *  \param current_agent_id Id of the agent whose turn it is now.
   */
  void ResetState(const std::vector<int>& vec_idx, const int current_agent_id);

  /** \brief Make a step: apply moves to states.
   *
   *  \param batch_move Moves, one for each state.
   */
  void ApplyBatchMove(
      const std::vector<HanabiMove>& batch_move, const int agent_id);

  /** \overload with moves encoded as move ids.
   */
  void ApplyBatchMove(
      const std::vector<int>& batch_move_ids, const int agent_id);

  /*
   * apply batch move
   * catch all illegal moves and mark corresponding states as terminal
   */
  const std::vector<bool>& Step(const std::vector<HanabiMove>& moves,
		  const int agent_id);
//  void Step(const std::vector<HanabiMove>& moves,
//		  const int agent_id);

  /*
   * apply batch move using move id
   * catch all illegal moves and mark corresponding states as terminal
   */
  const std::vector<bool>& Step(const std::vector<int>& moves_uids,
		  const int agent_id);
//  void Step(const std::vector<int>& moves_uids,
//		  const int agent_id);

  /** \brief Get observations for the specified agent.
   */
  const std::vector<HanabiObservation>& ObserveAgent(const int agent_id);
  const HanabiObservation& ObserveAgent(int agent_id, int idx);
  const std::vector<HanabiObservation> ObserveAgent(const int agent_id, std::vector<int> idx);

  HanabiEncodedBatchObservation ObserveAgentEncoded(const int agent_id);

  /** \brief Get the current score for each state.
   */
  std::vector<int> GetScore() const;

  /** \brief Convert observations to one-hot encoded representation.
   */
//  std::vector<std::vector<int8_t>> EncodeObservation(
//		  const std::vector<HanabiObservation>& observation) const;
  void EncodeObservation(
		  const std::vector<HanabiObservation>& observation);

  /** \brief Convert observations to one-hot encoded representation of legal moves.
   */
  void EncodeLegalMoves(
		  const std::vector<HanabiObservation>& observation);

  /** \brief Get moves based on their unique ids.
   */
  std::vector<HanabiMove> GetMove(const std::vector<int>& move_uid) const;

  /** \brief Get game status for each state.
   */
  std::vector<HanabiState::EndOfGameType> GetStateStatus() const;

  /** \brief Get shape of a single encoded observation.
   */
  std::vector<int> GetEncodedObservationShape() const {return observation_encoder_.Shape();};

  /** \brief Get length of a single flattened encoded observation.
   */
  int GetEncodedObservationFlatLength() const;

  /** \brief Get a reference to the HanabiGame game.
   */
  const HanabiGame& ParentGame() const {return game_;}

  /** \brief Get a pointer to the HanabiGame game.
   */
  HanabiGame* GetParentGamePtr() {return &game_;}
  const HanabiGame* GetParentGamePtr() const {return &game_;}

  /** \brief Get a const reference to the parallel states.
   */
  const std::vector<HanabiState>& States() const {return parallel_states_;}

  /** \brief Number of parallel states in this environment.
   */
  int NumStates() const {return n_states_;};

  const int8_t* EncodedStateObservations() const {
    return encoded_observations_.data(); }
  const int8_t* EncodedLegalMoves() const {
    return encoded_legal_moves_.data(); }


 private:
  /** \brief Create a new state and deal the cards.
   *
   *  \return New HanabiState with cards dealt to players.
   */
  HanabiState NewState();

  HanabiGame game_;                                     //< Underlying instance of HanabiGame.
  std::vector<HanabiState> parallel_states_;            //< List with game states.
  std::vector<std::vector<int>> agent_player_mapping_;  //< List of players associated with each agent.
  CanonicalObservationEncoder observation_encoder_;     //< Observation encoder.
  const int n_states_ = 1;                              //< Number of parallel states.
  std::vector<HanabiObservation> observations_;         //< List with game state observations.
  std::vector<HanabiObservation> observations_subset_;         //< List with game state observations.
  std::vector<int8_t> encoded_observations_;            //< Flattened encoded batch observation
  std::vector<int8_t> encoded_legal_moves_;             //< Flattened encoded legal moves
  std::vector<bool> illegal_moves_;                     //< List of moves which were illegal in the last step.

};

}  // namespace hanabi_learning_env

#endif // __HANABI_PARALLEL_ENV_H__
