// Copyright 2018 Google LLC
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

#include "hanabi_observation.h"

#include <algorithm>
#include <numeric>
#include <cassert>

#include "util.h"

namespace hanabi_learning_env {

namespace {
// Returns the offset of player ID pid relative to player ID observer_pid,
// or pid for negative values. That is, offset such that for a non-negative
// player id pid, we have (observer_pid + offset) % num_players == pid.
int PlayerToOffset(int pid, int observer_pid, int num_players) {
  return pid >= 0 ? (pid - observer_pid + num_players) % num_players : pid;
}

// Switch members from absolute player indices to observer-relative offsets,
// including player indices within the contained HanabiMove.
void ChangeHistoryItemToObserverRelative(int observer_pid, int num_players,
                                         bool show_cards,
                                         HanabiHistoryItem* item) {
  if (item->move.MoveType() == HanabiMove::kDeal) {
    assert(item->player < 0 && item->deal_to_player >= 0);
    item->deal_to_player =
        (item->deal_to_player - observer_pid + num_players) % num_players;
    if (item->deal_to_player == 0 && !show_cards) {
      // Hide cards dealt to observer if they shouldn't be able to see them.
      item->move = HanabiMove(HanabiMove::kDeal, -1, -1, -1, -1);
    }
  } else {
    assert(item->player >= 0);
    item->player = (item->player - observer_pid + num_players) % num_players;
  }
}

//


}  // namespace

HanabiObservation::HanabiObservation(const HanabiState& state,
                                     int observing_player)
    : cur_player_offset_(PlayerToOffset(state.CurPlayer(), observing_player,
                                        state.ParentGame()->NumPlayers())),
      discard_pile_(state.DiscardPile()),
	  fireworks_pile_(state.FireworksPile()),
      fireworks_(state.Fireworks()),
      deck_size_(state.Deck().Size()),
      information_tokens_(state.InformationTokens()),
      life_tokens_(state.LifeTokens()),
      legal_moves_(state.LegalMoves(observing_player)),
      parent_game_(state.ParentGame()),
	  parent_state_(&state),
	  observing_player_(observing_player){
  REQUIRE(observing_player >= 0 &&
          observing_player < state.ParentGame()->NumPlayers());
  hands_.reserve(state.Hands().size());
  const bool hide_knowledge =
      state.ParentGame()->ObservationType() == HanabiGame::kMinimal;
  const bool show_cards = state.ParentGame()->ObservationType() == HanabiGame::kSeer;
  hands_.push_back(
      HanabiHand(state.Hands()[observing_player], !show_cards, hide_knowledge));
  for (int offset = 1; offset < state.ParentGame()->NumPlayers(); ++offset) {
    hands_.push_back(HanabiHand(state.Hands()[(observing_player + offset) %
                                              state.ParentGame()->NumPlayers()],
                                false, hide_knowledge));
  }

  const auto& history = state.MoveHistory();
  auto start = std::find_if(history.begin(), history.end(),
                            [](const HanabiHistoryItem& item) {
                              return item.player != kChancePlayerId;
                            });
  std::reverse_iterator<decltype(start)> rend(start);
  for (auto it = history.rbegin(); it != rend; ++it) {
    last_moves_.push_back(*it);
    ChangeHistoryItemToObserverRelative(observing_player,
                                        state.ParentGame()->NumPlayers(),
                                        show_cards,
                                        &last_moves_.back());
    if (it->player == observing_player) {
      break;
    }
  }
}

std::string HanabiObservation::ToString() const {
  std::string result;
  result += "Life tokens: " + std::to_string(LifeTokens()) + "\n";
  result += "Info tokens: " + std::to_string(InformationTokens()) + "\n";
  result += "Fireworks: ";
  for (int i = 0; i < ParentGame()->NumColors(); ++i) {
    result += ColorIndexToChar(i);
    result += std::to_string(fireworks_[i]) + " ";
  }
  result += "\nHands:\n";
  for (int i = 0; i < hands_.size(); ++i) {
    if (i > 0) {
      result += "-----\n";
    }
    if (i == CurPlayerOffset()) {
      result += "Cur player\n";
    }
    result += hands_[i].ToString();
  }
  result += "Deck size: " + std::to_string(DeckSize()) + "\n";
  result += "Discard Pile:";
  for (int i = 0; i < discard_pile_.size(); ++i) {
    result += " " + discard_pile_[i].ToString();
  }
  result += "\nFireworks Pile:";
  for (int i = 0; i < fireworks_pile_.size(); ++i) {
    result += " " + fireworks_pile_[i].ToString();
  }
  return result;
}

bool HanabiObservation::CardPlayableOnFireworks(int color, int rank) const {
  if (color < 0 || color >= ParentGame()->NumColors()) {
    return false;
  }
  return rank == fireworks_[color];
}


const std::vector<int> HanabiObservation::GetDefaultCardCounter() const {

	int num_ranks = ParentGame()->NumRanks();
	int num_colors = ParentGame()->NumColors();
	int num_players = ParentGame()->NumPlayers();
	int offset = 0;

	std::vector<int> output_vector(num_ranks*num_colors);

	for (int i_color = 0; i_color < num_colors; i_color++) {

		for (int i_rank = 0; i_rank < num_ranks; i_rank++) {
			// initial value
			int index = i_color * num_ranks + i_rank;
			output_vector[index] = ParentGame()->NumberCardInstances(i_color, i_rank);
			// fireworks
			if(i_rank < Fireworks()[i_color])
				output_vector[index]--;
		}
	}

	// discard pile
	for (const HanabiCard& card : DiscardPile())
		output_vector[card.Color() * num_ranks + card.Rank()]--;

	// other players
	for (int i_player = 1; i_player < num_players; i_player++) {
		for (const HanabiCard& card : Hands()[i_player].Cards())
			output_vector[card.Color() * num_ranks + card.Rank()]--;
	}

	return output_vector;

}


std::vector<double> HanabiObservation::PlayablePercent() const {

	// get the card knowledge of the active player
	const std::vector<HanabiHand::CardKnowledge>& knowledge = hands_[0].Knowledge();

	// create the result vector with placeholder for each card in hand
	std::vector<double> playable(knowledge.size(), 0.0);

	// get default card values
	std::vector<int> default_card_counter = GetDefaultCardCounter();

	int num_ranks = ParentGame()->NumRanks();
	int num_colors = ParentGame()->NumColors();
	int num_players = ParentGame()->NumPlayers();

	// loop through own card knowledge
	int counter = 0;

	for (const HanabiHand::CardKnowledge& card_knowledge : knowledge) {

		// copy default card values
		std::vector<int> this_card_counter = default_card_counter;
		int num_playable = 0;

		for( int i_color = 0; i_color < num_colors; i_color++) {

			for (int i_rank = 0; i_rank < num_ranks; i_rank++) {

				int index = i_color * num_ranks + i_rank;

				// card hints
				if(!card_knowledge.ColorPlausible(i_color) ||
						!card_knowledge.RankPlausible(i_rank))
					this_card_counter[index] = 0;

				// playable counter
				if (CardPlayableOnFireworks(i_color, i_rank))
					num_playable += this_card_counter[index];
			}
		}

		// count possible cards
		int num_total = std::accumulate(this_card_counter.begin(),
				this_card_counter.end(), 0);

		// calculate percentage
		playable[counter] = (float) num_playable / num_total;
		counter += 1;

	}

	return playable;
}

bool HanabiObservation::HandPossible(std::vector<HanabiCard>& hand) const {

	// get the card knowledge of the active player
	const std::vector<HanabiHand::CardKnowledge>& knowledge = hands_[0].Knowledge();

	// make sure that input vector of same length as hand size
	assert(knowledge.size() == hand.size());

	// get default card values
	std::vector<int> default_card_counter = GetDefaultCardCounter();

	// make sure that card fits card knowledge
	for (int i = 0; i < knowledge.size(); i++) {

		HanabiHand::CardKnowledge card_knowledge = knowledge[i];
		HanabiCard card = hand[i];

		if(!card_knowledge.ColorPlausible(card.Color()) ||
				!card_knowledge.RankPlausible(card.Rank())) {
			return false;
		}
	}

	return true;
}

const HanabiCard HanabiObservation::GetCardToDiscard(int index) const {
	const std::vector<HanabiHand> state_hands = parent_state_->Hands();
	const std::vector<HanabiCard> cards = state_hands[observing_player_].Cards();
	assert(index < cards.size());
	return cards[index];
}

int HanabiObservation::Score() const {
	if (LifeTokens() == 0) {
		return 0;
	} else {
		return std::accumulate(Fireworks().begin(), Fireworks().end(), 0);
	}
}

int HanabiObservation::MaximumScore() const {
	int max_score = ParentGame()->MaxScore();
	int num_colors = ParentGame()->NumColors();
	int num_ranks = ParentGame()->NumRanks();
	int rank_fw;
	int num_discards;

	// loop through all cards that still must be played
	for(int i_color=0; i_color<num_colors; i_color++) {

		rank_fw = Fireworks()[i_color];
		for(int i_rank=rank_fw; i_rank<num_ranks; i_rank++) {

			// count number of card instances in discard pile
			num_discards = 0;
			for (auto& card: DiscardPile()) {
				if((card.Color() == i_color) && (card.Rank() == i_rank)) {
					num_discards += 1;
				}
			}

			if(num_discards == ParentGame()->NumberCardInstances(i_color, i_rank)) {
				max_score -= (num_ranks - i_rank);
				break;
			}
		}
	}
	return max_score;
}

double HanabiObservation::PlayabilityIndicator() const {

	double playability_avg = parent_state_->AveragePlayability();
	double deck_value = deck_size_ ? (playability_avg > 0.8) : 0;

	double discards_value = 0;
	for(const HanabiCard& c : discard_pile_) {
		if(c.Playability() > 0.8)
			discards_value += 1;
	}

	double fireworks_value = 0;
	for(const HanabiCard& c : fireworks_pile_) {
		if (c.Playability() > 0.8)
			fireworks_value += 1;
	}

	// calculate value of hand cards
	double hands_value = 0;
	for (int i_player = 0; i_player < ParentGame()->NumPlayers(); i_player++) {

		for(const HanabiCard& c : hands_[i_player].Cards()) {
			if (c.Playability() > 0.8)
				hands_value += 1;
		}
	}

	return deck_value + discards_value + fireworks_value + hands_value;

}

double HanabiObservation::CardKnowledgeIndicator() const {

	// get average playability / discardability and calculate value of deck
/*	double playability_avg = parent_state_->AveragePlayability();
	double discardability_avg = parent_state_->AverageDiscardability();

	double deck_card_value = (playability_avg < discardability_avg) ?
			discardability_avg - playability_avg :
			playability_avg - discardability_avg;*/

	double deck_card_value = parent_state_->AverageKnowledge();
	double deck_value = deck_card_value * deck_size_;

	// calculate value of discards
/*	double discards_value = 0;
	for(const HanabiCard& c : discard_pile_)
		discards_value += (c.Discardability() - c.Playability());*/
	double discards_value = 0;
	for(const HanabiCard& c : discard_pile_)
		discards_value += c.CardKnowledge();

	// calculate value of fireworks
/*	double fireworks_value = 0;
	for(const HanabiCard& c : fireworks_pile_)
		fireworks_value += (c.Playability() - c.Discardability());*/
	double fireworks_value = 0;
	for(const HanabiCard& c : fireworks_pile_)
		fireworks_value += c.CardKnowledge();

	// calculate value of hand cards
	double hands_value = 0;
	for (int i_player = 0; i_player < ParentGame()->NumPlayers(); i_player++) {

		for(const HanabiCard& c : hands_[i_player].Cards()) {
/*			double hands_card_value = (c.Playability() < c.Discardability()) ?
					c.Discardability() - c.Playability() :
					c.Playability() - c.Discardability();
			hands_value += hands_card_value;*/
			hands_value += c.CardKnowledge();
		}
	}

	return deck_value + discards_value + fireworks_value + hands_value;

/*	double playability_avg = parent_state_->AveragePlayability();
	std::vector<double> card_knowledge = parent_state_->CommonPlayability();
	// calculate absolute value of difference
	for(auto& c : card_knowledge)
		c = (c < playability_avg) ? playability_avg - c : c - playability_avg;
	// sum of card knowledge
	return std::accumulate(card_knowledge.begin(), card_knowledge.end(), 0.0);*/
}


std::vector<double> HanabiObservation::DiscardablePercent() const {

	// get the card knowledge of the active player
	const std::vector<HanabiHand::CardKnowledge>& knowledge = hands_[0].Knowledge();

	// create the result vector with placeholder for each card in hand
	std::vector<double> discardable(knowledge.size(), 0.0);

	// get default card values
	std::vector<int> default_card_counter = GetDefaultCardCounter();
	// get discards
	std::vector<int> common_card_counter = parent_state_->GetCommonCardCounter();

	int num_ranks = ParentGame()->NumRanks();
	int num_colors = ParentGame()->NumColors();
	int num_players = ParentGame()->NumPlayers();

	// loop through own card knowledge
	int counter = 0;

	for (const HanabiHand::CardKnowledge& card_knowledge : knowledge) {

		// copy default card values
		std::vector<int> this_card_counter = default_card_counter;
		int num_discardable = 0;

		for( int i_color = 0; i_color < num_colors; i_color++) {

			for (int i_rank = 0; i_rank < num_ranks; i_rank++) {

				int index = i_color * num_ranks + i_rank;

				// card hints
				if(!card_knowledge.ColorPlausible(i_color) ||
						!card_knowledge.RankPlausible(i_rank))
					this_card_counter[index] = 0;

				// discardable counter
				// already played on fireworks
				if (i_rank < fireworks_[i_color]) {
					num_discardable += this_card_counter[index];
				// cards of some lower rank are already discarded
				} else {
					for (int i_rank_lower = fireworks_[i_color]; i_rank_lower < i_rank; i_rank_lower++) {
						if (common_card_counter[i_color*num_ranks + i_rank_lower] == 0) {
							num_discardable += this_card_counter[index];
							break;
						}
					}
				}
			}
		}

		// count possible cards
		int num_total = std::accumulate(this_card_counter.begin(),
				this_card_counter.end(), 0);

		// calculate percentage
		discardable[counter] = (float) num_discardable / num_total;
		counter += 1;

	}

	return discardable;
}

}  // namespace hanabi_learning_env
