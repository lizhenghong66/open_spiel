#include "landlord_parser.h"

namespace landlord_learning_env
{
/**
     *解析出连牌后，看看是否还可以拆分成更小的连牌。 
     参数：
       continueRanks：保存连牌的起止rank（等级）的列表。
       startRank：找到的连牌的起始rank。
       endRank：找到的连牌的结束rank。
       minCount：该类连牌的最小连牌数。
     */
void addSubContinueRanks(std::vector<std::pair<int, int>> &continueRanks,
                         int startRank, int endRank, int minCount)
{
    while (endRank - startRank >= minCount)
    {
        continueRanks.push_back(std::make_pair(startRank, endRank - 1));
        endRank--;
    }
}

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
    int minCount,                                        //最少连续数量，如顺子最少5连张。
    int count                                            //每种牌数量。
)
{
    //查找连续的牌
    std::vector<std::pair<int, int>> continueRanks;
    for (int i = 0; i <= kPokerA_RANK - minCount + 1; i++)
    {
        if (rankCounts[i] >= count)
        { //连续牌的下值
            for (int j = i + 1; j <= kPokerA_RANK; j++)
            {
                if (rankCounts[j] < count)
                { //至少两个以上的连牌
                    if (j - i >= minCount)
                    {
                        continueRanks.push_back(std::make_pair(i, j - 1));
                        addSubContinueRanks(continueRanks, i, j - 1, minCount);
                    }
                    break;
                }
                else if (j == kPokerA_RANK && rankCounts[j] >= count && j - i + 1 >= minCount)
                {
                    continueRanks.push_back(std::make_pair(i, j));
                    addSubContinueRanks(continueRanks, i, j, minCount);
                }
            }
        }
    }
    return continueRanks;
}

RankCountAndCardArray buildRankCounts(std::vector<Poker> &pokers)
{
    std::vector<LandlordCard> cards = pokers2Cards(pokers);
    return buildRankCounts(cards);
}

RankCountAndCardArray buildRankCounts(std::vector<LandlordCard> &cards)
{
    std::array<RankType, RANK_COUNTS> rankCounts = {};
    std::array<std::vector<LandlordCard>, RANK_COUNTS> cardRanks = {{}};
    for (LandlordCard card : cards)
    {
        rankCounts[card.second]++;
        cardRanks[card.second].push_back(card);
    }

    return std::make_pair(rankCounts, cardRanks);
}

/**
     * 从手牌等级数量直方图中删除一个3张以上出牌操作。
     * 然后继续后续解析。
     */
inline std::array<RankType, RANK_COUNTS> updatedRankCounts(
    std::array<RankType, RANK_COUNTS> rankCounts,
    RankMove move)
{
    int count = 0; //连牌的每种牌的数量
    switch (move.Type())
    {
    case kThree:
        rankCounts[move.StartRank()] -= 3;
        break;
    case kBomb:
        rankCounts[move.StartRank()] -= 4;
        break;
    case kStraight:
        count = 1;
        break;
    case kTwoStraight:
        count = 2;
        break;
    case kThreeStraight:
        count = 3;
        break;
    }
    if (count > 0)
    {
        for (int i = move.StartRank(); i <= move.EndRank(); i++)
        {
            rankCounts[move.StartRank()] -= count;
        }
    }
    return rankCounts;
}

void deletePokerFromPokers(std::vector<int> &pokers, int poker)
{
    auto loc = find(pokers.begin(), pokers.end(), poker);
    if (loc != pokers.end())
    {
        pokers.erase(loc);
    }
}

void updatedAddPokers(LandlordMoveType type, int start, int end, int len,
                      std::vector<int> &mayAddPokers,
                      std::array<std::vector<RankMove>, MOVE_COUNTS> &allLegalActions)
{
    if (mayAddPokers.size() >= len)
    {
        //所有可能的组合带牌都计算了（[TODO]可能很多不合理，增加游戏逻辑可以排除一些不合理的带牌）
        std::vector<std::vector<int>> cmbs = combine(mayAddPokers.size(), len);
        std::set<std::vector<int>> addPokerSets;
        for (auto cmb : cmbs)
        {
            std::vector<int> addPokers;
            for (auto mayAdd : cmb)
            {
                addPokers.push_back(mayAddPokers[mayAdd]);
            }
            addPokerSets.insert(addPokers);
        }

        for (auto addPokers : addPokerSets)
        {
            allLegalActions[type].push_back(
                RankMove(type, start, end, addPokers));
        }
    }
}

int countMayAddCounts(int mainBaseCount, RankType startRank, int len, RankCountsArray &rankCounts, bool isSingle)
{
    int count = 0;
    int base = isSingle ? 1 : 2;

    for (int i = 0; i <= kPokerJOKER_RANK; i++)
    {
        if (i < startRank || i > startRank + len - 1)
        {
            count += rankCounts[i] / base;
        }
        else if (rankCounts[i] - mainBaseCount > 0)
        {
            count += (rankCounts[i] - mainBaseCount) / 2;
        }
    }
    return count;
}

bool chkAddedPokers(LandlordMoveType mainType, RankType startRank, int len, RankCountsArray &rankCounts)
{
    int count = 0;
    switch (mainType)
    {
    case kThreeAddSingle /* 3带单 */:
        /* code */
        count = countMayAddCounts(3, startRank, len, rankCounts, true);
        break;
    case kThreeAddPair /* 3带对 */:
        count = countMayAddCounts(3, startRank, len, rankCounts, false);
        break;
    case kFourAddTwoSingles /* 4带单 */:
        count = countMayAddCounts(4, startRank, len, rankCounts, true);
        break;
    case kFourAddTwoPairs /* 4带对 */:
        count = countMayAddCounts(4, startRank, len, rankCounts, false);
        break;
    case kThreeStraightAddOne /* 3顺带单 */:
        count = countMayAddCounts(3, startRank, len, rankCounts, true);
        break;
    case kThreeStraightAddPair /* 3顺带对 */:
        count = countMayAddCounts(3, startRank, len, rankCounts, false);
        break;
    default:
        break;
    }
    //std::cout << "===chkAddedPokers===," << moveType2String(mainType) << "(" << std::to_string(mainType) << ") can added counts:" << count << std::endl;
    return (count >= len ? true : false);
}

RankMoveByType parseByType(RankCountsArray &rankCounts)
{
    RankMoveByType allLegalActions;
    std::vector<int> singles, pairs, threes, bombs, kingbomb;

    for (int i = 0; i <= kPokerJOKER_RANK; i++)
    {
        if (rankCounts[i] > 0)
        {
            //增加一个单张move
            singles.push_back(i);
            allLegalActions[kSingle].push_back(RankMove(kSingle, i));
            //特殊处理王炸
            if (i == kPokerJOKER_RANK && rankCounts[i - 1] > 0)
            {
                //有王炸
                kingbomb.push_back(i);
                allLegalActions[kKingBomb].push_back(RankMove(kKingBomb, i - 1, i));
            }
        }
        if (rankCounts[i] > 1)
        {
            //增加一个对子move
            pairs.push_back(i);
            allLegalActions[kPair].push_back(RankMove(kPair, i));
        }
        if (rankCounts[i] > 2)
        {
            //增加一个三张move
            threes.push_back(i);
            allLegalActions[kThree].push_back(RankMove(kThree, i));
        }
        if (rankCounts[i] > 3)
        {
            //增加一个炸弹move
            bombs.push_back(i);
            allLegalActions[kBomb].push_back(RankMove(kBomb, i));
        }
    }

    //查找连续顺子牌
    std::vector<std::pair<int, int>> continueRanks1 = buildContinueRanks(rankCounts, 5, 1);
    for (auto continue1 : continueRanks1)
    {
        allLegalActions[kStraight].push_back(RankMove(kStraight, continue1.first, continue1.second));
    }
    //查找连续对子牌
    std::vector<std::pair<int, int>> continueRanks2 = buildContinueRanks(rankCounts, 3, 2);
    for (auto continue2 : continueRanks2)
    {
        allLegalActions[kTwoStraight].push_back(RankMove(kTwoStraight, continue2.first, continue2.second));
    }
    //查找连续三张牌
    std::vector<std::pair<int, int>> continueRanks3 = buildContinueRanks(rankCounts, 2, 3);
    for (auto continue3 : continueRanks3)
    {
        allLegalActions[kThreeStraight].push_back(RankMove(kThreeStraight, continue3.first, continue3.second));

        //3顺带单
        if (chkAddedPokers(kThreeStraightAddOne, continue3.first, continue3.second - continue3.first + 1, rankCounts))
        {
            allLegalActions[kThreeStraightAddOne].push_back(RankMove(kThreeStraightAddOne, continue3.first, continue3.second));
        }
        //3顺带对
        if (chkAddedPokers(kThreeStraightAddPair, continue3.first, continue3.second - continue3.first + 1, rankCounts))
        {
            allLegalActions[kThreeStraightAddPair].push_back(RankMove(kThreeStraightAddPair, continue3.first, continue3.second));
        }
    }
    //然后处理带牌情况，3带，4带，
    //处理3带1
    for (auto three : threes)
    {
        //处理3带单
        if (chkAddedPokers(kThreeAddSingle, three, 1, rankCounts))
        {
            allLegalActions[kThreeAddSingle].push_back(RankMove(kThreeAddSingle, three, three));
        }

        //处理3带对
        if (chkAddedPokers(kThreeAddPair, three, 1, rankCounts))
        {
            allLegalActions[kThreeAddPair].push_back(RankMove(kThreeAddPair, three, three));
        }
    }
    //处理4带2
    for (auto bomb : bombs)
    {
        //处理4带单
        if (chkAddedPokers(kFourAddTwoSingles, bomb, 2, rankCounts))
        {
            allLegalActions[kFourAddTwoSingles].push_back(RankMove(kFourAddTwoSingles, bomb, bomb));
        }
        //处理4带对
        if (chkAddedPokers(kFourAddTwoPairs, bomb, 2, rankCounts))
        {
            allLegalActions[kFourAddTwoPairs].push_back(RankMove(kFourAddTwoPairs, bomb, bomb));
        }
    }
    return allLegalActions;
}

std::vector<RankMove> parse(RankCountsArray &rankCounts)
{
    RankMoveByType allLegalActions = parseByType(rankCounts);
    std::vector<RankMove> allActions;
    for (int i = 0; i < allLegalActions.size(); i++)
    {
        auto actions = allLegalActions[i];
        if (actions.size() > 0)
        {
            allActions.insert(allActions.end(), actions.begin(), actions.end());
        }
    }

    return allActions;
}

std::vector<RankMove> parse(RankCountsArray &rankCounts, RankMove otherMove)
{
    RankMoveByType allLegalActions = parseByType(rankCounts);
    std::vector<RankMove> allAMoves = allLegalActions[otherMove.Type()];
    std::vector<RankMove> greaterMoves;
    greaterMoves.push_back(RankMove(LandlordMoveType::kPass, -1));
    //相同类型出牌中找大于的牌
    for (auto move : allAMoves)
    {
        switch (otherMove.Type())
        {
        case kStraight:
        case kTwoStraight:
        case kThreeStraight:
        case kThreeStraightAddOne:
        case kThreeStraightAddPair:
            if (move.EndRank() - move.StartRank() == otherMove.EndRank() - otherMove.StartRank())
            {
                //连牌长度必须一致
                if (move.StartRank() > otherMove.StartRank())
                {
                    greaterMoves.push_back(move);
                }
            }

            break;

        default:
            if (move.StartRank() > otherMove.StartRank())
            {
                greaterMoves.push_back(move);
            }
            break;
        }
    }

    //找炸弹
    if (otherMove.Type() != kBomb && otherMove.Type() != kKingBomb)
    {
        if (allLegalActions[kBomb].size() > 0)
        {
            greaterMoves.insert(greaterMoves.end(), allLegalActions[kBomb].begin(), allLegalActions[kBomb].end());
        }
        if (allLegalActions[kKingBomb].size() > 0)
        {
            greaterMoves.insert(greaterMoves.end(), allLegalActions[kKingBomb].begin(), allLegalActions[kKingBomb].end());
        }
    }
    else if (otherMove.Type() == kBomb)
    {
        if (allLegalActions[kKingBomb].size() > 0)
        {
            greaterMoves.insert(greaterMoves.end(), allLegalActions[kKingBomb].begin(), allLegalActions[kKingBomb].end());
        }
    }

    return greaterMoves;
}

void dispCardRanks(RankCardsArray &cardRanks)
{
    for (auto cardRank : cardRanks)
    {
        for (auto card : cardRank)
        {
            std::cout << "(" << std::to_string(card.first) << "," << std::to_string(card.second) << ") ";
        }
        std::cout << std::endl;
    }
}

std::string rankCountsArray2String(const RankCountsArray &countsArray)
{
    std::string strCounts = "";
    for (int i = 0; i < RANK_COUNTS - 1; i++)
    {
        strCounts += std::to_string(countsArray[i]) + " ";
    }
    strCounts += std::to_string(countsArray[RANK_COUNTS - 1]);
    return strCounts;
}
int getPokersCounts(RankCountsArray &countsArray)
{
    int counts = 0;
    for (int i = 0; i < RANK_COUNTS - 1; i++)
    {
        counts += countsArray[i];
    }
    return counts;
}

RankMove buildMoveByPokersCounts(RankCountsArray &countsArray)
{
    RankMove move(LandlordMoveType::kInvalid, -1);
    int counts = 0;
    std::vector<std::pair<int, int>> validRanks;
    std::pair<int, int> maxRank = std::make_pair(0, -1);
    int threeStart = -1;
    int threeEnd = -1;
    std::vector<std::pair<int, int>> threeStraights;
    //数量是否相等,用于判断顺子
    bool isSameRankCounts = true;

    for (int i = 0; i < RANK_COUNTS - 1; i++)
    {
        if (countsArray[i] > 0)
        {
            counts += countsArray[i];

            if (isSameRankCounts && validRanks.size() > 0)
            {
                if (countsArray[i] != validRanks[validRanks.size() - 1].second)
                {
                    isSameRankCounts = false;
                }
            }

            validRanks.push_back(std::make_pair(i, countsArray[i]));
            if (countsArray[i] > maxRank.second)
            {
                maxRank = std::make_pair(i, countsArray[i]);
            }

            //查看一个最大的三连组合
            if (countsArray[i] >= 3)
            {
                if (threeStart < 0)
                {
                    threeStart = i;
                }
                else if (i > 0 && countsArray[i - 1] < 3)
                {
                    threeStart = i;
                }
                else if (i == kPokerA_RANK && countsArray[i - 1] >= 3)
                {
                    threeEnd = i;
                    if (threeStart > 0 && threeStart < threeEnd)
                    {
                        threeStraights.push_back(std::make_pair(threeStart, threeEnd));
                    }
                }
            }
            else
            {
                if (i > 0 && countsArray[i - 1] >= 3)
                {
                    threeEnd = i - 1;
                    if (threeStart > 0 && threeStart < threeEnd)
                    {
                        threeStraights.push_back(std::make_pair(threeStart, threeEnd));
                    }
                }
                threeEnd = -1;
                threeStart = -1;
            }
        }
        else if (i > 0 && countsArray[i - 1] >= 3)
        {
            threeEnd = i - 1;
            if (threeStart >= 0 && threeStart < threeEnd)
            {
                threeStraights.push_back(std::make_pair(threeStart, threeEnd));
            }
            threeEnd = -1;
            threeStart = -1;
        }
    }
    if (threeStraights.size() == 1)
    {
        //3顺或3顺带1，是最复杂的组合，特殊处理。
        if (counts % 3 == 0 && (validRanks[validRanks.size() - 1].first - validRanks[0].first + 1) == counts / 3)
        {
            //3顺
            move = RankMove(kThreeStraight, validRanks[0].first, validRanks[validRanks.size() - 1].first);
        }
        else if (counts % 4 == 0)
        {
            //3顺带单
            std::vector<int> added;
            for (auto rank : validRanks)
            {
                if (rank.first < threeStraights[0].first || rank.first > threeStraights[0].second)
                {
                    if (rank.second == 1)
                    {
                        added.push_back(rank.first);
                    }
                }
                else
                {
                    if (rank.second == 4)
                    {
                        added.push_back(rank.first);
                    }
                }
            }
            if (added.size() == threeStraights[0].second - threeStraights[0].first + 1)
            {
                move = RankMove(kThreeStraightAddOne, threeStraights[0].first, threeStraights[0].second, added);
            }
        }
        else if (counts % 5 == 0)
        {
            //3顺带对
            std::vector<int> added;
            for (auto rank : validRanks)
            {
                if (rank.second == 2)
                {
                    added.push_back(rank.first);
                }
            }
            if (added.size() == threeStraights[0].second - threeStraights[0].first + 1)
            {
                move = RankMove(kThreeStraightAddPair, threeStraights[0].first, threeStraights[0].second, added);
            }
        }
    }
    else if (isSameRankCounts && counts >= 6 && counts % 2 == 0 && (validRanks[validRanks.size() - 1].first - validRanks[0].first + 1) == counts / 2)
    {
        //2顺
        move = RankMove(kTwoStraight, validRanks[0].first, validRanks[validRanks.size() - 1].first);
    }
    else if (isSameRankCounts && counts >= 5 && (validRanks[validRanks.size() - 1].first - validRanks[0].first + 1) == validRanks.size())
    {
        //顺子
        move = RankMove(kStraight, validRanks[0].first, validRanks[validRanks.size() - 1].first);
    }
    else if (validRanks.size() == 1)
    {
        //只有一种牌
        switch (validRanks[0].second)
        {
        case 1:
            move = RankMove(kSingle, validRanks[0].first);
            break;
        case 2:
            move = RankMove(kPair, validRanks[0].first);
            break;
        case 3:
            move = RankMove(kThree, validRanks[0].first);
            break;
        case 4:
            move = RankMove(kBomb, validRanks[0].first);
            break;
        }
    }
    else if (validRanks.size() == 2)
    {
        //王炸，或3带1
        if (counts == 2 && validRanks[0].first == kPokerJoker_RANK)
        { //王炸
            move = RankMove(kKingBomb, validRanks[0].first, validRanks[1].first);
        }
        else if (counts == 4)
        {
            //3带单
            if (validRanks[0].second == 3)
            {
                move = RankMove(kThreeAddSingle, validRanks[0].first, validRanks[0].first, {validRanks[1].first});
            }
            else
            {
                move = RankMove(kThreeAddSingle, validRanks[1].first, validRanks[1].first, {validRanks[0].first});
            }
        }
        else if (counts == 5)
        {
            //3带对
            if (validRanks[0].second == 3)
            {
                move = RankMove(kThreeAddPair, validRanks[0].first, validRanks[0].first, {validRanks[1].first});
            }
            else
            {
                move = RankMove(kThreeAddPair, validRanks[1].first, validRanks[1].first, {validRanks[0].first});
            }
        }
    }
    else if (validRanks.size() == 3)
    {
        //4带2
        if (counts == 6)
        {
            //4带2单
            if (maxRank.second == 4)
            {
                if (maxRank.first == validRanks[0].first)
                {
                    move = RankMove(kFourAddTwoSingles, validRanks[0].first, validRanks[0].first,
                                    {validRanks[1].first, validRanks[2].first});
                }
                else if (maxRank.first == validRanks[1].first)
                {
                    move = RankMove(kFourAddTwoSingles, validRanks[1].first, validRanks[1].first,
                                    {validRanks[0].first, validRanks[2].first});
                }
                else if (maxRank.first == validRanks[2].first)
                {
                    move = RankMove(kFourAddTwoSingles, validRanks[2].first, validRanks[2].first,
                                    {validRanks[0].first, validRanks[1].first});
                }
            }
        }
        else if (counts == 8)
        {
            //4带2对
            if (maxRank.second == 4)
            {
                if (maxRank.first == validRanks[0].first && validRanks[1].second == validRanks[2].second)
                {
                    move = RankMove(kFourAddTwoPairs, validRanks[0].first, validRanks[0].first,
                                    {validRanks[1].first, validRanks[2].first});
                }
                else if (maxRank.first == validRanks[1].first && validRanks[0].second == validRanks[2].second)
                {
                    move = RankMove(kFourAddTwoPairs, validRanks[1].first, validRanks[1].first,
                                    {validRanks[0].first, validRanks[2].first});
                }
                else if (maxRank.first == validRanks[2].first && validRanks[0].second == validRanks[1].second)
                {
                    move = RankMove(kFourAddTwoPairs, validRanks[2].first, validRanks[2].first,
                                    {validRanks[0].first, validRanks[1].first});
                }
            }
        }
    }

    return move;
}

RankCountsArray rankMove2Counts(RankMove &move)
{
    RankCountsArray countsArray = {0};
    switch (move.Type())
    {
    case kSingle /* 单 */:
        countsArray[move.StartRank()] = 1;
        break;
    case kPair /* 对 */:
        countsArray[move.StartRank()] = 2;
        break;
    case LandlordMoveType::kThree:
        countsArray[move.StartRank()] = 3;
        break;
    case kThreeAddSingle: //三带一
        countsArray[move.StartRank()] = 3;
        //countsArray[move.AddedRanks()[0]] += 1;
        break;
    case kThreeAddPair: //三带对
        countsArray[move.StartRank()] = 3;
        //countsArray[move.AddedRanks()[0]] += 2;
        break;
    case kFourAddTwoSingles: //四带两单牌
        countsArray[move.StartRank()] = 4;
        //countsArray[move.AddedRanks()[0]] += 1;
        //countsArray[move.AddedRanks()[1]] += 1;
        break;
    case kFourAddTwoPairs: //四带两对牌
        countsArray[move.StartRank()] = 4;
        //countsArray[move.AddedRanks()[0]] += 2;
        //countsArray[move.AddedRanks()[1]] += 2;
        break;
    case kStraight: //顺子
        for (RankType rank = move.StartRank(); rank <= move.EndRank(); rank++)
        {
            countsArray[rank] = 1;
        }
        break;
    case kTwoStraight: //拖拉机，连对
        for (RankType rank = move.StartRank(); rank <= move.EndRank(); rank++)
        {
            countsArray[rank] = 2;
        }
        break;
    case kThreeStraight: //飞机，没翅膀，3连牌
        for (RankType rank = move.StartRank(); rank <= move.EndRank(); rank++)
        {
            countsArray[rank] = 3;
        }
        break;
    case kThreeStraightAddOne:  // 飞机带单翅膀
    case kThreeStraightAddPair: // 飞机带双翅膀
        for (RankType rank = move.StartRank(); rank <= move.EndRank(); rank++)
        {
            countsArray[rank] = 3;
        }
        // for (auto rank : move.AddedRanks())
        // {
        //     countsArray[rank] += 1;
        // }
        break;
    case kBomb: // 炸弹，包括癞子炸弹，需要结合牌值编码比较大小。
        countsArray[move.StartRank()] = 4;
        break;
    case kKingBomb: // 王炸，双王，多数玩法都是最大牌了。
        countsArray[kPokerJoker_RANK] = 1;
        countsArray[kPokerJOKER_RANK] = 1;
        break;
    default:
        break;
    }
    return countsArray;
}

std::vector<RankMove> parseKickerByType(RankCountsArray &rankCounts, LandlordMoveType type)
{
    std::vector<RankMove> moves;
    for (int i = 0; i <= kPokerJOKER_RANK; i++)
    {
        if (type == kSingle)
        {
            if (rankCounts[i] > 0)
            {
                //增加一个单张move
                moves.push_back(RankMove(kSingle, i));
            }
        }
        else if (type == kPair)
        {
            if (rankCounts[i] > 1)
            {
                //增加一个对子move
                moves.push_back(RankMove(kPair, i));
            }
        }
    }

    return moves;
}

/**
 * add 2020/02/07
 */
std::vector<int> parseOrphanPokers(const RankCountsArray &rankCounts, int count)
{
    std::vector<int> orphanPokers;
    for (int i = 0; i <= kPokerA_RANK; i++)
    {
        if (rankCounts[i] == count)
        {
            //增加一个炸弹move
            orphanPokers.push_back(i);
        }
    }
    return orphanPokers;
}

void addAndSortMove(std::vector<RankMove> &moves, const RankMove &newMove)
{
    moves.push_back(newMove);
    std::sort(moves.begin(), moves.end());
}

void parseOrphanPokersByType(const RankCountsArray &rankCounts,
                             RankMoveByType &oneHandCombs)
{
    //孤立牌
    for (int i = 0; i <= kPokerJOKER_RANK; i++)
    {
        if (rankCounts[i] == 1)
        {
            //特殊处理王炸
            if (i == kPokerJOKER_RANK && rankCounts[i - 1] == 1)
            {
                //有王炸
                //oneHandCombs[kKingBomb].push_back(RankMove(kKingBomb, i - 1, i));
                addAndSortMove(oneHandCombs[kKingBomb], RankMove(kKingBomb, i - 1, i));
            }
            else if (i == kPokerJoker_RANK && rankCounts[i + 1] == 1)
            {
                //do nothing,避免拆王炸
            }
            else
            {
                //增加一个单张move
                //oneHandCombs[kSingle].push_back(RankMove(kSingle, i));
                addAndSortMove(oneHandCombs[kSingle], RankMove(kSingle, i));
            }
        }
        if (rankCounts[i] == 2)
        {
            //增加一个对子move
            //oneHandCombs[kPair].push_back(RankMove(kPair, i));
            addAndSortMove(oneHandCombs[kPair], RankMove(kPair, i));
        }
        if (rankCounts[i] == 3)
        {
            //增加一个三张move
            //oneHandCombs[kThree].push_back(RankMove(kThree, i));
            addAndSortMove(oneHandCombs[kThree], RankMove(kThree, i));
        }
        if (rankCounts[i] == 4)
        {
            //增加一个炸弹move
            //oneHandCombs[kBomb].push_back(RankMove(kBomb, i));
            addAndSortMove(oneHandCombs[kBomb], RankMove(kBomb, i));
        }
    }
}

void parseContinueCombs(const RankCountsArray &rankCounts,
                        const std::vector<std::pair<int, int>> &continueRanks,
                        const int continueCounts,
                        const std::vector<int> &threes,
                        const std::vector<int> &bombs,
                        const RankMoveByType &preOneHandCombs,
                        std::vector<RankMoveByType> &allHandCombs)
{
    for (auto continueRank : continueRanks)
    {
        //先提取一个连牌
        RankMoveByType oneHandCombs = preOneHandCombs;
        if (continueCounts == 1)
        {
            //oneHandCombs[kStraight].push_back(RankMove(kStraight, continueRank.first, continueRank.second));
            addAndSortMove(oneHandCombs[kStraight],
                           RankMove(kStraight, continueRank.first, continueRank.second));
        }
        else if (continueCounts == 2)
        {
            //oneHandCombs[kTwoStraight].push_back(RankMove(kTwoStraight, continueRank.first, continueRank.second));
            addAndSortMove(oneHandCombs[kTwoStraight],
                           RankMove(kTwoStraight, continueRank.first, continueRank.second));
        }
        else if (continueCounts == 3)
        {
            //oneHandCombs[kThreeStraight].push_back(RankMove(kThreeStraight, continueRank.first, continueRank.second));
            addAndSortMove(oneHandCombs[kThreeStraight],
                           RankMove(kThreeStraight, continueRank.first, continueRank.second));
        }
        else
        {
            return;
        }
        RankCountsArray newRankCounts = rankCounts;
        //删除顺子牌
        for (int i = continueRank.first; i <= continueRank.second; i++)
        {
            newRankCounts[i] -= continueCounts;
        }
        parseByTypeBase(newRankCounts, oneHandCombs, allHandCombs);
        for (auto three : threes)
        {
            if (three >= continueRank.first && three <= continueRank.second)
            {
                RankMoveByType oneHandCombs = preOneHandCombs;
                //oneHandCombs[kThree].push_back(RankMove(kThree, three));
                addAndSortMove(oneHandCombs[kThree], RankMove(kThree, three));

                RankCountsArray newRankCounts = rankCounts;
                //删除三张牌
                for (int i = continueRank.first; i <= continueRank.second; i++)
                {
                    newRankCounts[three] -= 3;
                }
                parseByTypeBase(newRankCounts, oneHandCombs, allHandCombs);
            }
        }
        for (auto bomb : bombs)
        {
            if (bomb >= continueRank.first && bomb <= continueRank.second)
            {
                RankMoveByType oneHandCombs = preOneHandCombs;
                //oneHandCombs[kBomb].push_back(RankMove(kBomb, bomb));
                addAndSortMove(oneHandCombs[kBomb], RankMove(kBomb, bomb));

                RankCountsArray newRankCounts = rankCounts;
                //删除炸弹牌
                for (int i = continueRank.first; i <= continueRank.second; i++)
                {
                    newRankCounts[bomb] -= 4;
                }
                parseByTypeBase(newRankCounts, oneHandCombs, allHandCombs);
            }
        }
    }
}

std::vector<RankMoveByType> parseHandPokers(const RankCountsArray &rankCounts)
{
    RankMoveByType oneHandCombs;
    std::vector<RankMoveByType> allHandCombs;
    parseByTypeBase(rankCounts, oneHandCombs, allHandCombs);
    std::vector<RankMoveByType> filteredAllHandCombs;
    //std::copy(allHandCombs.begin(), allHandCombs.end(), filteredAllHandCombs.begin());
    //return filteredAllHandCombs;
    return allHandCombs;
}

void parseByTypeBase(const RankCountsArray &rankCounts,
                     const RankMoveByType &preOneHandCombs,
                     std::vector<RankMoveByType> &allHandCombs)
{
    //三张牌
    std::vector<int> threes = parseOrphanPokers(rankCounts, 3);
    //炸弹牌
    std::vector<int> bombs = parseOrphanPokers(rankCounts, 4);

    //查找连续顺子牌
    std::vector<std::pair<int, int>> continueRanks1 = buildContinueRanks(rankCounts, 5, 1);
    parseContinueCombs(rankCounts, continueRanks1, 1, threes, bombs,
                       preOneHandCombs, allHandCombs);
    //查找连续对子牌
    std::vector<std::pair<int, int>> continueRanks2 = buildContinueRanks(rankCounts, 3, 2);
    parseContinueCombs(rankCounts, continueRanks2, 2, threes, bombs,
                       preOneHandCombs, allHandCombs);
    //查找连续三张牌
    std::vector<std::pair<int, int>> continueRanks3 = buildContinueRanks(rankCounts, 2, 3);
    parseContinueCombs(rankCounts, continueRanks3, 3, threes, bombs,
                       preOneHandCombs, allHandCombs);

    if (continueRanks1.size() + continueRanks2.size() + continueRanks3.size() == 0)
    {
        //没有连牌，不需要递归解析
        RankMoveByType oneHandCombs = preOneHandCombs;
        parseOrphanPokersByType(rankCounts, oneHandCombs);
        //allHandCombs.push_back(oneHandCombs);
        bool isNewCombs = true;
        for (auto comb : allHandCombs)
        {
            bool hasSameComb = true;
            for (size_t i = 0; i < comb.size(); i++)
            { //movetype
                auto typeMoves = comb[i];
                if (comb[i].size() != oneHandCombs[i].size())
                {
                    hasSameComb = false;
                    break;
                }
                else
                {
                    for (size_t j = 0; j < comb[i].size(); j++)
                    {
                        if (!(comb[i][j] == oneHandCombs[i][j]))
                        {
                            hasSameComb = false;
                            break;
                        }
                    }
                    if (!hasSameComb){
                        break;
                    }
                }
            }
            if (hasSameComb)
            {
                isNewCombs = false;
                break;
            }
        }
        if (isNewCombs)
        {
            //allHandCombs.insert(oneHandCombs);
            allHandCombs.push_back(oneHandCombs);
        }
    }
}
std::string rankCountsArray2DispString(const RankCountsArray &countsArray){
    std::string str("[");
    for (size_t i = 0; i < countsArray.size(); i++)
    {
        if (countsArray[i] > 0){
            makeRankString(i,countsArray[i],str);
            str += ",";
        }
    }
    str.replace(str.length()-1,1,"");
    str += "]";
    return str;    
}

std::string handCombToDispString(const RankMoveByType handComb){
    std::string str("[");
    for (int i = LandlordMoveType::kSingle; i <= LandlordMoveType::kKingBomb; i++)
    {
        if (handComb[i].size() >0){
            //str += moveType2String((LandlordMoveType)i) +":";
            str += "[";
            for (auto move : handComb[i]){
                 str += move.toArrayString();
            }
            str += "],";
        }
    }
    str.replace(str.length()-1,1,"");
    str += "]";
    return str;
}
std::string allHandCombsToDispString(const std::vector<RankMoveByType> allHandCombs){
    std::string str{""};
    int count = 0;
    for (auto combs : allHandCombs){
        char lineBuffer[10];
        snprintf ( lineBuffer, 10, "%3d", ++count );
        str +=std::string(lineBuffer) + " : " +  handCombToDispString(combs)+"\n";
    }
    str.replace(str.length()-1,1,"");
    return str;
}

} // namespace landlord_learning_env