#include "landlord_parser.h"

namespace landlord_learning_env
{
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
    std::array<RankType, RANK_COUNTS> &rankCounts, //牌等级数量数组
    int minCount,                                  //最少连续数量，如顺子最少5连张。
    int count                                      //每种牌数量。
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

    return std::make_pair(rankCounts,cardRanks);
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

void deletePokerFromPokers(std::vector<int> pokers, int poker)
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
        for (auto cmb : cmbs)
        {
            std::vector<int> addPokers;
            for (auto mayAdd : cmb)
            {
                addPokers.push_back(mayAddPokers[mayAdd]);
            }
            allLegalActions[type].push_back(
                RankMove(type, start, end, addPokers));
        }
    }
}

std::vector<RankMove> parse(RankCountsArray rankCounts)
{
    //std::set<std::array<std::vector<RankMove>,MAX_RANK>> allLegalActions;
    std::array<std::vector<RankMove>, MOVE_COUNTS> allLegalActions;
    //std::array<RankType, RANK_COUNTS> rankCounts = buildRankCounts(cards);
    //build all possible move(action)
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
                allLegalActions[kKingBomb].push_back(RankMove(kKingBomb, i));
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
        std::vector<int> mayAddPokers;
        int len = continue3.second - continue3.first + 1;
        for (auto single : singles)
        {
            if (single < continue3.first || single > continue3.second)
            {
                mayAddPokers.push_back(single);
            }
        }
        updatedAddPokers(kThreeStraightAddOne,continue3.first ,continue3.second,len,mayAddPokers,allLegalActions);

        //3顺带对
        mayAddPokers.resize(0);
        for (auto pair : pairs)
        {
            if (pair < continue3.first || pair > continue3.second)
            {
                mayAddPokers.push_back(pair);
            }
        }
        updatedAddPokers(kThreeStraightAddPair,continue3.first ,continue3.second,len,mayAddPokers,allLegalActions);
    }
    //然后处理带牌情况，3带，4带，
    //处理3带1
    for (auto three : threes){
         //处理3带单
        std::vector<int> mayAddPokers = singles;
        int len = 1;
        deletePokerFromPokers(mayAddPokers,three);
        updatedAddPokers(kThreeStraightAddOne,three ,three,len,mayAddPokers,allLegalActions);

        //处理3带对
        mayAddPokers = pairs;
        deletePokerFromPokers(mayAddPokers,three);
        updatedAddPokers(kThreeStraightAddPair,three ,three,len,mayAddPokers,allLegalActions);
    }
    //处理4带2
    for (auto bomb : bombs){
         //处理4带单
        std::vector<int> mayAddPokers = singles;
        int len = 2;
        deletePokerFromPokers(mayAddPokers,bomb);
        updatedAddPokers(kFourAddTwoSingles,bomb ,bomb,len,mayAddPokers,allLegalActions);

        //处理4带对
        mayAddPokers = pairs;
        deletePokerFromPokers(mayAddPokers,bomb);
        updatedAddPokers(kFourAddTwoPairs,bomb ,bomb,len,mayAddPokers,allLegalActions);
    }

    std::vector<RankMove> allActions;
    for (int i = 0; i < allLegalActions.size();i++){
        auto actions =  allLegalActions[i];
        if (actions.size() > 0){
            allActions.insert(allActions.end(),actions.begin(),actions.end());
        }
    }

    return allActions;
}

void dispCardRanks(RankCardsArray &cardRanks){
   for (auto cardRank : cardRanks)
    {
        for (auto card : cardRank)
        {
            std::cout << "(" << std::to_string(card.first) << "," << std::to_string(card.second) << ") ";
        }
        std::cout << std::endl;
    }
}

} // namespace landlord_learning_env