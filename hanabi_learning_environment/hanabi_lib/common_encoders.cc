#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "common_encoders.h"

namespace hanabi_learning_env {

namespace {

/*
 * Computes the product of dimensions in shape, i.e. how many individual
 * pieces of data the encoded observation requires.
 */
int FlatLength(const std::vector<int>& shape) {
  return std::accumulate(std::begin(shape), std::end(shape), 1,
                         std::multiplies<int>());
}

/*
 * The card's one-hot index using a color-major ordering.
 */
int CardIndex(int color, int rank, int num_ranks) {
  return color * num_ranks + rank;
}


/*
 *
 */
int GetCardLength(const HanabiGame& game) {

	return game.NumColors() * game.NumRanks();

}


/*
 * create vector of length NumColors x NumRanks
 * contains the number of cards for each color-rank combination
 */
int InsertDefaultKnowledge(const HanabiGame& game,
		int start_offset, std::vector<int>* encoding) {

	int offset = start_offset;

	for (int i_color = 0; i_color < game.NumColors(); i_color++) {

		for (int i_rank = 0; i_rank < game.NumRanks(); i_rank++)
			(*encoding)[offset+i_rank] = game.NumberCardInstances(i_color, i_rank);

		offset += game.NumRanks();

	}

	assert(offset - start_offset == GetCardLength(game));
	return offset - start_offset;

}


/*
 *
 */
int ApplyBoardKnowledge(const HanabiGame& game, const HanabiObservation& obs,
		int start_offset, std::vector<int>* encoding) {

	int n_ranks = game.NumRanks();

	// discard pile
	for (const HanabiCard& card : obs.DiscardPile())
	  (*encoding)[start_offset + CardIndex(card.Color(), card.Rank(), n_ranks)]--;


	// fireworks
	for( int i_color = 0; i_color < game.NumColors(); i_color++) {

		for(int i_rank = 0; i_rank < obs.Fireworks()[i_color]; i_rank++)
			(*encoding)[start_offset + CardIndex(i_color, i_rank, n_ranks)]--;

	}

	return GetCardLength(game);

}

/*
 *
 */
int ApplyCardKnowledge(const HanabiGame& game,
		const HanabiHand::CardKnowledge& card_knowledge,
		int start_offset, std::vector<int>* encoding) {

	int n_ranks = game.NumRanks();
	int n_colors = game.NumColors();

	for (int i_color = 0; i_color < n_colors; i_color++) {

		for (int i_rank = 0; i_rank < n_ranks; i_rank++) {

			if(!card_knowledge.ColorPlausible(i_color) ||
					!card_knowledge.RankPlausible(i_rank)) {

				(*encoding)[start_offset + CardIndex(i_color, i_rank, n_ranks)] = 0;

			}
		}
	}

	return GetCardLength(game);

}


/*
 * Section includes knowledge about fireworks and available tokens (life and hints)
 */
int GetBoardLength(const HanabiGame& game) {

	return game.NumColors() + 2;
}

/*
 * encode knowledge about fireworks and available tokens (life and hints)
 */
int EncodeBoard(const HanabiGame& game, const HanabiObservation& obs,
		int start_offset, std::vector<int>* encoding) {

	int offset = start_offset;

	for( int i=0; i<game.NumColors(); i++)
		(*encoding)[offset++] = obs.Fireworks()[i];


	(*encoding)[offset++] = obs.LifeTokens();
	(*encoding)[offset++] = obs.InformationTokens();

	assert(offset - start_offset == GetBoardLength(game));
	return offset - start_offset;

}

/*
 *
 */
int GetCardKnowledgeLength(const HanabiGame& game) {

	return game.NumPlayers() * game.HandSize() * game.NumColors() * game.NumRanks();

}

/*
 *
 */
int EncodeCardKnowledge(const HanabiGame& game, const HanabiObservation& obs,
		int start_offset, std::vector<int>* encoding) {

	int offset = start_offset;
	const std::vector<HanabiHand>& hands = obs.Hands();

	for(int i_player = 0; i_player < game.NumPlayers(); i_player++) {

		const std::vector<HanabiHand::CardKnowledge>& knowledge =
		        hands[i_player].Knowledge();

		for(int i_card = 0; i_card < game.HandSize(); i_card++) {

			if (i_card < knowledge.size()) {

				const HanabiHand::CardKnowledge& card_knowledge = knowledge[i_card];

				// information that is not card specific
				// define how many cards of each type are available
				InsertDefaultKnowledge(game, offset, encoding);
				// apply the knowledge about the discard pile and fireworks
				ApplyBoardKnowledge(game, obs, offset, encoding);

				// apply knowledge about specific card
				ApplyCardKnowledge(game, card_knowledge, offset, encoding);

			}

			offset += GetCardLength(game);

		}

	}

	assert(offset - start_offset == GetCardKnowledgeLength(game));
	return offset - start_offset;

}


/*
 *
 */
int GetHandsLength(const HanabiGame& game) {
  return (game.NumPlayers() - 1) * game.HandSize() * GetCardLength(game) +
         game.NumPlayers();
}


/*
 * Encodes cards in all other player's hands (excluding our unknown hand),
 * and whether the hand is missing a card for all players (when deck is empty.)
 * Each card in a hand is encoded with a one-hot representation using
 * <num_colors> * <num_ranks> bits (25 bits in a standard game) per card.
 * Returns the number of entries written to the encoding.
 */
int EncodeHands(const HanabiGame& game, const HanabiObservation& obs,
                int start_offset, std::vector<int>* encoding) {
  int bits_per_card = GetCardLength(game);
  int num_ranks = game.NumRanks();
  int num_players = game.NumPlayers();
  int hand_size = game.HandSize();

  int offset = start_offset;
  const std::vector<HanabiHand>& hands = obs.Hands();
  assert(hands.size() == num_players);
  for (int player = 1; player < num_players; ++player) {
    const std::vector<HanabiCard>& cards = hands[player].Cards();
    int num_cards = 0;

    for (const HanabiCard& card : cards) {
      // Only a player's own cards can be invalid/unobserved.
      assert(card.IsValid());
      assert(card.Color() < game.NumColors());
      assert(card.Rank() < num_ranks);
      (*encoding)[offset + CardIndex(card.Color(), card.Rank(), num_ranks)] = 1;

      ++num_cards;
      offset += bits_per_card;
    }

    // A player's hand can have fewer cards than the initial hand size.
    // Leave the bits for the absent cards empty (adjust the offset to skip
    // bits for the missing cards).
    if (num_cards < hand_size) {
      offset += (hand_size - num_cards) * bits_per_card;
    }
  }

  // For each player, set a bit if their hand is missing a card.
  for (int player = 0; player < num_players; ++player) {
    if (hands[player].Cards().size() < game.HandSize()) {
      (*encoding)[offset + player] = 1;
    }
  }
  offset += num_players;

  assert(offset - start_offset == HandsSectionLength(game));
  return offset - start_offset;

}


/*
 *
 */
int GetDiscardsLength(const HanabiGame& game) {

	return GetCardLength(game);

}


/*
 *
 */
int EncodeDiscards(const HanabiGame& game, const HanabiObservation& obs,
        int start_offset, std::vector<int>* encoding) {

	const std::vector<HanabiCard>& discards = obs.DiscardPile();

	for (const HanabiCard& card : discards) {
		//std::cout << card.ToString() << "\n";
		(*encoding)[start_offset+CardIndex(card.Color(), card.Rank(), game.NumRanks())]+=1;
	}

	return GetDiscardsLength(game);

}


}


/*
 *
 */
std::vector<int> CommonObservationEncoder::Shape() const {

	return {GetBoardLength(*parent_game_)
		+ GetCardKnowledgeLength(*parent_game_)
		+ GetHandsLength(*parent_game_)
		+ GetDiscardsLength(*parent_game_)};

}


/*
 *
 */
std::vector<int> CommonObservationEncoder::Encode(const HanabiObservation& obs) const {

	// define encoding vector and starting position
	std::vector<int> encoding(FlatLength(Shape()), 0);
	int offset = 0;

	offset += EncodeCardKnowledge(*parent_game_, obs, offset, &encoding);
	offset += EncodeHands(*parent_game_, obs, offset, &encoding);
	offset += EncodeDiscards(*parent_game_, obs, offset, &encoding);
	offset += EncodeBoard(*parent_game_, obs, offset, &encoding);

	return encoding;





}

}
