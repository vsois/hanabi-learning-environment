#ifndef __REPRESENTATIONS_H__
#define __REPRESENTATIONS_H__
#include "../hanabi_lib/hanabi_card.h"
#include "../hanabi_lib/hanabi_move.h"
#include "../hanabi_lib/hanabi_game.h"
#include "../hanabi_lib/hanabi_hand.h"
#include "../hanabi_lib/hanabi_history_item.h"
#include "../hanabi_lib/hanabi_state.h"
#include "../hanabi_lib/hanabi_observation.h"
#include "../hanabi_lib/hanabi_parallel_env.h"
#include "../hanabi_lib/canonical_encoders.h"
#include "../hanabi_lib/observation_encoder.h"

// HanabiCard
std::string hanabi_card_repr(const hanabi_learning_env::HanabiCard& card) {
  return "<HanabiCard(color=" + std::to_string(card.Color())
            + ", rank=" + std::to_string(card.Rank()) + ")>";
}

std::string hanabi_card_vec_repr(
    const std::vector<hanabi_learning_env::HanabiCard>& cards) {
  std::string repr("[");
  if (!cards.empty()) {
    for (size_t i = 0; i < cards.size() - 1; ++i) {
      repr += hanabi_card_repr(cards[i]) + ", ";
    }
    repr += hanabi_card_repr(cards.back());
  }
  repr += "]";
  return repr;
}

// HanabiMove
std::string hanabi_move_repr(const hanabi_learning_env::HanabiMove& move) {
  return "<HanabiMove("
    "type=" + std::to_string(move.MoveType())
    + ", card_index=" + std::to_string(move.CardIndex())
    + ", target_offset=" + std::to_string(move.TargetOffset())
    + ", color=" + std::to_string(move.Color())
    + ", rank=" + std::to_string(move.Rank())
    + ")>";
}

// HanabiGame
std::string hanabi_game_repr(const hanabi_learning_env::HanabiGame& game) {
  return "<HanabiGame("
    "num_players=" + std::to_string(game.NumPlayers())
    + ", max_score=" + std::to_string(game.MaxScore())
    + ", num_colors=" + std::to_string(game.NumColors())
    + ", num_ranks=" + std::to_string(game.NumRanks())
    + ", max_life_tokens=" + std::to_string(game.MaxLifeTokens())
    + ", max_information_tokens=" + std::to_string(game.MaxInformationTokens())
    + ", max_deck_size=" + std::to_string(game.MaxDeckSize())
    + ", hand_size=" + std::to_string(game.HandSize())
    + ")>";
}
// HanabiHand

std::string hanabi_valueknow_repr(
    const hanabi_learning_env::HanabiHand::ValueKnowledge& val_know) {
  return "<ValueKnowledge(value=" + std::to_string(val_know.Value()) + ")>";
}

std::string hanabi_cardknow_repr(
    const hanabi_learning_env::HanabiHand::CardKnowledge& card_know) {
  return "<CardKnowledge(" + card_know.ToString() + ")>";
}

std::string hanabi_cardknow_vec_repr(
    const std::vector<hanabi_learning_env::HanabiHand::CardKnowledge>& knows) {
  std::string repr("[");
  if (!knows.empty()) {
    for (size_t i = 0; i < knows.size() - 1; ++i) {
      repr += hanabi_cardknow_repr(knows[i]) + ", ";
    }
    repr += hanabi_cardknow_repr(knows.back()) + "]";
  }
  return repr;
}

std::string hanabi_hand_repr(const hanabi_learning_env::HanabiHand& hand) {
  return "<HanabiHand("
    "cards=" + hanabi_card_vec_repr(hand.Cards())
    + ", knowledge=" + hanabi_cardknow_vec_repr(hand.Knowledge())
    + ")>";
}

// HanabiHistoryItem
std::string hanabi_history_item_repr(
    const hanabi_learning_env::HanabiHistoryItem& hist) {
  return "<HanabiHistoryItem("
    "move=" + hanabi_move_repr(hist.move)
    + ", player=" + std::to_string(hist.player)
    + ", scored=" + std::to_string(hist.scored)
    + ", information_token=" + std::to_string(hist.information_token)
    + ", color=" + std::to_string(hist.color)
    + ", rank=" + std::to_string(hist.rank)
    + ")>";
}

// HanabiState
std::string hanabi_state_repr(const hanabi_learning_env::HanabiState& state) {
  return "<HanabiState(" + state.ToString() + ")>";
}

// HanabiObservation
std::string hanabi_observation_repr(
    const hanabi_learning_env::HanabiObservation& obs) {
  return "<HanabiObservation(" + obs.ToString()
    + ")>";
}

// HanabiObservation
std::string hanabi_parallel_env_repr(
    const hanabi_learning_env::HanabiParallelEnv& env) {
  return "<HanabiParallelEnv("
    "n_states=" + std::to_string(env.NumStates())
    + ")>";
}

// CanonicalObservationEncoder
std::string canonical_encoder_repr(const hanabi_learning_env::CanonicalObservationEncoder& enc) {
  return "<Canonical Observation Encoder("
    "type=" + std::to_string(enc.type())
    + ")>";
}

#endif // __REPRESENTATIONS_H__
