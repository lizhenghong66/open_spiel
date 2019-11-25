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
std::map<LandlordMoveType,std::string> MoveTypeStringMap = {
      {kPass,"kPass"},                      //过
      {kSingle , "kSingle"},                  //单 
      {kPair,"kPair"},                       //对
      {kThree,"kThree"},                   //三不带 （有些地方中途不允许，只能最后一手）
      {kThreeAddSingle,"kThreeAddSingle"},                               //三带一
      {kThreeAddPair,"kThreeAddPair"},                                //三带对
      {kFourAddTwoSingles,"kFourAddTwoSingles"},                   //四带两单牌
      {kFourAddTwoPairs,"kFourAddTwoPairs"},                       //四带两对牌
      {kStraight,"kStraight"},                                           //顺子 
      {kTwoStraight,"kTwoStraight"},                                 //拖拉机，连对
      {kThreeStraight,"kThreeStraight"},                             //飞机，没翅膀，3连牌
      {kThreeStraightAddOne,"kThreeStraightAddOne"},             // 飞机带单翅膀
      {kThreeStraightAddPair,"kThreeStraightAddPair"},           // 飞机带双翅膀

      {kBomb,"kBomb"},           // 炸弹，包括癞子炸弹，需要结合牌值编码比较大小。
      {kKingBomb,"kKingBomb"},           // 王炸，双王，多数玩法都是最大牌了。
      {kWeightBomb,"kWeightBomb"},           // 重炸，炸弹+花牌，允许花牌时最大牌了。
      {kKingWeightBomb,"kKingWeightBomb"},           // 重炸，双王+花牌，允许花牌时都是最大牌了。
      {kContinueBomb,"kContinueBomb"},           // 连炸（航天飞机）。允许连炸时最大牌。航天飞机
      {kContinueBombAddOne,"kContinueBombAddOne"},           // 航天飞机带单。允许连炸时最大牌。航天飞机
      {kContinueBombAddPair,"kContinueBombAddPair"},           // 航天飞机带对。允许连炸时最大牌。航天飞机
      {kInvalid,"kInvalid"},    //无效
};

std::string RankMove::toString() const{
        std::string str = "";
        //str =  std::to_string(type_)  +  ":"  +  std::to_string(startRank_) +  
        str =  MoveTypeStringMap[type_] +"(" + std::to_string(type_)  +  "):"  +  std::to_string(startRank_) +  
            " --> " + std::to_string(endRank_)  + ","  +  pokers2String(addedRanks_);
        return str;
}

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
    default:
      return "(INVALID)";
  }
}

std::string moveType2String(LandlordMoveType type){
  return MoveTypeStringMap[type];
}
}  // namespace landlord_learning_env
