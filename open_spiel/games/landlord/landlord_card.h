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

#ifndef __LANDLORD_CARD_H__
#define __LANDLORD_CARD_H__

#include <string>
#include <vector>

namespace landlord_learning_env {
// Constants.
constexpr int kNumSuits = 4;   // 基础牌花色数
constexpr int kNumCardsPerSuit = 13; // 基础牌每花色牌数量
constexpr int kNumCardsOfJoker = 2; // 王牌数量
constexpr int kNumCards = kNumSuits * kNumCardsPerSuit + kNumCardsOfJoker;  //牌数量
constexpr int kNumCardsIncludeFlower =kNumCards + 1; // 花牌数量，可以代替任意牌。
constexpr int kNumPlayers = 3;     // 玩家数量
constexpr int kNumCardsPerHand = 17;    //每玩家手牌数量
constexpr int kPoker3_RANK = 0; // 3的rank值
constexpr int kPokerA_RANK = 11; // A的rank值
constexpr int kPoker2_RANK = 12; // 2的rank值
constexpr int kPokerJoker_RANK = 13; //小王的rank值
constexpr int kPokerJOKER_RANK = 14; //大王的rank值
constexpr int MAX_STRAIGHT_RANK = 11; //连牌的最大rank值。
constexpr int MAX_RANK = 15; //poker的最大等级值（Flower花牌不能单独出）。
constexpr int RANK_COUNTS = 16; //poker的等级个数（包括了花牌）。
/*
55张牌（包括花牌），从0到54顺序编码。
*/
using Poker = int;
using SuitType = int;
using RankType = int;

using LandlordCard = std::pair<SuitType,RankType>;
constexpr LandlordCard InvalidCard = std::make_pair(-1,-1);

enum Suits{kDiamond=0,kClub,kHeart,kSpade,kNone};

inline bool isValidCard(LandlordCard card){
    return card.first >=0 && card.second >= 0;
}

inline LandlordCard poker2Card(const Poker poker){
    SuitType suit = poker / kNumCardsPerSuit;
    RankType rank = poker % kNumCardsPerSuit + (suit == Suits::kNone?kNumCardsPerSuit:0);
    return std::make_pair(suit,rank);
}

inline Poker card2Poker(LandlordCard card){
    SuitType suit = card.first;
    RankType rank = card.second;
    Poker poker = suit  * kNumCardsPerSuit + rank- (suit == Suits::kNone?kNumCardsPerSuit:0);
    return poker;
}

inline std::vector<LandlordCard> pokers2Cards(std::vector<Poker> pokers){
    std::vector<LandlordCard> cards;
    for (int i = 0; i < pokers.size();i++){
        cards.push_back(poker2Card(pokers[i]));
    }
    return cards;
}

inline std::vector<Poker> cards2Pokers(std::vector<LandlordCard> cards){
    std::vector<Poker> pokers;
    for (int i = 0; i < cards.size();i++){
        pokers.push_back(card2Poker(cards[i]));
    }
    return pokers;
}

inline std::string rank2DispString(RankType rank){
  return std::string{"3456789TJQKA2mMF"[rank]};
}
inline std::string card2String(LandlordCard card){
  std::string str =  card.first  == Suits::kNone?std::string():std::string()+"DCHS"[card.first];
  //str += "3456789TJQKA2mMF"[card.second];
  str += rank2DispString(card.second);
  return str;
}

inline std::string poker2String(Poker poker){
    //LandlordCard card = poker2Card(poker);
   //return card2String(card);
   return std::to_string(poker);
}

inline std::string cards2String(std::vector<LandlordCard> cards){
  std::string result;
  if (cards.size() > 0){
      for (int i = 0; i < cards.size()-1; ++i) {
        result += card2String( cards[i]) + " ";
      }
      result += card2String( cards[cards.size() -1]);
  }
  return result;
}

inline std::string pokers2String(std::vector<Poker> pokers){
  std::string result;
  if (pokers.size() > 0){
    for (int i = 0; i < pokers.size()-1; ++i) {
      result += poker2String( pokers[i]) + " ";
    }      
    result += poker2String( pokers[pokers.size() -1]);
  }
  return result;
}
inline void makeRankString(RankType rank,int count,std::string& str){
  for (size_t i = 0; i < count-1; i++)
  {
    str += rank2DispString(rank) +",";
  }
  
  str += rank2DispString(rank);
}
}  // namespace landlord_learning_env

#endif
