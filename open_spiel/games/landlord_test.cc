// Copyright 2019 DeepMind Technologies Ltd. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "open_spiel/game_parameters.h"
#include "open_spiel/spiel_utils.h"
#include "open_spiel/tests/basic_tests.h"
#include "landlord.h"
namespace open_spiel {
namespace landlord {
namespace {

namespace testing = open_spiel::testing;

void encodeTests(){
  Action action = encodeActionType(kBomb);
  assert(action ==((Action)kBomb) << ACTION_FLAG_LOC);
  action = encodeRankPoker(3,4);
  assert(action ==0x800);
  action = encodeActionType(kBomb) |  encodeRankPoker(3,4);
  assert(action ==0x000D000000000800L);
  LandlordMoveType type = decodeActionType(action);
  assert(type == kBomb);
  int count = decodeRankPoker(3,action);
  assert(count == 4);
  RankCountsArray rankCounts = {1,2,3,4,0,3,2,1};
  action = encodeRankCounts(rankCounts);
  RankCountsArray decCounts = decodeRankCounts(action);
  assert(rankCounts == decCounts);

  RankMove move(kSingle,0);
  action = rankMove2Action(move);
  assert(action=281474976710657L);

  move = RankMove(kPair,0);
  action = rankMove2Action(move);
  assert(action=562949953421314L);
  RankMove deMove = action2RankMove(action);
  assert (move == deMove);

  action = 281474976710672L;
  type = decodeActionType(action);
  rankCounts = decodeRankCounts(action);

}

void BasicLandlordTests() {
  testing::LoadGameTest("landlord");
  testing::NoChanceOutcomesTest(*LoadGame("landlord"));
  //testing::RandomSimTest(*LoadGame("landlord"), 100);
  testing::RandomSimTestNoSerialize(*LoadGame("landlord"), 1);
  
  // for (int players = 3; players <= 5; players++) {
  //   testing::RandomSimTest(
  //       *LoadGame("landlord", {{"players", GameParameter(players)}}), 100);
  // }
}

void SimpleLandlordTests() {
  std::shared_ptr<const Game> game = LoadGame("landlord");
  SPIEL_CHECK_TRUE(game != nullptr);
  std::unique_ptr<State> state = game->NewInitialState();

  SPIEL_CHECK_TRUE(state->IsChanceNode());
  std::vector<Action> legalActions = state->LegalActions();
  state->ApplyAction(legalActions[0]);  // Spawn B top-right
  std::cout << state->ToString() << std::endl;
   state->ApplyAction(BidAction::kBidThree);
   std::vector<double> obs;
   state->ObservationTensor(state->CurrentPlayer(),&obs);
}
}  // namespace
}  // namespace landlord
}  // namespace open_spiel

int main(int argc, char **argv) { 
  //open_spiel::landlord::SimpleLandlordTests();
  open_spiel::landlord::encodeTests();
  open_spiel::landlord::BasicLandlordTests(); 

}
