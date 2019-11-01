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

#include "landlord_move.h"


namespace landlord_learning_env {
void LandlordMove::chkValid(){
  switch (move_type_)
  {
  case kPass/* constant-expression */:
    /* code */
    break;
  
  default:
    break;
  }

}
LandlordMove::LandlordMove(LandlordMoveType move_type, 
    std::vector <Poker>  pokers,int8_t laizi_rank = -1,int8_t tian_laizi_rank = -1)
      : move_type_(move_type),
        pokers_(pokers), laizi_rank_(laizi_rank),tian_laizi_rank_(tian_laizi_rank){
  int dilaiCount = 0,tianLaiCount = 0;
  for (Poker poker  : pokers_){
    LandlordCard card = poker2Card(poker);
    rankCounts_[card.second]++;
    if (card.second == laizi_rank_){
      dilaiCount++;
    }else if (card.second == tian_laizi_rank_){
      tianLaiCount++;
    }
  }
}

bool LandlordMove::operator==(const LandlordMove& other_move) const {
  if (MoveType() != other_move.MoveType()) {
    return false;
  }
  switch (MoveType()) {
    default:
      return true;
  }
}

std::string LandlordMove::ToString() const {
  switch (MoveType()) {
    case kPlay:
      return "(Play " + std::to_string(CardIndex()) + ")";
    case kDiscard:
      return "(Discard " + std::to_string(CardIndex()) + ")";
    case kRevealColor:
      return "(Reveal player +" + std::to_string(TargetOffset()) + " color " +
             ColorIndexToChar(Color()) + ")";
    case kRevealRank:
      return "(Reveal player +" + std::to_string(TargetOffset()) + " rank " +
             RankIndexToChar(Rank()) + ")";
    case kDeal:
      if (color_ >= 0) {
        return std::string("(Deal ") + ColorIndexToChar(Color()) +
               RankIndexToChar(Rank()) + ")";
      } else {
        return std::string("(Deal XX)");
      }
    default:
      return "(INVALID)";
  }
}

}  // namespace landlord_learning_env
