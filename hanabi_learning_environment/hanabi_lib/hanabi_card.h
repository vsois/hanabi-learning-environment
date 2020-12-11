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

#ifndef __HANABI_CARD_H__
#define __HANABI_CARD_H__

#include <string>
#include <unordered_map>

namespace hanabi_learning_env {

class HanabiCard {
 public:
  enum ColorType : int { kRed = 0, kYellow = 1, kGreen = 2, kWhite = 3, kBlue = 4, kUnknownColor = -1 };
  enum RankType : int { k1 = 0, k2 = 1, k3 = 2, k4 = 3, k5 = 4, kUnknownRank = -1 };
  HanabiCard(ColorType color, RankType rank) : color_(color), rank_(rank) {}
  HanabiCard(int color, int rank) : color_(ColorType(color)), rank_(RankType(rank)) {}
  HanabiCard() = default;  // Create an invalid card.
  bool operator==(const HanabiCard& other_card) const;
  bool IsValid() const { return color_ >= 0 && rank_ >= 0; }
  std::string ToString() const;
  int Color() const { return color_; }
  int Rank() const { return rank_; }
  size_t Hash() const {return hasher_(ToString());}

 private:
  ColorType color_ = ColorType::kUnknownColor;  // 0 indexed card color.
  RankType rank_ = RankType::kUnknownRank;   // 0 indexed card rank.
  std::hash<std::string> hasher_;
};

}  // namespace hanabi_learning_env

#endif
