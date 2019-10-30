#include <iostream>
#include <assert.h>

#include "landlord_parser.h"

namespace landlord_learning_env {
    std::vector<LandlordCard> toCards( std::vector<std::array<int,2>> &pokers){
            std::vector<LandlordCard> cards;
            for (auto poker : pokers){
                cards.push_back(std::make_pair(poker[0],poker[1]));
            }
            return cards;
    }
    void parser_test1(){
        std::vector<std::array<int,2>> pokers = {{0,0},{0,1},{1,1},{2,1},
        {1,2},{2,2},{3,2},{0,3},{1,3},{3,4},{2,4},{1,5},{2,6},{0,8},{1,8},{2,8},{3,8}};
        std::vector<LandlordCard> hands = toCards(pokers);
        assert(hands.size() == 17);
        std::array<RankType,RANK_COUNTS> rankCounts = buildRankCounts(hands);
        std::array<RankType,RANK_COUNTS> expectedRankCounts ={1,3,3,2,2,1,1,0,4,0,0,0,0,0,0,0};
        assert(rankCounts == expectedRankCounts);
        std::cout << "tesk1 buildRankCounts  ok!" << std::endl;

        //查找连续顺子牌
        std::vector<std::pair<int,int>> continueRanks1 = buildContinueRanks(rankCounts,5,1);
        std::vector<std::pair<int,int>> expected1{{0,6},{0,5},{0,4},{1,6},{1,5},{2,6}};
        assert(continueRanks1 == expected1);
        //查找连续对子牌
        std::vector<std::pair<int,int>> continueRanks2 = buildContinueRanks(rankCounts,3,2);
         std::vector<std::pair<int,int>> expected2{{1,4},{1,3},{2,4}};
        assert(continueRanks2 == expected2);
        //查找连续三张牌
        std::vector<std::pair<int,int>> continueRanks3 = buildContinueRanks(rankCounts,2,3);
        std::vector<std::pair<int,int>> expected3{{1,2}}; 
        assert(continueRanks3 == expected3);
        std::cout << "tesk1  ok!" << std::endl;
    }

    void parser_test2(){
        std::vector<LandlordCard> hands = {{0,4},{0,5},{1,5},{2,5},
        {1,6},{2,6},{3,6},{0,7},{1,7},{3,8},{2,8},{1,9},{3,9},{3,10},{2,10},
        {0,10},{0,11},{1,11},{2,11},{3,11}};;
        assert(hands.size() == 20);
        std::array<RankType,RANK_COUNTS> rankCounts = buildRankCounts(hands);
        std::array<RankType,RANK_COUNTS> expectedRankCounts =
            {0,0,0,0,1,3,3,2,2,2,3,4};
        assert(rankCounts == expectedRankCounts);
        std::cout << "tesk2  rankCounts ok!" << std::endl;

        //查找连续顺子牌
        std::vector<std::pair<int,int>> continueRanks1 = buildContinueRanks(rankCounts,5,1);
        std::vector<std::pair<int,int>> expected1{{4,11},{4,10},{4,9},{4,8},{5,11},{5,10},{5,9},{6,11},{6,10},{7,11}};
        assert(continueRanks1 == expected1);
        //查找连续对子牌
        std::vector<std::pair<int,int>> continueRanks2 = buildContinueRanks(rankCounts,3,2);
         std::vector<std::pair<int,int>> expected2{{5,11},{5,10},{5,9},
            {5,8},{5,7},{6,11},{6,10},{6,9},{6,8},{7,11},{7,10},{7,9},{8,11},{8,10},{9,11}};
        assert(continueRanks2 == expected2);
        //查找连续三张牌
        std::vector<std::pair<int,int>> continueRanks3 = buildContinueRanks(rankCounts,2,3);
        std::vector<std::pair<int,int>> expected3{{5,6},{10,11}}; 
        assert(continueRanks3 == expected3);
        std::cout << "tesk2  ok!" << std::endl;
    }

}//namespace landlord_learning_env

int main(int argc, char** argv) {
   landlord_learning_env::parser_test1();
   landlord_learning_env::parser_test2();
}