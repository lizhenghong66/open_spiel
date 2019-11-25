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
  return {1000};
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
  return (history_.size() - 1) % landlord::NumPlayers;
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
      //已经叫牌，开始游戏了
      //因为action编码原因，可以很容易区分主动出牌和被动出牌，
      Action lastAction1 = history_[history_.size()-1];
      Action lastAction2 = history_[history_.size()-2];
      // LandlordMoveType  last1 = decodeActionType(lastAction1);
      // LandlordMoveType  last2 = decodeActionType(lastAction2);
      LandlordMoveType  last1 = decodeActionType4(lastAction1);
      LandlordMoveType  last2 = decodeActionType4(lastAction2);
      
      std::vector<RankMove> moves;
      if (last1 == LandlordMoveType::kPass && last2 == LandlordMoveType::kPass){
        //自己出牌后，其他两家没有出牌（地主第一次出牌因为叫牌action解析后的moveType也时kPass。
        moves = parse(hands_[CurrentPlayer()].first);
      }else{
        //RankMove otherMove = last1 == LandlordMoveType::kPass?action2RankMove(lastAction2):action2RankMove(lastAction1);
        RankMove otherMove = 
          (last1 == LandlordMoveType::kPass?action2Move4(lastAction2):action2Move4(lastAction1));
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
    RankMove move = action2Move4(action_id);
    strAction = move.toString();
  }

  return strAction;
}

std::vector<double> OpenSpielLandlordState::Rewards() const
{
  return std::vector<double>(NumPlayers(), 0);
}

std::vector<double> OpenSpielLandlordState::Returns() const
{
  return std::vector<double>(NumPlayers(), 0);
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

      RankMove move = action2Move4(action);
      if (move.Type() != LandlordMoveType::kPass && move.Type() != kInvalid){
        RankCountsArray countsArray = rankMove2Counts(move);
        Player cur_player = CurrentPlayer();
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
        if (lastMaxBidedPlayer_ == 1)
        {
          std::swap(originHands_[lastMaxBidedPlayer_], originHands_[0]);
          std::swap(originHands_[lastMaxBidedPlayer_], originHands_[2]);
        }
        else if (lastMaxBidedPlayer_ == 2)
        {
          std::swap(originHands_[lastMaxBidedPlayer_], originHands_[0]);
          std::swap(originHands_[lastMaxBidedPlayer_], originHands_[1]);
        }

        //此时再解析手牌直方图
        for (Player player = 0; player < num_players_; player++)
        {
          hands_[player] = buildRankCounts(originHands_[player]);
        }
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
                         std::to_string(lastMaxBidedPlayer_) + "\n";
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
        for(Player player = 0; player < landlord::NumPlayers; player++){
          if (getPokersCounts(hands_[player].first) == 0){//有人牌打完了。
            ret = true;
            break;
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
  std::cout << "rankMoves2Actions:" << std::endl;
  for (auto move : moves){
    //Action action = rankMove2Action(move);
    Action action = move2Action4(move);
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
