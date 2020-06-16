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

#include <cassert>
#include <cstring>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#include "hanabi_game.h"
#include "hanabi_state.h"
#include "hanabi_parallel_env.h"

std::vector<int> get_moves(const std::vector<int>& legal_moves, const int max_moves) {
  std::vector<int> moves;
  
  for (size_t j = 0; j < legal_moves.size() / max_moves; j++) {
    for (size_t i = 0; i < max_moves; i++) {
      if (legal_moves[j * max_moves + i] == 1) {
        moves.push_back(i);
        break;
      }
    }
  }
  return moves;
}

std::vector<int> term_states(const std::vector<bool>& done) {
  std::vector<int> ts;
  for (size_t i = 0; i < done.size(); i++) {
    if (done[i]) {
      ts.push_back(i);
    }
  }
  return ts;
}

void check_terminal(const std::vector<hanabi_learning_env::HanabiState>& states,
    const std::vector<int>& terminal) {
  for (size_t i = 0; i < states.size(); i++) {
    for (size_t j = 0; j < terminal.size(); j++) {
      if (states[i].IsTerminal() == terminal[j]) {
        break;
      }
      if (j == states.size() - 1) {
        std::cout << "1Found rogue terminal state " << i << std::endl;
      }
    }
  }
}

std::vector<int> check_any_terminal(const std::vector<hanabi_learning_env::HanabiState>& states) {
  std::vector<int> rogue;
  for (size_t i = 0; i < states.size(); i++) {
    if (states[i].IsTerminal()) {
      std::cout << "2Found rogue terminal state " << i << std::endl;
      rogue.push_back(i);
    }
  }
  return rogue;
}


void check_rogue_terminal_known(const std::vector<int>& rogue,
    const std::vector<int>& terminal) {
  for (size_t i = 0; i < rogue.size(); i++) {
    std::cout << "rogue state " << rogue[i];
    for (size_t j = 0; j < terminal.size(); j++) {
      if (rogue[i] == terminal[j]) {
        std::cout << " is known" << std::endl;
        break;
      }
    }
    std::cout << std::endl;
  }
}

void SimulateGames(const std::unordered_map<std::string, std::string>& game_params) {
  hanabi_learning_env::HanabiParallelEnv env(game_params, 1024);
  env.Reset();
  
  auto obs = env.ObserveAgent(0);
  size_t c = 0;
  int agent_id;
  while(c < 10000) {
    if (c % 1000 == 0) {
      std::cout << c << "cycles passed" << std::endl;
    }
    agent_id = c % env.GetGame().NumPlayers();
    obs = env.ObserveAgent(agent_id);

    auto term = term_states(obs.done);

    check_terminal(env.GetStates(), term);

    env.ResetStates(term, agent_id);

    auto rogue = check_any_terminal(env.GetStates());

    check_rogue_terminal_known(rogue, term);

    obs = env.ObserveAgent(agent_id);

    term = term_states(obs.done);

    check_terminal(env.GetStates(), term);

    env.ResetStates(term, agent_id);

    rogue = check_any_terminal(env.GetStates());

    check_rogue_terminal_known(rogue, term);
    
    env.ApplyBatchMove(get_moves(obs.legal_moves, env.MaxMoves()), agent_id);

    c++;
  }

}
//
//
//
//
//
//
//
//
//
//
// struct GameResult {
//   int score;
//   int fireworks_played;  // Number of successful Play moves.
//   int num_steps;         // Number of moves by a player.
// };
//
constexpr const char* kGameParamArgPrefix = "--config.hanabi.";
//
// GameResult SimulateGame(const hanabi_learning_env::HanabiGame& game,
//                         bool verbose, std::mt19937* rng) {
//   hanabi_learning_env::HanabiState state(&game);
//   GameResult result = {0, 0, 0};
//   while (!state.IsTerminal()) {
//     if (state.CurPlayer() == hanabi_learning_env::kChancePlayerId) {
//       // All of this could be replaced with state.ApplyRandomChance().
//       // Only done this way to demonstrate picking specific chance moves.
//       auto chance_outcomes = state.ChanceOutcomes();
//       std::discrete_distribution<std::mt19937::result_type> dist(
//           chance_outcomes.second.begin(), chance_outcomes.second.end());
//       auto move = chance_outcomes.first[dist(*rng)];
//       if (verbose) {
//         std::cout << "Legal chance:";
//         for (int i = 0; i < chance_outcomes.first.size(); ++i) {
//           std::cout << " <" << chance_outcomes.first[i].ToString() << ", "
//                     << chance_outcomes.second[i] << ">";
//         }
//         std::cout << "\n";
//         std::cout << "Sampled move: " << move.ToString() << "\n\n";
//       }
//       state.ApplyMove(move);
//       continue;
//     }
//
//     auto legal_moves = state.LegalMoves(state.CurPlayer());
//     std::uniform_int_distribution<std::mt19937::result_type> dist(
//         0, legal_moves.size() - 1);
//     auto move = legal_moves[dist(*rng)];
//     if (verbose) {
//       std::cout << "Current player: " << state.CurPlayer() << "\n";
//       std::cout << state.ToString() << "\n\n";
//       std::cout << "Legal moves:";
//       for (int i = 0; i < legal_moves.size(); ++i) {
//         std::cout << " " << legal_moves[i].ToString();
//       }
//       std::cout << "\n";
//       std::cout << "Sampled move: " << move.ToString() << "\n\n";
//     }
//     state.ApplyMove(move);
//     ++result.num_steps;
//     if (state.MoveHistory().back().scored) {
//       ++result.fireworks_played;
//     }
//   }
//
//   if (verbose) {
//     std::cout << "Game done, terminal state:\n" << state.ToString() << "\n\n";
//     std::cout << "score = " << state.Score() << "\n\n";
//   }
//
//   result.score = state.Score();
//   return result;
// }
//
// void SimulateGames(
//     const std::unordered_map<std::string, std::string>& game_params,
//     int num_trials = 1, bool verbose = true) {
//   std::mt19937 rng;
//   rng.seed(std::random_device()());
//
//   hanabi_learning_env::HanabiGame game(game_params);
//   auto params = game.Parameters();
//   std::cout << "Hanabi game created, with parameters:\n";
//   for (const auto& item : params) {
//     std::cout << "  " << item.first << "=" << item.second << "\n";
//   }
//
//   std::vector<GameResult> results;
//   results.reserve(num_trials);
//   for (int trial = 0; trial < num_trials; ++trial) {
//     results.push_back(SimulateGame(game, verbose, &rng));
//   }
//
//   if (num_trials > 1) {
//     GameResult avg_score = std::accumulate(
//         results.begin(), results.end(), GameResult(),
//         [](const GameResult& lhs, const GameResult& rhs) {
//           GameResult result = {lhs.score + rhs.score,
//                                lhs.fireworks_played + rhs.fireworks_played,
//                                lhs.num_steps + rhs.num_steps};
//           return result;
//         });
//     std::cout << "Average score: "
//               << static_cast<double>(avg_score.score) / results.size()
//               << " average number of fireworks played: "
//               << static_cast<double>(avg_score.fireworks_played) /
//                      results.size()
//               << " average num_steps: "
//               << static_cast<double>(avg_score.num_steps) / results.size()
//               << "\n";
//   }
// }
//
std::unordered_map<std::string, std::string> ParseArguments(int argc,
                                                            char** argv) {
  std::unordered_map<std::string, std::string> game_params;
  const auto prefix_len = strlen(kGameParamArgPrefix);
  for (int i = 1; i < argc; ++i) {
    std::string param = argv[i];
    if (param.compare(0, prefix_len, kGameParamArgPrefix) == 0 &&
        param.size() > prefix_len) {
      std::string value;
      param = param.substr(prefix_len, std::string::npos);
      auto value_pos = param.find("=");
      if (value_pos != std::string::npos) {
        value = param.substr(value_pos + 1, std::string::npos);
        param = param.substr(0, value_pos);
      }
      game_params[param] = value;
    }
  }
  return game_params;
}

int main(int argc, char** argv) {
  auto game_params = ParseArguments(argc, argv);
  SimulateGames(game_params);
  return 0;
}
