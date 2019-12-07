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

#include "open_spiel/games/landlord.h"

#include "open_spiel/game_parameters.h"
#include "open_spiel/spiel.h"
#include "open_spiel/spiel_utils.h"

namespace open_spiel
{
namespace landlord
{

namespace
{

const GameType kGameType{
    /*short_name=*/"landlord",
    /*long_name=*/"Landlord",
    GameType::Dynamics::kSequential,
    GameType::ChanceMode::kExplicitStochastic,
    GameType::Information::kImperfectInformation,
    GameType::Utility::kIdentical,
    GameType::RewardModel::kRewards,
    /*max_num_players=*/3,
    /*min_num_players=*/3,
    /*provides_information_state=*/false,
    /*provides_information_state_as_normalized_vector=*/false,
    /*provides_observation=*/true,
    /*provides_observation_as_normalized_vector=*/true,
    /*parameter_specification=*/
    {
        {"isAllowAddPair", GameParameter(true)},
        {"isAllowContinuableBomb", GameParameter(false)},
        {"isThreeNoAddMustLastPut", GameParameter(false)},
        {"isUsedWildCard", GameParameter(false)},
        {"isUseLaizi", GameParameter(false)},
        {"isUseTiandiLaizi", GameParameter(false)},
    }};

std::shared_ptr<const Game> Factory(const GameParameters &params)
{
  return std::shared_ptr<const Game>(new OpenSpielLandlordGame(params));
}

REGISTER_SPIEL_GAME(kGameType, Factory);

} // namespace

/* std::unordered_map<std::string, std::string> OpenSpielLandlordGame::MapParams()
    const {
  std::unordered_map<std::string, std::string> landlord_params;
  if (IsParameterSpecified(game_parameters_, "isAllowAddPair"))
    landlord_params["isAllowAddPair"] = absl::StrCat(
      ParameterValue<bool>("isAllowAddPair"));

  
  return landlord_params;
} */

OpenSpielLandlordGame::OpenSpielLandlordGame(const GameParameters &params)
    : Game(kGameType, params),
      isAllowAddPair_(ParameterValue<bool>("isAllowAddPair")),
      isAllowContinuableBomb_(ParameterValue<bool>("isAllowContinuableBomb")),
      isThreeNoAddMustLastPut_(ParameterValue<bool>("isThreeNoAddMustLastPut")),
      isUsedWildCard_(ParameterValue<bool>("isUsedWildCard")),
      isUseLaizi_(ParameterValue<bool>("isUseLaizi")),
      isUseTiandiLaizi_(ParameterValue<bool>("isUseTiandiLaizi"))
{
}

int OpenSpielLandlordGame::NumDistinctActions() const { return landlord::NumDistinctActions; }

std::unique_ptr<State> OpenSpielLandlordGame::NewInitialState() const
{
  return std::unique_ptr<State>(new OpenSpielLandlordState(shared_from_this()));
}

int OpenSpielLandlordGame::NumPlayers() const { return landlord::NumPlayers; }

double OpenSpielLandlordGame::MinUtility() const { return -2 * pow(2, 6); }

double OpenSpielLandlordGame::MaxUtility() const
{
  return 2 * pow(2, 6);
}

std::shared_ptr<const Game> OpenSpielLandlordGame::Clone() const
{
  return std::shared_ptr<Game>(new OpenSpielLandlordGame(GetParameters()));
}

std::vector<int> OpenSpielLandlordGame::ObservationNormalizedVectorShape() const
{
  return {ObservationNormalizedVectorLen};
}

int OpenSpielLandlordGame::MaxGameLength() const
{
  // This is an overestimate.
  return 100;
}

Player OpenSpielLandlordState::CurrentPlayer() const
{
  if (!dealt_)
    return kChancePlayerId;
  if (IsTerminal())
    return kTerminalPlayerId;
  //发牌也使用了一个action，必须排除
  if(!bided_){
    return (history_.size() - 1) % landlord::NumPlayers;
  }
  return (lastMaxBidedPlayer_ + (history_.size() - 4) % landlord::NumPlayers)%landlord::NumPlayers;
}

std::vector<Action> OpenSpielLandlordState::LegalActions() const
{
  if (IsTerminal())
  {
    return {};
  }
  else if (dealt_)
  {
    //已经发牌
    if (bided_)
    {
      //return {};
      //已经叫牌，开始游戏了
      //因为action编码原因，可以很容易区分主动出牌和被动出牌，
      Action lastAction1 = history_[history_.size()-1];
      Action lastAction2 = history_[history_.size()-2];
      // LandlordMoveType  last1 = decodeActionType(lastAction1);
      // LandlordMoveType  last2 = decodeActionType(lastAction2);
      LandlordMoveType  last1 = decodeActionType5(lastAction1);
      LandlordMoveType  last2 = decodeActionType5(lastAction2);
      
      std::vector<RankMove> moves;
      if (history_.size() <= 4 || last1 == LandlordMoveType::kPass && last2 == LandlordMoveType::kPass){
        //自己出牌后，其他两家没有出牌（地主第一次出牌因为叫牌action解析后的moveType也时kPass。
        //地主出牌时，因为前面发牌和叫牌，history里面已经有4个action。
        //为了避免编码变化的影响，使用历史命令长度来区分（这个需要根据叫牌策略可能有所改变）是否时地主首次出牌。
        moves = parse(hands_[CurrentPlayer()].first);
      }else{
        //RankMove otherMove = last1 == LandlordMoveType::kPass?action2RankMove(lastAction2):action2RankMove(lastAction1);
        RankMove otherMove = 
          (last1 == LandlordMoveType::kPass?action2Move5(lastAction2):action2Move5(lastAction1));
         moves = parse(hands_[CurrentPlayer()].first,otherMove);
      }
      std::vector<Action> actions = rankMoves2Actions(moves);
      return actions;
    }
    else
    {
      std::vector<Action> bidActions = {kPass};
      Action lastAction = lastMaxBidedAction_ < kPass ? kPass : lastMaxBidedAction_;

      for (Action action = lastAction + 1; action <= kThree; action++)
      {
        bidActions.push_back(action);
      }
      std::cout << "LegalActions  size when bid:"  << bidActions.size() << std::endl;
      return bidActions; //叫牌动作。
    }
  }
  else
  {
    return {0}; //没有发牌，特殊action 0，结合dealt_标识可以进行发牌。
  }
}

std::vector<std::pair<Action, double>> OpenSpielLandlordState::ChanceOutcomes()
    const
{
  return {{0, 1.0}};
}

std::string OpenSpielLandlordState::ActionToString(Player player,
                                                   Action action_id) const
{
  std::string strAction = "";
  if (!dealt_)
  {
    strAction = "Deal";
  }
  else if (!bided_)
  {
    strAction = std::to_string(player) + " call " + std::to_string(action_id);
  }
  else
  {
    RankMove move = action2Move5(action_id);
    strAction = move.toString();
  }

  return strAction;
}

std::vector<double> OpenSpielLandlordState::Returns() const
{
  std::vector<double> returns = {0,0,0};
  if (!IsTerminal()) return returns;
  int times = 0; //2**0 ,基础倍数。
  if (playerValidActions_[0] == 1){
    //地主只出了1手牌，反春
    times++;
  }else if(playerValidActions_[1] ==0 && playerValidActions_[1] ==0){
      //其他两家都没出牌，春天。
      times++;
  }
  times += bombCounts_;
  times = std::pow(2,times);

  double base = lastMaxBidedAction_;
  if (winPlayer_ == 0){
    returns = {2*base * times,-base * times,-base*times};
  }else{
    returns = {-2*base * times,base * times,base*times};
  }
  return returns;
}

void OpenSpielLandlordState::DoApplyAction(Action action)
{
  if (dealt_)
  {
    //发牌结束
    if (bided_)
    {
      //叫牌结束
      // RankMove move = action2RankMove(action);
      // if (move.Type() != LandlordMoveType::kPass && move.Type() != kInvalid){
      //   RankCountsArray countsArr = decodeRankCounts(action);
      //   Player cur_player = CurrentPlayer();
      //   std::cout << "player " << std::to_string(cur_player) << " put " << rankCountsArray2String(countsArr) <<  std::endl;
      //   for (RankType rank = 0; rank < RANK_COUNTS; rank++){
      //     if (countsArr[rank] > 0){
      //       hands_[cur_player].first[rank] -= countsArr[rank];
      //       for(int i = 0; i < countsArr[rank]; i++){
      //         LandlordCard card = hands_[cur_player].second[rank].back();
      //         //std::cout << "put:" << card2String(card) << ",poker:" << card2Poker(card) << std::endl;
      //         originHands_[cur_player].erase(std::remove(originHands_[cur_player].begin(),originHands_[cur_player].end(),card2Poker(card)));
      //         hands_[cur_player].second[rank].pop_back();
      //       }            
      //     }
      //   }

      RankMove move = action2Move5(action);
      Player cur_player = CurrentPlayer();
      if (move.Type() == LandlordMoveType::kThreeStraightAddOne &&
        (move.EndRank()-move.StartRank()) == 4){
          //5张三连带单
          std::vector<int> mayAddPokers;
          for (int i = 0; i <= kPokerJOKER_RANK; i++){
            if (i < move.StartRank() || i > move.EndRank())
            {
                for(int i =0; i<  hands_[cur_player].first[i];i++){
                    mayAddPokers.push_back(i);  //这样可以拆分其他牌
                }
            }else{
                if (hands_[cur_player].first[i] == 4){
                    mayAddPokers.push_back(i); //3顺中有炸弹，多余牌可以带。
                }
            }
          }
          if (mayAddPokers.size() == 5){
            move = RankMove(move.Type(),move.StartRank(),move.EndRank(),
                mayAddPokers);
          }else{
            move = RankMove(LandlordMoveType::kInvalid,0);
          }          
      }else if(move.Type() == LandlordMoveType::kThreeStraightAddPair &&
        (move.EndRank() - move.StartRank()) == 3){
          //4张三连带对
          std::vector<int> mayAddPokers;
          for (int i = 0; i <= kPokerA_RANK; i++){
            if (i < move.StartRank() || i > move.EndRank())
            {
                for(int i =0; i<  hands_[cur_player].first[i];i++){
                  if (hands_[cur_player].first[i] == 4){
                      mayAddPokers.push_back(i);
                      mayAddPokers.push_back(i);
                  }else if(hands_[cur_player].first[i] == 2){
                      mayAddPokers.push_back(i);
                  }
                }
            }
          }
          if (mayAddPokers.size() == 4){
            move = RankMove(move.Type(),move.StartRank(),move.EndRank(),
                mayAddPokers);
          }else{
            move = RankMove(LandlordMoveType::kInvalid,0);
          }  
      }

      if (move.Type() != LandlordMoveType::kPass && move.Type() != kInvalid){
        RankCountsArray countsArray = rankMove2Counts(move);
        playerValidActions_[cur_player]++;   //当前玩家的有效操作增加。
        if (move.Type() == kBomb || move.Type() == kKingBomb){
          bombCounts_++;   //炸弹数量增加1个。
        }
        std::cout << "player " << std::to_string(cur_player) << " put " << rankCountsArray2String(countsArray) <<  std::endl;
        for (RankType rank = 0; rank < RANK_COUNTS; rank++){
          if (countsArray[rank] > 0){
            hands_[cur_player].first[rank] -= countsArray[rank];
            for(int i = 0; i < countsArray[rank]; i++){
              LandlordCard card = hands_[cur_player].second[rank].back();
              std::cout << "put:" << card2String(card) << ",poker:" << card2Poker(card) << std::endl;
              originHands_[cur_player].erase(std::remove(originHands_[cur_player].begin(),originHands_[cur_player].end(),card2Poker(card)));
              hands_[cur_player].second[rank].pop_back();
            }            
          }
        }

        std::cout << " after put,origin hand len = " << std::to_string(originHands_[cur_player].size()) <<  std::endl;
        std::cout << " after put,count histogram = " << rankCountsArray2String(hands_[cur_player].first) <<  std::endl;

      }
    }
    else
    {
      //开始叫牌
      if (action > lastMaxBidedAction_)
      {
        lastMaxBidedAction_ = action;
        lastMaxBidedPlayer_ = CurrentPlayer();
      }
      if (history_.size() >= landlord::NumPlayers && lastMaxBidedAction_ > kPass)
      {
        bided_ = true;
        //叫牌结束，
        //将地主牌发出
        originHands_[lastMaxBidedPlayer_].insert(originHands_[lastMaxBidedPlayer_].end(),
                                                 landLordPokers_.begin(), landLordPokers_.end());
        //地主默认都是0，便于后面机器学习时统一处理。因此需要进行手牌交换
        // if (lastMaxBidedPlayer_ == 1)
        // {
        //   std::swap(originHands_[lastMaxBidedPlayer_], originHands_[0]);
        //   std::swap(originHands_[lastMaxBidedPlayer_], originHands_[2]);
        // }
        // else if (lastMaxBidedPlayer_ == 2)
        // {
        //   std::swap(originHands_[lastMaxBidedPlayer_], originHands_[0]);
        //   std::swap(originHands_[lastMaxBidedPlayer_], originHands_[1]);
        // }

        //此时再解析手牌直方图
        for (Player player = 0; player < num_players_; player++)
        {
          hands_[player] = buildRankCounts(originHands_[player]);
        }
        //hands_[lastMaxBidedPlayer_] = buildRankCounts(originHands_[lastMaxBidedPlayer_]);
      }
    }
  }
  else
  {
    //进行发牌
    deck_.Shuffle(&rng_);
    for (Player player = 0; player < num_players_; player++)
    {
      originHands_[player] = deck_.Deal(player * 17, (player + 1) * 17);
    }
    landLordPokers_ = deck_.Deal(51, deck_.NumCards());
    dealt_ = true;

    //此时再解析手牌直方图
    for (Player player = 0; player < num_players_; player++)
    {
      hands_[player] = buildRankCounts(originHands_[player]);
    }
  }
}

std::string OpenSpielLandlordState::Observation(Player player) const
{
  return "";
}

void OpenSpielLandlordState::ObservationAsNormalizedVector(
    Player player, std::vector<double> *values) const
{
  values->resize(game_->ObservationNormalizedVectorSize());
  std::fill(values->begin(), values->end(), 0);
  
  int offset = 0;
  // Mark who I am.
  (*values)[player] = 1;
  offset += num_players_;
  
  //记录当前状态(发牌/叫分/出牌，三阶段)
  if (!dealt_){
    (*values)[offset] = 1;
  }else if (!bided_){
    (*values)[offset+1] = 1;
  }else{
    (*values)[offset+2] = 1;
  }
  offset += 3;

  //记录最大叫分和方位
  if(lastMaxBidedPlayer_ >= 0){
     (*values)[offset+lastMaxBidedPlayer_] = 1;
  }
  offset += 3;
  if (((int)lastMaxBidedAction_) > 0){
    (*values)[offset+((int)lastMaxBidedAction_)-1] = 1;
  }
  offset += 3;
  //先前是考虑固定0方位为地主，但是叫牌后，地主可能不是0号玩家，需要进行换牌，反而麻烦。
  //palyer== lastMaxBidedPlayer_ 是地主，
  //palyer== （lastMaxBidedPlayer_ +1）% 3 是下家
  //palyer== （lastMaxBidedPlayer_ +2）% 3 是顶家
  //只是修改计算当前玩家时做一下修改。
  
  //编码手牌直方图（ranks数量，每个rank 4bit，15个ranks）
  // We note several features use a thermometer representation instead of one-hot.
// For example, life tokens could be: 0000 (0), 1000 (1), 1100 (2), 1110 (3),1111 (4).
// Returns the number of entries written to the encoding.
  for(int i = 0; i < landlord::NumPlayers; i++){
    for(int j = 0; j < 15; j++){
      for (int k = 0; k < hands_[i].first[j]; k++){
          (*values)[offset+ i * 60 + j*4 + k]  = 1;
      }      
    }
  }
  offset += 3 * 60;
  //编码最近3个出牌action，put action 接近10万，使用17bit整数编码。
  //编码最近3个出牌action，put action 约34238，使用16bit整数编码。
  int actionBitLen = 16;
  for(int i = history_.size()-1; i > 4 && history_.size() - i <= 3; i--){
    Action lastAction = history_[i];
    for (int j = 0; j < actionBitLen; j++){
      int flag = 1 << j;
      flag = (lastAction & flag) >> j;
      (*values)[offset+ (history_.size() - i - 1)*actionBitLen + j]  = flag;
    }
  }
  offset += 3 * actionBitLen;
  //每个玩家已出牌，因为看了所有玩家牌，这个出牌可以先忽略。
}

std::unique_ptr<State> OpenSpielLandlordState::Clone() const
{
  return std::unique_ptr<State>(new OpenSpielLandlordState(*this));
}

std::string OpenSpielLandlordState::ToString() const
{
  std::string strState = "dealt_:" +
                         std::to_string(dealt_) + ",bided:" +
                         std::to_string(bided_) + ",history_.size:" +
                         std::to_string(history_.size()) + ",lastMaxBidedAction_:" +
                         std::to_string(lastMaxBidedAction_) + ",lastMaxBidedPlayer_:" +
                         std::to_string(lastMaxBidedPlayer_) + ",winPlayer:" +
                         std::to_string(winPlayer_) + ",bombCounts:" +
                         std::to_string(bombCounts_) + ",playerValidActions:[" +
                         std::to_string(playerValidActions_[0]) + " " 
                         + std::to_string(playerValidActions_[1]) + " " 
                         + std::to_string(playerValidActions_[2]) + "]\n";
  for (Player player = 0; player < landlord::NumPlayers; player++)
  {
    strState += "player " + std::to_string(player) + ":" +
                cards2String(pokers2Cards(originHands_[player])) + "\n";
    strState +=
        rankCountsArray2String(hands_[player].first) + "\n";
  }
  return strState;
}

bool OpenSpielLandlordState::IsTerminal() const
{
  bool ret = false;
  if (dealt_){
    if (!bided_ && history_.size() >= 4 )         //已经叫牌一圈都没人叫分，重新开始游戏)
    {
      ret = true;
    }else if(bided_ && lastMaxBidedAction_ > kPass) //有人叫牌'
    {
      if (winPlayer_ != kInvalidPlayer){
        ret = true;
      }else{
        for(Player player = 0; player < landlord::NumPlayers; player++){
          if (getPokersCounts(hands_[player].first) == 0){//有人牌打完了。
            ret = true;
            winPlayer_ = player;
            break;
          }
        }
      }
    }
  }
  return ret;
}

OpenSpielLandlordState::OpenSpielLandlordState(std::shared_ptr<const Game> game)
    : State(game),
      game_(static_cast<const OpenSpielLandlordGame *>(game.get())),
      prev_state_score_(0.),
      rng_(game_->rand_seed())
{
}

Action rankMove2Action(RankMove &move)
{
  Action action = encodeActionType(move.Type());
  RankCountsArray countsArray = rankMove2Counts(move);
  action |= encodeRankCounts(countsArray);
  return action;
}

RankMove action2RankMove(Action &action)
{
  LandlordMoveType type = decodeActionType(action);
  if (type == LandlordMoveType::kPass){
    return RankMove(LandlordMoveType::kPass, -1);
  }
  RankCountsArray countsArray = decodeRankCounts(action);
  RankMove move = buildMoveByPokersCounts(countsArray);
  if (move.Type() == type){
    return move;
  }
  std::cout << "action:" << moveType2String(type) << "," << rankCountsArray2String(countsArray) 
     << "parsed move:" << move.toString() << std::endl;

  return RankMove(kInvalid, -1); //非法acton
}

std::vector<Action> rankMoves2Actions(std::vector<RankMove> &moves){
  std::set<Action> actions;
  //std::cout << "rankMoves2Actions:" << std::endl;
  for (auto move : moves){
    //Action action = rankMove2Action(move);
    Action action = move2Action5(move);
    // RankMove tmp = action2RankMove(action);
    // std::cout << move.toString() << std::endl;
    // RankCountsArray counts = decodeRankCounts(action);
    // std::cout <<  tmp.toString() << "," << action << "," << rankCountsArray2String(counts)<< std::endl;
    // if (!(tmp == move)){
    //   action = rankMove2Action(move);
    // }
    // assert(tmp == move);

    actions.insert(action);
  }

  std::vector<Action> results;
  for (auto action: actions){
    results.push_back(action);
  }
  std::sort(results.begin(),results.end());
  return results;
}

} // namespace landlord
} // namespace open_spiel
