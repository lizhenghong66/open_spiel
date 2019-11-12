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

namespace open_spiel {
namespace landlord {

namespace {

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

std::shared_ptr<const Game> Factory(const GameParameters& params) {
  return std::shared_ptr<const Game>(new OpenSpielLandlordGame(params));
}

REGISTER_SPIEL_GAME(kGameType, Factory);

}  // namespace

/* std::unordered_map<std::string, std::string> OpenSpielLandlordGame::MapParams()
    const {
  std::unordered_map<std::string, std::string> landlord_params;
  if (IsParameterSpecified(game_parameters_, "isAllowAddPair"))
    landlord_params["isAllowAddPair"] = absl::StrCat(
      ParameterValue<bool>("isAllowAddPair"));

  
  return landlord_params;
} */

OpenSpielLandlordGame::OpenSpielLandlordGame(const GameParameters& params)
    : Game(kGameType, params),
    isAllowAddPair_(ParameterValue<bool>("isAllowAddPair")),
    isAllowContinuableBomb_(ParameterValue<bool>("isAllowContinuableBomb")),
    isThreeNoAddMustLastPut_(ParameterValue<bool>("isThreeNoAddMustLastPut")),
    isUsedWildCard_(ParameterValue<bool>("isUsedWildCard")),
    isUseLaizi_(ParameterValue<bool>("isUseLaizi")),
    isUseTiandiLaizi_(ParameterValue<bool>("isUseTiandiLaizi"))
     {}

int OpenSpielLandlordGame::NumDistinctActions() const { return landlord::NumDistinctActions; }

std::unique_ptr<State> OpenSpielLandlordGame::NewInitialState() const {
  return std::unique_ptr<State>(new OpenSpielLandlordState(shared_from_this()));
}

int OpenSpielLandlordGame::NumPlayers() const { return landlord::NumPlayers; }

double OpenSpielLandlordGame::MinUtility() const { return -2* pow(2,6); }

double OpenSpielLandlordGame::MaxUtility() const {
  return 2* pow(2,6);
}

std::shared_ptr<const Game> OpenSpielLandlordGame::Clone() const {
  return std::shared_ptr<Game>(new OpenSpielLandlordGame(GetParameters()));
}

std::vector<int> OpenSpielLandlordGame::ObservationNormalizedVectorShape() const {
  return {1000};
}

int OpenSpielLandlordGame::MaxGameLength() const {
  // This is an overestimate.
  return 100;  
}

Player OpenSpielLandlordState::CurrentPlayer() const {
  if (!dealt_) return kChancePlayerId;
  if (IsTerminal()) return kTerminalPlayerId;
  //发牌也使用了一个action，必须排除
  return (history_.size() - 1) % landlord::NumPlayers;
}

std::vector<Action> OpenSpielLandlordState::LegalActions() const {
  if (IsTerminal()) {
    return {};
  } else if (dealt_) {
    //已经发牌
    if (bided_){
      //已经叫牌，开始游戏了
      return {};
    }else{
      std::vector<Action>  bidActions = {kPass};
      Action lastAction = lastMaxBidedAction_ < kPass? kPass:lastMaxBidedAction_;

      for (Action action = lastAction +1; action <= kThree; action++){
          bidActions.push_back(action);
      }
      return bidActions; //叫牌动作。
    }
  } else {
    return {0}; //没有发牌，特殊action 0，结合dealt_标识可以进行发牌。
  }
}

std::vector<std::pair<Action, double>> OpenSpielLandlordState::ChanceOutcomes()
    const {
  return {{0, 1.0}};
}

std::string OpenSpielLandlordState::ActionToString(Player player,
                                                 Action action_id) const {
  std::string strAction="";
  if (!dealt_){
    strAction="Deal";
  }else if (!bided_){
    strAction=std::to_string(player) + " call " + std::to_string(action_id);
  }else{

  }
  
 return strAction;
}

std::vector<double> OpenSpielLandlordState::Rewards() const {
  return std::vector<double>(NumPlayers(), 0);
}

std::vector<double> OpenSpielLandlordState::Returns() const {
  return std::vector<double>(NumPlayers(),0);
}

void OpenSpielLandlordState::DoApplyAction(Action action) {
  if(dealt_){
    //发牌结束
    if (bided_){
      //叫牌结束
    }else{
      //开始叫牌
      if (action > lastMaxBidedAction_){
        lastMaxBidedAction_ = action;
        lastMaxBidedPlayer_ = CurrentPlayer();
      }
      if (history_.size() >= landlord::NumPlayers && lastMaxBidedAction_ > kPass){
        bided_ = true;
        //叫牌结束，
        //将地主牌发出
        originHands_[lastMaxBidedPlayer_].insert(originHands_[lastMaxBidedPlayer_].end(),
          landLordPokers_.begin(),landLordPokers_.end());
        //地主默认都是0，便于后面机器学习时统一处理。因此需要进行手牌交换
        if (lastMaxBidedPlayer_ == 1){          
          std::swap(originHands_[lastMaxBidedPlayer_],originHands_[0]);
          std::swap(originHands_[lastMaxBidedPlayer_],originHands_[2]);
        }else if (lastMaxBidedPlayer_ == 2){
          std::swap(originHands_[lastMaxBidedPlayer_],originHands_[0]);
          std::swap(originHands_[lastMaxBidedPlayer_],originHands_[1]);
        }

        //此时再解析手牌直方图
        for (Player player = 0; player < num_players_;player++){
          hands_[player] = buildRankCounts(originHands_[player]);
        }
      }
    }
  }else{
    //进行发牌
    deck_.Shuffle(&rng_);
    for (Player player = 0; player < num_players_;player++){
      originHands_[player] = deck_.Deal(player *17,(player+1)*17);
    }
    landLordPokers_ = deck_.Deal(51,deck_.NumCards());
    dealt_ = true;
  }
}

std::string OpenSpielLandlordState::Observation(Player player) const {
  return "";
}

void OpenSpielLandlordState::ObservationAsNormalizedVector(
    Player player, std::vector<double>* values) const {
      values->resize(game_->ObservationNormalizedVectorSize());
      std::fill(values->begin(),values->end(),0);
}

std::unique_ptr<State> OpenSpielLandlordState::Clone() const {
  return std::unique_ptr<State>(new OpenSpielLandlordState(*this));
}

std::string OpenSpielLandlordState::ToString() const {
  std::string strState = "dealt_:" + 
    std::to_string(dealt_)  + ",bided:" + 
    std::to_string(bided_) + ",history_.size:" +
    std::to_string(history_.size()) + ",lastMaxBidedAction_:" +
    std::to_string(lastMaxBidedAction_)+ ",lastMaxBidedPlayer_:" +
    std::to_string(lastMaxBidedPlayer_) + "\n";
    for (Player player = 0; player < landlord::NumPlayers;player++){
      strState +="player " + std::to_string(player) + ":" +  
        cards2String(pokers2Cards(originHands_[player])) +"\n";
      strState +=  
        rankCountsArray2String(hands_[player].first) +"\n";
  }
  return strState; 
}

bool OpenSpielLandlordState::IsTerminal() const { 
  return dealt_ && (
    !bided_ && history_.size() >= 4  //已经叫牌一圈都没人叫分，重新开始游戏
    || bided_ && lastMaxBidedAction_ > kPass  //有人叫牌
    );
 }

OpenSpielLandlordState::OpenSpielLandlordState(std::shared_ptr<const Game> game)
    : State(game),
      game_(static_cast<const OpenSpielLandlordGame*>(game.get())),
      prev_state_score_(0.),
      rng_(game_->rand_seed()){
      }

}  // namespace landlord
}  // namespace open_spiel
