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

template<typename EncodingPrecision>
inline void EncodeObservation(
    hanabi_learning_env::ObservationEncoder& encoder,
    const hanabi_learning_env::HanabiObservation& observation,
    typename std::vector<EncodingPrecision>::iterator enc_observation_begin,
    const std::vector<hanabi_learning_env::HanabiMove>& legal_moves,
    typename std::vector<EncodingPrecision>::iterator enc_legal_moves_begin,
    const hanabi_learning_env::HanabiGame& game
    ) {
  const auto encoded_observation = encoder.Encode(observation);
  // auto vec_observ_iter = observation.observation.begin() +
  //     state_idx * encoded_observation.size();
  std::copy(encoded_observation.begin(), encoded_observation.end(),
            enc_observation_begin);
  // auto lm_iter =
  //     batch_observation_enc_.legal_moves.begin() + state_idx * game_.MaxMoves();
  auto lm_iter = enc_legal_moves_begin;
  std::fill(lm_iter, enc_legal_moves_begin + game.MaxMoves(), 0);
  for (const auto& lm : legal_moves) {
    *(enc_legal_moves_begin + game.GetMoveUid(lm)) = 1;
  }

}

hanabi_learning_env::HanabiParallelEnv::HanabiParallelEnv(
    const std::unordered_map<std::string,
    std::string>& game_params,
    const int n_states)
  : game_(HanabiGame(game_params)),
    observation_encoder_(&game_),
    n_states_(n_states),
    encoded_observations_(n_states, std::vector<int8_t>(GetObservationFlatLength(), 0)),
    encoded_legal_moves_(n_states, std::vector<int8_t>(game_.MaxMoves(), 0)),
    illegal_moves_(n_states, false)
{
  Reset();
  ObserveAgent(0);
}

void hanabi_learning_env::HanabiParallelEnv::Reset() {
  parallel_states_.clear();
  agent_player_mapping_.clear();
  observations_.clear();

  const auto n_players = game_.NumPlayers();
  agent_player_mapping_.resize(n_players);
  for (int state_id = 0; state_id < n_states_; ++state_id) {
    parallel_states_.push_back(NewState());
    observations_.emplace_back(parallel_states_.back(), 0);
    for (size_t agent_idx = 0; agent_idx < n_players; ++agent_idx) {
      agent_player_mapping_[agent_idx].push_back(
          (parallel_states_.back().CurPlayer() + agent_idx) % n_players);
    }
  }
}

hanabi_learning_env::HanabiState
hanabi_learning_env::HanabiParallelEnv::NewState() {
  HanabiState state(&game_);
  while (state.CurPlayer() == kChancePlayerId) {
    state.ApplyRandomChance();
  }
  return state;
}

std::vector<int> hanabi_learning_env::HanabiParallelEnv::GetScores() const {
  std::vector<int> scores;
  std::transform(parallel_states_.begin(), parallel_states_.end(),
                 std::back_inserter(scores),
                 [](const hanabi_learning_env::HanabiState& state) {
                    return state.Score(); });
  return scores;
}

void hanabi_learning_env::HanabiParallelEnv::ResetStates(
    const std::vector<int>& states,
    const int current_agent_id) {
  #pragma omp parallel for
  for (size_t idx = 0; idx < states.size(); ++idx) {
    const size_t state_idx = states[idx];
    auto& state = parallel_states_[state_idx];
    state = NewState();
    for (int player_idx = 0; player_idx < game_.NumPlayers(); ++player_idx) {
      const int agent_id =
        (current_agent_id + player_idx) % game_.NumPlayers();
      const int corresponding_player_id =
        (state.CurPlayer() + player_idx) % game_.NumPlayers();
      agent_player_mapping_[agent_id][state_idx] = corresponding_player_id;
    }
    REQUIRE(!parallel_states_[state_idx].IsTerminal());
  }
}

void hanabi_learning_env::HanabiParallelEnv::ApplyBatchMove(
    const std::vector<HanabiMove>& batch_move, const int agent_id) {
  const auto& player_ids = agent_player_mapping_[agent_id];
  #pragma omp parallel for
  for (size_t state_idx = 0; state_idx < parallel_states_.size(); ++state_idx) {
    const int cur_player = player_ids[state_idx];
    auto& state = parallel_states_[state_idx];
    REQUIRE(cur_player == state.CurPlayer());
    const auto& move = batch_move[state_idx];
    state.ApplyMove(move);
    int next_player = state.CurPlayer();
    while (next_player == kChancePlayerId) {
      state.ApplyRandomChance();
      next_player = state.CurPlayer();
    }
  }
}

template<typename T>
void print_enc_obs(std::vector<T> obs) {
  std::cout << "moves cpp: ";
  for (const auto x : obs) {
    std::cout << x;
  }
  std::cout << std::endl;
}

void hanabi_learning_env::HanabiParallelEnv::ApplyBatchMove(
    const std::vector<int>& batch_move, const int agent_id) {
  std::vector<HanabiMove> moves;
  std::transform(batch_move.begin(),
                 batch_move.end(),
                 std::back_inserter(moves),
                 [this](const int muid){ return game_.GetMove(muid); });
  ApplyBatchMove(moves, agent_id);
}

int hanabi_learning_env::HanabiParallelEnv::GetObservationFlatLength() const {
  const auto obs_shape = EncodedObservationShape();
  return std::accumulate(
            obs_shape.begin(), obs_shape.end(), 1, std::multiplies<int>());
}


hanabi_learning_env::HanabiParallelEnv::HanabiEncodedBatchObservation
hanabi_learning_env::HanabiParallelEnv::ObserveAgentEncoded(const int agent_id) {
  HanabiEncodedBatchObservation batch_observation(
      n_states_, GetObservationFlatLength(), game_.MaxMoves());
  const auto player_ids = agent_player_mapping_[agent_id];
  #pragma omp parallel for
  for (size_t state_idx = 0; state_idx < parallel_states_.size(); ++state_idx) {
    const int player_idx = player_ids[state_idx];
    const auto& state = parallel_states_[state_idx];
    const HanabiObservation observation(state, player_idx);
    const auto encoded_observation = observation_encoder_.Encode(observation);
    auto vec_observ_iter = batch_observation.observation.begin() +
        state_idx * encoded_observation.size();
    vec_observ_iter = std::copy(encoded_observation.begin(),
                                encoded_observation.end(),
                                vec_observ_iter);
    // gather legal moves
    auto lm_iter =
        batch_observation.legal_moves.begin() + state_idx * game_.MaxMoves();
    for (const auto& lm : state.LegalMoves(player_idx)) {
      *(lm_iter + game_.GetMoveUid(lm)) = 1;
    }
    batch_observation.scores[state_idx] = state.Score();
    batch_observation.done[state_idx] = state.IsTerminal();
  }
  return batch_observation;
}


const std::vector<hanabi_learning_env::HanabiObservation>&
hanabi_learning_env::HanabiParallelEnv::ObserveAgent(const int agent_id) {
  const auto& player_ids = agent_player_mapping_[agent_id];
  // std::vector<HanabiObservation> observs;
  // observs.reserve(n_states_);
  // std::transform(
  //     parallel_states_.begin(),
  //     parallel_states_.end(),
  //     player_ids.begin(),
  //     std::back_inserter(observs),
  //     [](const HanabiState& s, const int pid) {
  //       return HanabiObservation(s, pid);
  //     }
  // );
  #pragma omp parallel for
  for (size_t state_idx = 0; state_idx < parallel_states_.size(); ++state_idx) {
    const int player_idx = player_ids[state_idx];
    const auto& state = parallel_states_[state_idx];
    const auto& observation = HanabiObservation(state, player_idx);
    observations_[state_idx] = observation;
    // encode
    EncodeObservation<decltype(encoded_observations_)::value_type::value_type>(
        observation_encoder_,
        observation,
        encoded_observations_[state_idx].begin(),
        state.LegalMoves(player_idx),
        encoded_legal_moves_[state_idx].begin(),
        game_
    );
  }
  return observations_;
  // return observs;
  // return {};
}

std::vector<std::vector<int8_t>>
hanabi_learning_env::HanabiParallelEnv::EncodeObservations(
    const std::vector<hanabi_learning_env::HanabiObservation>& observations) const {
  std::vector<std::vector<int8_t>> encoded_observations(n_states_);
  #pragma omp parallel for
  for (size_t state_idx = 0; state_idx < parallel_states_.size(); ++state_idx) {
    const auto eobs = observation_encoder_.Encode(observations[state_idx]);
    // encoded_observations[state_idx] = std::move(eobs);
    std::move(eobs.begin(), eobs.end(), std::back_inserter(encoded_observations[state_idx]));
  }
  return encoded_observations;
}

std::vector<hanabi_learning_env::HanabiState::EndOfGameType>
hanabi_learning_env::HanabiParallelEnv::GetStateStatuses() const {
  std::vector<HanabiState::EndOfGameType> statuses;
  for (const auto& state : parallel_states_) {
    statuses.push_back(state.EndOfGameStatus());
  }
  return statuses;
}

std::vector<std::vector<hanabi_learning_env::HanabiMove>>
hanabi_learning_env::HanabiParallelEnv::GetLegalMoves(const int agent_id) const {
  std::vector<std::vector<HanabiMove>> lms;
  const auto player_ids = agent_player_mapping_[agent_id];
  for (size_t state_idx = 0; state_idx < parallel_states_.size(); ++state_idx) {
    const int player_idx = player_ids[state_idx];
    const auto& state = parallel_states_[state_idx];
    lms.push_back(state.LegalMoves(player_idx));
  }
  return lms;
}

std::vector<std::vector<int8_t>>
hanabi_learning_env::HanabiParallelEnv::EncodeLegalMoves(
    const std::vector<hanabi_learning_env::HanabiObservation>& observations) const {
  std::vector<std::vector<int8_t>> encoded_lms(n_states_);
  #pragma omp parallel for
  for (size_t state_idx = 0; state_idx < parallel_states_.size(); ++state_idx) {
    const auto& state_lms = observations[state_idx].LegalMoves();
    std::vector<int8_t> enc_state_lms(game_.MaxMoves(), 0);
    for (const auto& lm : state_lms) {
      enc_state_lms[game_.GetMoveUid(lm)] = 1;
    }
    encoded_lms[state_idx] = (enc_state_lms);
  }
  return encoded_lms;
}

std::vector<std::vector<int8_t>>
hanabi_learning_env::HanabiParallelEnv::EncodeLegalMoves(
    const std::vector<std::vector<hanabi_learning_env::HanabiMove>>& lms) const {
  std::vector<std::vector<int8_t>> encoded_lms(n_states_);
  #pragma omp parallel for
  for (size_t state_idx = 0; state_idx < parallel_states_.size(); ++state_idx) {
    const auto& state_lms = lms[state_idx];
    std::vector<int8_t> enc_state_lms(game_.MaxMoves(), 0);
    for (const auto& lm : state_lms) {
      enc_state_lms[game_.GetMoveUid(lm)] = 1;
    }
    encoded_lms[state_idx] = (enc_state_lms);
  }
  return encoded_lms;
}

std::vector<bool> hanabi_learning_env::HanabiParallelEnv::MovesAreLegal(
    const std::vector<hanabi_learning_env::HanabiMove>& moves) const {
  std::vector<bool> legal(n_states_);
  std::transform(moves.begin(), moves.end(), parallel_states_.begin(),
      legal.begin(), [](const HanabiMove& m, const HanabiState& s) {
        return s.MoveIsLegal(m);
      }
  );
  return legal;
}

std::vector<hanabi_learning_env::HanabiMove>
hanabi_learning_env::HanabiParallelEnv::GetMoves(
    const std::vector<int>& move_uids) const {
  std::vector<HanabiMove> moves;
  moves.reserve(move_uids.size());
  std::transform(
      move_uids.begin(), move_uids.end(),
      std::back_inserter(moves),
      [this] (const int move_uid) { return game_.GetMove(move_uid); }
  );
  // for (size_t move_idx = 0; move_idx < move_uids.size(); move_idx++) {
  //   moves[move_idx] = game_.GetMove(move_uids[move_idx]);
  // }
  return moves;
}

std::vector<bool> hanabi_learning_env::HanabiParallelEnv::MakeLegal(
    std::vector<hanabi_learning_env::HanabiMove>& moves, const int agent_id) const {
  const auto& player_ids = agent_player_mapping_[agent_id];
  std::vector<bool> legal(n_states_, true);
  // auto moves_iter = moves.begin();
  // auto legal_iter = legal.begin();
  // auto states_iter = parallel_states_.cbegin();
  // auto pid_iter = player_ids.cbegin();
  #pragma omp parallel for
  for (size_t state_idx = 0; state_idx < n_states_; ++state_idx) {
    auto& move = moves[state_idx];
    const auto& state = parallel_states_[state_idx];
    if (!state.MoveIsLegal(move)) {
      move = state.LegalMoves(player_ids[state_idx]).front();
      legal[state_idx] = false;
    }
  }
  // for (; moves_iter < moves.end(); ++moves_iter, ++states_iter, ++pid_iter, ++legal_iter) {
  //   *legal_iter = states_iter->MoveIsLegal(*moves_iter);
  //   if (!*legal_iter) {
  //     *moves_iter = states_iter->LegalMoves(*pid_iter)[0];
  //   }
  // }
  return legal;
}

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


void hanabi_learning_env::HanabiParallelEnv::Step(
    const std::vector<hanabi_learning_env::HanabiMove>& moves_orig, const int agent_id, const int next_agent_id) {
  std::vector<HanabiMove> moves;
  moves.reserve(n_states_);
  std::copy(moves_orig.begin(), moves_orig.end(), std::back_inserter(moves));
  const auto& player_ids = agent_player_mapping_[agent_id];
  const auto& next_player_ids = agent_player_mapping_[next_agent_id];
  // #pragma omp parallel for
  for (size_t state_idx = 0; state_idx < n_states_; ++state_idx) {
    // make moves legal
    const auto pid = player_ids[state_idx];
    auto move = moves.begin() + state_idx;
    auto& state = parallel_states_[state_idx];
    // std::cout << "make legal" << std::endl;
    MakeMoveLegal(move, state, state_idx, pid, illegal_moves_);
    // apply moves
    // std::cout << "apply_move" << std::endl;
    state.ApplyMove(*move);

    // deal cards 
    int chance_player = state.CurPlayer();
    while (chance_player == kChancePlayerId) {
      state.ApplyRandomChance();
      chance_player = state.CurPlayer();
    }
    // reobserve as next agent
    const auto next_pid = next_player_ids[state_idx];
    const auto& observation = HanabiObservation(state, next_pid);
    observations_[state_idx] = observation;

    // encode
    // std::cout << "encode" << std::endl;
    EncodeObservation<decltype(encoded_observations_)::value_type::value_type>(
        observation_encoder_,
        observation,
        encoded_observations_[state_idx].begin(),
        state.LegalMoves(next_pid),
        encoded_legal_moves_[state_idx].begin(),
        game_
    );
  }
}
void hanabi_learning_env::HanabiParallelEnv::Step(
    const std::vector<int>& move_uids, const int agent_id, const int next_agent_id) {
  std::vector<HanabiMove> moves;
  moves.reserve(n_states_);
  std::transform(move_uids.begin(),
                 move_uids.end(),
                 std::back_inserter(moves),
                 [this](const int muid){ return game_.GetMove(muid); });
  Step(moves, agent_id, next_agent_id);
}
