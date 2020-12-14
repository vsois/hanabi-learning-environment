#include <functional>
#include <iterator>
#include <numeric>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include "hanabi_game.h"
#include "hanabi_move.h"
#include "hanabi_observation.h"
#include "hanabi_state.h"
#include "observation_encoder.h"
#include "util.h"
#include "hanabi_parallel_env.h"

/*
 * private function
 * create a new state and deal cards
 * return state object
 */
hanabi_learning_env::HanabiState hanabi_learning_env::HanabiParallelEnv::NewState() {

	HanabiState state(&game_);
	while (state.CurPlayer() == kChancePlayerId) {
		state.ApplyRandomChance();
	}
	return state;
}

/*
 *
 */
inline void MakeMoveLegal(
    std::vector<hanabi_learning_env::HanabiMove>::iterator move,
    const hanabi_learning_env::HanabiState& state,
    const int state_idx,
    const int player_id,
    std::vector<bool>& illegal_moves) {
  if (!state.MoveIsLegal(*move)) {
    // std::cout << "generating move" << std::endl;
    // std::cout << "state is terminal " << state.IsTerminal() << std::endl;
    // std::cout << "cur vs pid " << state.CurPlayer() << " vs " << player_id << std::endl;
    auto lm = state.LegalMoves(player_id).front();
    // std::cout << "correcting move" << std::endl;
    *move = lm;
    illegal_moves[state_idx] = true;
  } else {
    illegal_moves[state_idx] = false;
  }
}
/*
 * Constructor
 */
hanabi_learning_env::HanabiParallelEnv::HanabiParallelEnv(
		const std::unordered_map<std::string, std::string>& game_params,
		const int n_states)
: game_(HanabiGame(game_params)),
  observation_encoder_(&game_),
  n_states_(n_states),
  encoded_observations_(n_states * GetEncodedObservationFlatLength(), 0),
  encoded_legal_moves_(n_states * game_.MaxMoves(), 0),
  illegal_moves_(n_states, false)
{
		Reset();
}

/*
 * public
 * reset the environment in each state
 */
void hanabi_learning_env::HanabiParallelEnv::Reset() {

	parallel_states_.clear();
	agent_player_mapping_.clear();
	observations_.clear();

	const auto n_players = game_.NumPlayers();
	agent_player_mapping_.resize(n_players);

	for (int state_id = 0; state_id < n_states_; ++state_id) {

		// create state and deal cards, store result in vector
		parallel_states_.push_back(NewState());
		observations_.emplace_back(parallel_states_.back(), 0);

		// create player mapping
		for (size_t agent_idx = 0; agent_idx < n_players; ++agent_idx) {
			agent_player_mapping_[agent_idx].push_back(
					(parallel_states_.back().CurPlayer() + agent_idx) % n_players);
		}
	}
}

/*
 * public
 * reset the states given by the index vector
 * the agent_id identifies the active player
 */
void hanabi_learning_env::HanabiParallelEnv::ResetState(
		const std::vector<int>& vec_idx,
		const int current_agent_id) {

	#pragma omp parallel for
	for (size_t idx = 0; idx < vec_idx.size(); ++idx) {

		// reset the state
		const size_t state_idx = vec_idx[idx];
		auto& state = parallel_states_[state_idx];
		state = NewState();

		//std::cout << "get state " << parallel_states_[state_idx].ToString() << std::endl;

		// adjust player mapping in order to start with given player id
		for (int player_idx = 0; player_idx < game_.NumPlayers(); ++player_idx) {
			const int agent_id = (current_agent_id + player_idx) % game_.NumPlayers();
			const int corresponding_player_id = (state.CurPlayer() + player_idx) % game_.NumPlayers();
			agent_player_mapping_[agent_id][state_idx] = corresponding_player_id;
		}

		REQUIRE(!parallel_states_[state_idx].IsTerminal());
	}
}

/*
 * public
 * apply move to state
 */
void hanabi_learning_env::HanabiParallelEnv::ApplyBatchMove(
		const std::vector<HanabiMove>& batch_move, const int agent_id) {

	const auto& player_ids = agent_player_mapping_[agent_id];

	#pragma omp parallel for
	for (size_t state_idx = 0; state_idx < parallel_states_.size(); ++state_idx) {

		// get player and state
		const int cur_player = player_ids[state_idx];
		auto& state = parallel_states_[state_idx];
		REQUIRE(cur_player == state.CurPlayer());

		// apply move to state
		const auto& move = batch_move[state_idx];
		state.ApplyMove(move);

		// when card is discarded or played: deal new card to player
		int next_player = state.CurPlayer();
		while (next_player == kChancePlayerId) {
			state.ApplyRandomChance();
			next_player = state.CurPlayer();
		}
	}
}

/*
 * public
 * apply moves to states using move ids
 */
void hanabi_learning_env::HanabiParallelEnv::ApplyBatchMove(
		const std::vector<int>& batch_move, const int agent_id) {

	// create moves from move id
	std::vector<HanabiMove> moves;
	std::transform(batch_move.begin(),
			batch_move.end(),
			std::back_inserter(moves),
			[this](const int muid){ return game_.GetMove(muid); });

	// apply the moves
	ApplyBatchMove(moves, agent_id);
}

/*
 * public
 * check if move is legal, then apply move to state
 */
const std::vector<bool>&
//void
hanabi_learning_env::HanabiParallelEnv::Step(
		const std::vector<hanabi_learning_env::HanabiMove>& moves_orig,
		const int agent_id) {

	// create empty move vector and reserve storage
	std::vector<HanabiMove> moves;
	moves.reserve(n_states_);
	// copy the moves from input vector to new move vector
	std::copy(moves_orig.begin(), moves_orig.end(), std::back_inserter(moves));

	// map external player id to internal player id
	const auto& player_ids = agent_player_mapping_[agent_id];

  	#pragma omp parallel for
	for (size_t state_idx = 0; state_idx < n_states_; ++state_idx) {

		// make moves legal >> replace illegal move with discard card 0
		const auto pid = player_ids[state_idx];
		auto move = moves.begin() + state_idx;
		auto& state = parallel_states_[state_idx];
		MakeMoveLegal(move, state, state_idx, pid, illegal_moves_);

		// apply move
		state.ApplyMove(*move);

		// deal cards
		int chance_player = state.CurPlayer();
		while (chance_player == kChancePlayerId) {
			state.ApplyRandomChance();
			chance_player = state.CurPlayer();
		}
	}

	return illegal_moves_;
}

/*
 * public
 * check if move is legal, then apply moves to state by converting move uids to move objects
 */
const std::vector<bool>&
//void
hanabi_learning_env::HanabiParallelEnv::Step(
		const std::vector<int>& move_uids,
		const int agent_id) {

	// create moves from move id
	std::vector<HanabiMove> moves;
	moves.reserve(n_states_);
	std::transform(move_uids.begin(),
			move_uids.end(),
			std::back_inserter(moves),
			[this](const int muid){ return game_.GetMove(muid); });

	return Step(moves, agent_id);
	//Step(moves, agent_id);
}

/*
 * public
 * observe the current states from agent_id pov
 */
const std::vector<hanabi_learning_env::HanabiObservation>&
hanabi_learning_env::HanabiParallelEnv::ObserveAgent(const int agent_id) {

	// get internal player id
	const auto& player_ids = agent_player_mapping_[agent_id];

	#pragma omp parallel for
	for (size_t state_idx = 0; state_idx < parallel_states_.size(); ++state_idx) {
		const int player_idx = player_ids[state_idx];
		const auto& state = parallel_states_[state_idx];
		const auto& observation = HanabiObservation(state, player_idx);
		observations_[state_idx] = observation;
	}

	return observations_;
}

const hanabi_learning_env::HanabiObservation&
hanabi_learning_env::HanabiParallelEnv::ObserveAgent(int agent_id, int idx) {

	// get internal player id
	const auto& player_ids = agent_player_mapping_[agent_id];

	const int player_idx = player_ids[idx];
	const auto& state = parallel_states_[idx];
	const auto& observation = HanabiObservation(state, player_idx);
	observations_[idx] = observation;

	return observations_.at(idx);
}

/*
 * public
 * get the score values of the current states
 */
std::vector<int> hanabi_learning_env::HanabiParallelEnv::GetScore() const {

	std::vector<int> scores;

	std::transform(parallel_states_.begin(),
			parallel_states_.end(),
			std::back_inserter(scores),
			[](const hanabi_learning_env::HanabiState& state)
	{
		return state.Score();
	});
  return scores;
}

/*
 * public
 * encode the observations as integer vectors
 */
void hanabi_learning_env::HanabiParallelEnv::EncodeObservation(
		const std::vector<hanabi_learning_env::HanabiObservation>& observation) {

	std::vector<int8_t>::iterator enc_observation_begin;

	#pragma omp parallel for
	for (size_t state_idx = 0; state_idx < observation.size(); ++state_idx) {
		const auto eobs = observation_encoder_.Encode(observation[state_idx]);
		enc_observation_begin = encoded_observations_.begin() \
				+ state_idx * GetEncodedObservationFlatLength();
		std::copy(
				eobs.begin(),
				eobs.end(),
				enc_observation_begin);
	}
}

/*
 * public
 * encode legal moves as 1-hot encoded vector (1 if move legal else 0)
 */
//std::vector<std::vector<int8_t>>
//hanabi_learning_env::HanabiParallelEnv::EncodeLegalMoves(
//		const std::vector<hanabi_learning_env::HanabiObservation>& observation) const {
//
//	std::vector<std::vector<int8_t>> encoded_lms(n_states_);
//
//	#pragma omp parallel for
//	for (size_t state_idx = 0; state_idx < parallel_states_.size(); ++state_idx) {
//
//		const auto& state_lms = observation[state_idx].LegalMoves();
//		std::vector<int8_t> enc_state_lms(game_.MaxMoves(), 0);
//		for (const auto& lm : state_lms) {
//			enc_state_lms[game_.GetMoveUid(lm)] = 1;
//		}
//		encoded_lms[state_idx] = (enc_state_lms);
//	}
//	return encoded_lms;
//}
void hanabi_learning_env::HanabiParallelEnv::EncodeLegalMoves(
		const std::vector<hanabi_learning_env::HanabiObservation>& observation) {

	#pragma omp parallel for
	for (size_t state_idx = 0; state_idx < observation.size(); ++state_idx) {

		auto lm_iter = encoded_legal_moves_.begin() + state_idx * game_.MaxMoves();
		std::fill(lm_iter, lm_iter + game_.MaxMoves(), 0);
		for (const auto& lm : observation[state_idx].LegalMoves()) {
			*(lm_iter + game_.GetMoveUid(lm)) = 1;
		}
	}
}

/*
 * public
 * convert move ids to move objects
 */
std::vector<hanabi_learning_env::HanabiMove>
hanabi_learning_env::HanabiParallelEnv::GetMove(
		const std::vector<int>& move_uid) const {

	std::vector<HanabiMove> moves;
	moves.reserve(move_uid.size());
	std::transform(
			move_uid.begin(),
			move_uid.end(),
			std::back_inserter(moves),
			[this] (const int uid) { return game_.GetMove(uid); }
	);
  return moves;
}

/*
 * public
 * get the status of the current states, eg if game is finished
 */
std::vector<hanabi_learning_env::HanabiState::EndOfGameType>
hanabi_learning_env::HanabiParallelEnv::GetStateStatus() const {

	std::vector<HanabiState::EndOfGameType> status;
	for (const auto& state : parallel_states_) {
		status.push_back(state.EndOfGameStatus());
	}
  return status;
}

/*
 * public
 * get length of flattened encoded observation
 */
int hanabi_learning_env::HanabiParallelEnv::GetEncodedObservationFlatLength() const {
	const auto obs_shape = GetEncodedObservationShape();
	return std::accumulate(
			obs_shape.begin(),
			obs_shape.end(),
			1,
			std::multiplies<int>());
}

/*
 *
 */
hanabi_learning_env::HanabiParallelEnv::HanabiEncodedBatchObservation
hanabi_learning_env::HanabiParallelEnv::ObserveAgentEncoded(const int agent_id) {

	HanabiEncodedBatchObservation batch_observation(
			n_states_,
			GetEncodedObservationFlatLength(),
			game_.MaxMoves());

	const auto player_ids = agent_player_mapping_[agent_id];

	#pragma omp parallel for
	for (size_t state_idx = 0; state_idx < parallel_states_.size(); ++state_idx) {
		const int player_idx = player_ids[state_idx];
		const auto& state = parallel_states_[state_idx];
		const HanabiObservation observation(state, player_idx);
		const auto encoded_observation = observation_encoder_.Encode(observation);
		auto vec_observ_iter = batch_observation.observation.begin() +
				state_idx * encoded_observation.size();
		vec_observ_iter = std::copy(
				encoded_observation.begin(),
				encoded_observation.end(),
				vec_observ_iter);

		// gather legal moves
		auto lm_iter = batch_observation.legal_moves.begin() + state_idx * game_.MaxMoves();
		for (const auto& lm : state.LegalMoves(player_idx)) {
			*(lm_iter + game_.GetMoveUid(lm)) = 1;
		}
		batch_observation.scores[state_idx] = state.Score();
		batch_observation.done[state_idx] = state.IsTerminal();
	}
	return batch_observation;
}




