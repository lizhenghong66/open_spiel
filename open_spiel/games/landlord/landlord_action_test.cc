#include <iostream>
#include <assert.h>

#include "landlord_action.h"
#include "landlord_parser.h"
namespace landlord_learning_env {
    void chkMoveEncode(RankMove move){
        int action = move2Action4(move);
        RankMove decodedMove = action2Move4(action);
        std::cout << move.toString() << " ====> " << decodedMove.toString() << std::endl;
        assert (move == decodedMove);
    }
    void disp(std::vector<RankCountsArray> handCombines){
        int counts = 10;
        std::cout << "前 " << counts << "个组合" << std::endl;
        for(int i =0; i < counts; i++){
            std::cout << rankCountsArray2String(handCombines[i]) <<std::endl;
        }
        std::cout << "后 " << counts << "个组合" << std::endl;
        for(int i =counts; i > 0; i--){
            std::cout << rankCountsArray2String(handCombines[handCombines.size()-i]) <<std::endl;
        }
    }
    void action_test1(){
        chkMoveEncode(RankMove(kSingle,12));
        chkMoveEncode(RankMove(kSingle,kPokerA_RANK));
        chkMoveEncode(RankMove(kPair,kPokerA_RANK));
        chkMoveEncode(RankMove(kThree,kPokerA_RANK));
        chkMoveEncode(RankMove(kBomb,kPokerA_RANK));
        chkMoveEncode(RankMove(kKingBomb,kPokerJoker_RANK,kPokerJOKER_RANK));

        chkMoveEncode(RankMove(kStraight,kPoker3_RANK,kPokerA_RANK));
        chkMoveEncode(RankMove(kTwoStraight,kPoker3_RANK,4));
        chkMoveEncode(RankMove(kThreeStraight,kPoker3_RANK,3));

        chkMoveEncode(RankMove(kThreeAddSingle,kPoker3_RANK,kPoker3_RANK,{4}));
        chkMoveEncode(RankMove(kThreeAddPair,kPoker3_RANK,kPoker3_RANK,{4}));

        chkMoveEncode(RankMove(kFourAddTwoSingles,kPoker3_RANK,kPoker3_RANK,{4,8}));
        chkMoveEncode(RankMove(kFourAddTwoPairs,kPoker3_RANK,kPoker3_RANK,{3,7}));

        chkMoveEncode(RankMove(kThreeStraightAddOne,kPoker3_RANK,2,{4,7,9}));
        chkMoveEncode(RankMove(kThreeStraightAddPair,kPoker3_RANK,2,{5,8,10}));

        chkMoveEncode(RankMove(kThreeStraightAddOne,kPoker3_RANK,2,{0,7,9}));
        std::cout << "action counts encode decoding success ..." << std::endl;

        std::vector<std::array<int,16>> handCombines = parseHandCombines(17);
        std::cout << "叫牌前的手牌rank组合总数："  << handCombines.size() << std::endl;
        disp(handCombines);


        uint64_t combineCounts = parseHandCombinesCounts(20);
        std::cout << "地主的手牌rank组合总数："  << std::to_string(combineCounts) << std::endl;

        combineCounts = parseHandCombinesCounts(handCombines,17);
        std::cout << "三家手牌组合总数："  << combineCounts << std::endl;

        //disp(handCombines);
    }
}//namespace landlord_learning_env

int main(int argc, char** argv) {
   landlord_learning_env::action_test1();
}
