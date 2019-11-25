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

#ifndef THIRD_PARTY_OPEN_SPIEL_GAMES_LANDLORD_H_
#define THIRD_PARTY_OPEN_SPIEL_GAMES_LANDLORD_H_

// Game of landlord:
// https://baike.baidu.com/item/%E6%96%97%E5%9C%B0%E4%B8%BB/177997?fr=aladdin
// 斗地主玩法比较多，可以考虑通过配置选择。
//  这里只考虑3人斗地主
 // Parameters: 
//  玩法选项:
 //1. isAllowAddPair                            true   #是否允许带对（默认允许），个别地方玩法不允许带对。
 //2. isAllowContinuableBomb      fasle  # 是否允许连炸，如  44445555，最大炸弹，结算时算3炸。
 //3. isThreeNoAddMustLastPut    false  # 三不带牌是否必须最后一手出，中途不允许。
 //4. isUsedWildCard                          false  # 是否使用百搭牌（花牌，55张牌）,线下癞子玩法，默认为false。花牌代替任意牌，不能单出。
 //                                                                            # 使用花牌时，容易出现重炸（炸弹+花牌，或普通炸弹+花牌），结算时算2炸。
 //5. isUseLaizi                                      false    # 王以外的牌选一个做癞子，可以 代替王以外的任意牌，单独出时只能是本牌
 //6. isUseTiandiLaizi                        false    # 天地癞子，王以外的牌选两个癞子，可以 代替王以外的任意牌，单独出时只能是本牌                            
 // 洗牌方位：第一次随机选择开始洗牌方位，后面都是上一局地主方位洗牌。
 // 洗牌方式： 1.  每次随机洗牌    
//                         2.  不洗牌，即第一次随机洗牌，后面将玩家剩余手牌（保留顺序）追加到打出的牌中，然后随机倒几次牌。
//                         3.  倒牌参数：最小次数（0），最大次数（0），最小牌数（1），最大牌数（牌张数-1）。                     
 //  开始发牌方位：1.  洗牌方位的下家。   2.  发牌前随机翻一张牌(地主牌))，根据牌面值确定发牌方位。
 //  发牌方式：1.  每次发牌1张   
//                         2.   每次直接发牌17张   
 //                        3.  如有翻地主牌，可分两次发牌，第一次发翻出地主牌面值张数的牌，后面再发剩余的牌。
 //                        4.  如有翻地主牌，先取出地主补充牌，第一次发翻出地主牌面值张数的牌，后面再发剩余的牌。
//  开始叫牌方位：1.  开始发牌方位。   2.  如有翻出的地主牌，则获得地主牌的玩家开始叫牌。
 //  叫牌选项： 1.   典型网络叫牌，1---3分，分最高者当地主。
//                          2.   线下叫法，只有一次机会，叫地主后其他玩家没机会了。
//                          3.   是否允许闷（不给其他玩家看底牌机会），输赢加倍。
//                          4.  是否尾家必闷（允许闷牌的前提下）
//                          5.  是否允许倒牌（加倍玩家输赢加倍，没没到牌玩家不变）
//                          6.  弃庄的农民是否允许跟倒   
//                          7.  庄家（地主）是否允许拉牌（再加倍，只针对参与倒牌玩家）。
//  暂时只考虑实现最基本的网络斗地主玩法（基本牌，只叫分，叫分这部分也考虑先hardcode）。

#include <memory>
#include <array>
#include "open_spiel/spiel.h"
#include "open_spiel/games/landlord/landlord_deck.h"
#include "open_spiel/games/landlord/landlord_parser.h"
#include "open_spiel/games/landlord/landlord_action.h"

using  namespace landlord_learning_env;
namespace open_spiel {
namespace landlord {
//参考chess.h定义了一些常量和action的编码
// Constants.
constexpr int NumPlayers = 3;   //暂时只考虑3人斗地主
// See action encoding below.
constexpr int NumDistinctActions= (1 << 15); 

enum BidAction{kPass,kOne,kTwo,kThree};
//landlord action encode
//0--47 bit,每3bit记录一个rank 扑克的数量（0--4）
//48-52，5bit记录出牌类型（action Type）。
constexpr int ACTION_FLAG_LOC = 48;
constexpr Action ACTION_FLAG = 0x1F;
constexpr int RANK_FLAG_LEN = 3;
constexpr Action RANK_FLAG = 7;

inline Action encodeActionType(LandlordMoveType type){
    Action action = type;
   action =  action <<  ACTION_FLAG_LOC;
  return action;
}

inline Action encodeRankPoker(RankType rank,int count){
  Action action = ((Action)count) << RANK_FLAG_LEN * rank;
  return action;
}

inline LandlordMoveType decodeActionType(Action action){
  LandlordMoveType type;
  Action flag = ACTION_FLAG << ACTION_FLAG_LOC;
  flag = action & flag;
  flag = flag >> ACTION_FLAG_LOC;
   type = (LandlordMoveType)flag;
  return type;
}

inline int decodeRankPoker(RankType rank,Action action){
  Action flag = RANK_FLAG << rank * RANK_FLAG_LEN;
  flag = action & flag;
  flag = flag >> rank * RANK_FLAG_LEN;

  return flag;
}

inline Action encodeRankCounts(RankCountsArray &rankCounts){
  Action action = 0;
  for (int rank = 0; rank < RANK_COUNTS;rank++){
    if (rankCounts[rank] > 0){
      action |= encodeRankPoker(rank,rankCounts[rank]);
    }
  }
  return action;
}

inline RankCountsArray decodeRankCounts( Action action){
 RankCountsArray rankCounts = {0};
  for (int rank = 0; rank < RANK_COUNTS;rank++){
    rankCounts[rank] = decodeRankPoker(rank,action);
  }
  return rankCounts;
}

Action rankMove2Action(RankMove &move);
RankMove action2RankMove(Action &action);
std::vector<Action> rankMoves2Actions(std::vector<RankMove> &moves);

class OpenSpielLandlordGame : public Game {
 public:
  explicit OpenSpielLandlordGame(const GameParameters& params);
  int NumDistinctActions() const override;
  std::unique_ptr<State> NewInitialState() const override;

  int NumPlayers() const override;
  double MinUtility() const override;
  double MaxUtility() const override;
  std::shared_ptr<const Game> Clone() const override;
  std::vector<int> ObservationNormalizedVectorShape() const override;
  int MaxGameLength() const override;

  /* const landlord_learning_env::ObservationEncoder& Encoder() const {
    return encoder_;
  } */

  // const landlord_learning_env::LandlordGame& LandlordGame() const { return game_; }

  bool isAllowAddPair() const{
    return isAllowAddPair_;
  }

  int rand_seed() const {
    return rd_(); 
  }
 private:
    bool isAllowAddPair_;  // 是否允许带对牌
    bool isAllowContinuableBomb_; //是否允许连炸
    bool isThreeNoAddMustLastPut_; // 三不带牌是否必须最后一手出
    bool isUsedWildCard_; // 是否使用花牌（整副牌55张了）
    bool isUseLaizi_; //是否使用癞子牌，随机选择一种牌为癞子，2（王做癞子)或4张（其他牌）
    bool isUseTiandiLaizi_; //是否使用天地癞子（更多癞子，6或8个癞子）。

    //int rng_seed_;
   mutable  std::random_device rd_;
};

class OpenSpielLandlordState : public State {
 public:
  explicit OpenSpielLandlordState(std::shared_ptr<const Game> game);
  Player CurrentPlayer() const override;
  std::vector<Action> LegalActions() const override;
  std::vector<std::pair<Action, double>> ChanceOutcomes() const override;
  std::string ActionToString(Player player, Action action_id) const override;
  std::vector<double> Returns() const override;

  // We support observations only, not information states. The information
  // state would have to include the entire history of the game, and is
  // impractically large.
  // The observation by default includes knowledge inferred from past hints.
  std::string Observation(Player player) const override;
  void ObservationAsNormalizedVector(Player player,
                                     std::vector<double>* values) const;

  std::unique_ptr<State> Clone() const override;
  //ActionsAndProbs ChanceOutcomes() const override;
  std::string ToString() const override;
  bool IsTerminal() const override;

 protected:
  void DoApplyAction(Action action) override;

 private:
  const OpenSpielLandlordGame* game_;
  double prev_state_score_;

  // Fields sets to bad/invalid values. Use Game::NewInitialState().
  Player cur_player_;
  
  mutable std::mt19937 rng_;
  mutable Deck deck_;  //主要用户洗牌和发牌。
  mutable bool dealt_ = false;  //标识是否已经发牌结束。
  mutable bool bided_ = false;  //叫分成功否
  
  Action lastMaxBidedAction_ = kInvalidAction;
  Player lastMaxBidedPlayer_ = kInvalidPlayer;

  //发牌后手牌的原始牌，以及解析后的直方图（记录每个rank牌的数量，以及每个rank各自的牌）。
  std::array<std::vector<Poker>,landlord::NumPlayers> originHands_;
  mutable std::array<RankCountAndCardArray,landlord::NumPlayers> hands_;
  std::vector<Poker> landLordPokers_;  //发牌后留给地主的牌。

  int bombCounts_ =0; //已经打出的炸弹数量，用来计算最终输赢时的倍数。
  mutable int winPlayer_ = kInvalidPlayer;
  std::array<int,landlord::NumPlayers> playerValidActions_={0}; //玩家的有效操作数统计，便于计算春天或反春。
};

}  // namespace landlord
}  // namespace open_spiel

#endif  // THIRD_PARTY_OPEN_SPIEL_GAMES_LANDLORD_H_
