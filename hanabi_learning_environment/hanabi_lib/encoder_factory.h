#ifndef __HANABI_OBS_ENCODER_FACTORY_H__
#define __HANABI_OBS_ENCODER_FACTORY_H__

#include "observation_encoder.h"
#include "canonical_encoders.h"
#include "common_encoders.h"

namespace hanabi_learning_env {

class ObservationEncoderFactory {

public:
	static ObservationEncoder* Create(
			const ObservationEncoder::Type type,
			const HanabiGame* parent_game) {
		if (type == ObservationEncoder::Type::kCanonical) {
			return new CanonicalObservationEncoder(parent_game);
		}
		else if (type == ObservationEncoder::Type::kCommon) {
			return new CommonObservationEncoder(parent_game);
		}

	}

};

}  // namespace hanabi_learning_env

#endif
