#ifndef __COMMON_ENCODERS_H__
#define __COMMON_ENCODERS_H__

#include <vector>

#include "hanabi_game.h"
#include "hanabi_observation.h"
#include "observation_encoder.h"

namespace hanabi_learning_env {


class CommonObservationEncoder : public ObservationEncoder {

public:

	explicit CommonObservationEncoder(const HanabiGame* parent_game)
		: parent_game_(parent_game) {}

	std::vector<int> Shape() const override;
	std::vector<int> Encode(const HanabiObservation& obs) const override;

	ObservationEncoder::Type type() const override {
		return ObservationEncoder::Type::kCommon;
	}

private:

	const HanabiGame* parent_game_ = nullptr;

};


} // namespace hanabi_learning_env

#endif
