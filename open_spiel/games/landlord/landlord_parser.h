//对手牌进行解析
//先不考虑癞子牌
#ifndef __LANDLORD_PARSER_H__
#define __LANDLORD_PARSER_H__
#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <utility>

#include "landlord_card.h"
#include "landlord_move.h"

namespace landlord_learning_env {
    struct RankMove {
        LandlordMove::Type type; 
        RankType startRank;
        RankType endRank; //非连牌时与startRank相同或忽略（没设置）
        std::vector<RankType> addedRanks; //带牌时带牌的rank列表。  
    };

    void addSubContinueRanks(std::vector<std::pair<int,int>> &continueRanks,
        int startRank,int endRank,int minCount){
            while(endRank  -  startRank >= minCount){
                continueRanks.push_back(std::make_pair(startRank,endRank-1));
                endRank--;
            }
    }

    std::vector<std::pair<int,int>> buildContinueRanks(
        std::array<RankType,RANK_COUNTS> &rankCounts,   //牌等级数量数组
        int minCount,  //最少连续数量，如顺子最少5连张。
        int count            //每种牌数量。
        ){
        //查找连续的牌
        std::vector<std::pair<int,int>> continueRanks;
        for (int i = 0; i <= kPokerA_RANK-minCount+1; i++){
            if (rankCounts[i] >= count){//连续牌的下值
                for (int j = i+1; j <= kPokerA_RANK; j++){
                    if (rankCounts[j] < count){ //至少两个以上的连牌
                        if (  j-i >= minCount){
                            continueRanks.push_back(std::make_pair(i,j-1));
                            addSubContinueRanks(continueRanks,i,j-1,minCount);
                        }
                        break;
                    }else if (j == kPokerA_RANK && rankCounts[j] >= count && j-i +1>= minCount){
                        continueRanks.push_back(std::make_pair(i,j));
                        addSubContinueRanks(continueRanks,i,j,minCount);
                    }
                }
            }
        }
        return continueRanks;
    }

    std::array<RankType,RANK_COUNTS> buildRankCounts(std::vector<LandlordCard> &cards){
        std::array<RankType,RANK_COUNTS> rankCounts  = {};
        std::array<std::vector<LandlordCard>,RANK_COUNTS> cardRanks = {{}};
        for (LandlordCard card  : cards){
            rankCounts[card.second]++;
            cardRanks[card.second].push_back(card);
        }

        for (auto cardRank : cardRanks){
            for (auto card : cardRank){
                std::cout << "(" << std::to_string(card.first)  << "," << std::to_string(card.second) << ") ";
            }
            std::cout << std::endl;
        }
        return rankCounts;
    }

    inline void parse(std::vector<LandlordCard> cards){
        std::array<RankType,RANK_COUNTS> rankCounts = buildRankCounts(cards);
        //build all possible move(action)
        //single
        for (int i = 0; i <= kPokerJOKER_RANK; i++){
            if (rankCounts[i] > 0){
                //增加一个单牌move
            }
        }
        //对牌
        for (int i = 0; i <= kPokerJOKER_RANK; i++){
            if (rankCounts[i] > 1){
                //增加一个对牌move
            }
        }
        //三不带
        for (int i = 0; i <= kPokerJOKER_RANK; i++){
            if (rankCounts[i] > 2){
                //增加一个三不带move
            }
        }

        //炸弹
        for (int i = 0; i <= kPokerJOKER_RANK; i++){
            if (rankCounts[i] > 3){
                //增加一个炸弹move
            }
        }

        for (int i = 0; i <= kPokerJOKER_RANK; i++){
            if (rankCounts[i] > 0){
                //增加一个单张move

                //特殊处理王炸
                if (i == kPokerJOKER_RANK && rankCounts[i-1] > 0){
                    //有王炸
                }
            }else  if (rankCounts[i] > 1){
                //增加一个对子move
            }else if (rankCounts[i] > 2){
                //增加一个三张move
            }else if (rankCounts[i] > 3){
                //增加一个炸弹move
            }
        }

        //查找连续顺子牌
        std::vector<std::pair<int,int>> continueRanks1 = buildContinueRanks(rankCounts,5,1);
        
        //查找连续对子牌
        std::vector<std::pair<int,int>> continueRanks2 = buildContinueRanks(rankCounts,3,2);
        
        //查找连续三张牌
        std::vector<std::pair<int,int>> continueRanks3 = buildContinueRanks(rankCounts,2,3);

        //然后处理带牌情况，3带，3顺带，4带，
    }

    
}// namespace landlord_learning_env

#endif  //__LANDLORD_PARSER_H__