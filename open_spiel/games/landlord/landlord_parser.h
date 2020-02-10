/*
 * @Author: your name
 * @Date: 2019-10-29 15:55:34
 * @LastEditTime : 2020-02-10 22:01:29
 * @LastEditors  : Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /open_spiel/open_spiel/games/landlord/landlord_parser.h
 */
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
    const std::array<RankType, RANK_COUNTS> &rankCounts, //牌等级数量数组
    int minCount,                                  //最少连续数量，如顺子最少5连张。
    int count                                      //每种牌数量。
);

void dispCardRanks(RankCardsArray &cardRanks);

RankMoveByType parseByType(RankCountsArray &rankCounts);
std::vector<RankMove> parse(RankCountsArray &rankCounts);
std::vector<RankMove> parse(RankCountsArray &rankCounts,RankMove otherMove);
std::string rankCountsArray2String(const RankCountsArray &countsArray);
int getPokersCounts(RankCountsArray &countsArray);
RankCountsArray rankMove2Counts(RankMove &move);
RankMove buildMoveByPokersCounts(RankCountsArray &countsArray);
std::vector<RankMove> parseKickerByType(RankCountsArray &rankCounts, LandlordMoveType type);

/**+++2020/02/07
 * 基于规则实现AI，也可以作为深度学习AI的对手。
 * 先前只是实现了基本的解析，只是解析了当前手牌的所有可能的出牌（
 *   并没有考虑拆牌的合理性，原想通过深度学习解决，但是还是出现了一些困难）。
 * 下一步需要对手牌解析进行更详细的分组：
 *  1. 首先提取炸弹（避免炸弹拆开），再解析3连，2连，单连
*/
/**
 * 根据手牌数量直方图解析所有出牌的组合及每种组合的最快出牌手数
 * 
 */
std::vector<RankMoveByType> parseHandPokers(const RankCountsArray &rankCounts);
/**
 * 只解析主牌，不处理带牌，并且单/对/三牌不共享情况下，基础牌型的可能组合。
 */
// void  parseByTypeBase(RankCountsArray &rankCounts,
//     RankMoveByType& allLegalActions);
void parseByTypeBase(const RankCountsArray &rankCounts,
                     const RankMoveByType &preOneHandCombs,
                      std::vector<RankMoveByType>& allHandCombs);

std::string rankCountsArray2DispString(const RankCountsArray &countsArray);
std::string handCombToDispString(const RankMoveByType handComb);
std::string allHandCombsToDispString(const std::vector<RankMoveByType> allHandCombs);
} // namespace landlord_learning_env

#endif //__LANDLORD_PARSER_H__