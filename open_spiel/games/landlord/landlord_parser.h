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
using RankCountsArray = std::array<RankType, RANK_COUNTS>;
using RankCardsArray = std::array<std::vector<LandlordCard>, RANK_COUNTS>;
using RankCountAndCardArray = std::pair<RankCountsArray,RankCardsArray>;
using RankMoveByType = std::array<std::vector<RankMove>, MOVE_COUNTS>;
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

RankMoveByType parseByType(RankCountsArray &rankCounts);
std::vector<RankMove> parse(RankCountsArray &rankCounts);
std::vector<RankMove> parse(RankCountsArray &rankCounts,RankMove otherMove);
std::string rankCountsArray2String(RankCountsArray &countsArray);
int getPokersCounts(RankCountsArray &countsArray);
RankCountsArray rankMove2Counts(RankMove &move);
RankMove buildMoveByPokersCounts(RankCountsArray &countsArray);
std::vector<RankMove> parseKickerByType(RankCountsArray &rankCounts, LandlordMoveType type);
} // namespace landlord_learning_env

#endif //__LANDLORD_PARSER_H__