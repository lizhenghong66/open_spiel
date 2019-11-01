//对手牌进行解析
//先不考虑癞子牌
#ifndef __LANDLORD_PARSER_H__
#define __LANDLORD_PARSER_H__
#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <set>
#include <utility>
#include <algorithm>
#include <iostream>

#include "landlord_card.h"
#include "landlord_move.h"
#include "util.h"
namespace landlord_learning_env
{
extern std::map<LandlordMoveType,std::string> MoveTypeStringMap;

class RankMove
{
public:
    RankMove(LandlordMoveType move_type, RankType startRank,
             RankType endRank = -1, std::vector<Poker> addedRanks = {}) : type_(move_type), startRank_(startRank), endRank_(endRank),
                                                                          addedRanks_(addedRanks){};
    bool operator==(const RankMove &other_move) const
    {
        return (type_ == other_move.Type() &&
                startRank_ == other_move.StartRank() &&
                endRank_ == other_move.EndRank() &&
                addedRanks_ == other_move.AddedRanks());
    };
    LandlordMoveType Type() const { return type_; }
    RankType StartRank() const { return startRank_; }
    RankType EndRank() const { return endRank_; }
    std::vector<RankType> AddedRanks() const { return addedRanks_; }
    bool ChangeType(LandlordMoveType newType, std::vector<RankType> addedRanks)
    {
        //[TODO]以后考虑增加合法性校验
        LandlordMoveType type_ = newType;
        addedRanks_ = addedRanks;
        return true;
    }

    std::string toString() const{
        std::string str = "";
        //str =  std::to_string(type_)  +  ":"  +  std::to_string(startRank_) +  
        str =  MoveTypeStringMap[type_] +"(" + std::to_string(type_)  +  "):"  +  std::to_string(startRank_) +  
            " --> " + std::to_string(endRank_)  + ","  +  pokers2String(addedRanks_);
        return str;
    }

private:
    LandlordMoveType type_;
    RankType startRank_;
    RankType endRank_;                 //非连牌时与startRank相同或忽略（没设置）
    std::vector<RankType> addedRanks_; //带牌时带牌的rank列表。
};
using RankCountsArray = std::array<RankType, RANK_COUNTS>;
using RankCardsArray = std::array<std::vector<LandlordCard>, RANK_COUNTS>;
using RankCountAndCardArray = std::pair<RankCountsArray,RankCardsArray>;

/**
 * 根据手牌数组解析为两个数组（1个每个等级牌的数量，以及每个等级的相关牌列表）
 */
RankCountAndCardArray buildRankCounts(std::vector<Poker> &pokers);
RankCountAndCardArray buildRankCounts(std::vector<LandlordCard> &cards);
/**
     * 根据手牌rank（等级）数量直方图（每个等级都占一项，没有则为0，否则记录该等级的牌数量），
     * 解析可能的连续牌（顺子，两连，三连）。
     * 参数：
     * rankCounts：牌等级数量数组
     * minCount：最少连续数量，如顺子最少5连张。
     * count：连续的每种牌数量
     */
std::vector<std::pair<int, int>> buildContinueRanks(
    std::array<RankType, RANK_COUNTS> &rankCounts, //牌等级数量数组
    int minCount,                                  //最少连续数量，如顺子最少5连张。
    int count                                      //每种牌数量。
);

void dispCardRanks(RankCardsArray &cardRanks);

std::vector<RankMove> parse(RankCountsArray rankCounts);
} // namespace landlord_learning_env

#endif //__LANDLORD_PARSER_H__