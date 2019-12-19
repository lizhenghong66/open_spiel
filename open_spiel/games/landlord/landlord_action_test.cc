#include <iostream>
#include <fstream>
#include <assert.h>
#include <chrono>

#include "landlord_action.h"
#include "landlord_parser.h"
namespace landlord_learning_env
{
void chkMoveEncode(RankMove move)
{
#if 0
    int action = move2Action4(move);
    RankMove decodedMove = action2Move4(action);
    std::cout << move.toString() << " ====> " << decodedMove.toString() << std::endl;
    assert(move == decodedMove);
#else
    int action = move2Action5(move);
    RankMove decodedMove = action2Move5(action);
    if (move.Type() == LandlordMoveType::kThreeStraightAddOne &&
        (move.EndRank()-move.StartRank()) == 4 ||
        move.Type() == LandlordMoveType::kThreeStraightAddPair &&
        (move.EndRank() - move.StartRank()) == 3){
            decodedMove = RankMove(decodedMove.Type(),decodedMove.StartRank(),decodedMove.EndRank(),
                move.AddedRanks());
            // for (auto addRank : move.AddedRanks()){
            //     decodedMove.AddedRanks().push_back(addRank);
            // }
    }
    std::cout << move.toString() << " ====> " << decodedMove.toString() << std::endl;
    assert(move == decodedMove);
#endif
}
void disp(std::vector<RankCountsArray> handCombines)
{
    int counts = 10;
    std::cout << "前 " << counts << "个组合" << std::endl;
    for (int i = 0; i < counts; i++)
    {
        std::cout << rankCountsArray2String(handCombines[i]) << std::endl;
    }
    std::cout << "后 " << counts << "个组合" << std::endl;
    for (int i = counts; i > 0; i--)
    {
        std::cout << rankCountsArray2String(handCombines[handCombines.size() - i]) << std::endl;
    }
}
void action_test1()
{
    chkMoveEncode(RankMove(kSingle, 12));
    chkMoveEncode(RankMove(kSingle, kPokerA_RANK));
    chkMoveEncode(RankMove(kPair, kPokerA_RANK));
    chkMoveEncode(RankMove(kThree, kPokerA_RANK));
    chkMoveEncode(RankMove(kBomb, kPokerA_RANK));
    chkMoveEncode(RankMove(kKingBomb, kPokerJoker_RANK, kPokerJOKER_RANK));

    chkMoveEncode(RankMove(kStraight, kPoker3_RANK, kPokerA_RANK));
    chkMoveEncode(RankMove(kTwoStraight, kPoker3_RANK, 4));
    chkMoveEncode(RankMove(kThreeStraight, kPoker3_RANK, 3));

    chkMoveEncode(RankMove(kThreeAddSingle, kPoker3_RANK, kPoker3_RANK, {4}));
    chkMoveEncode(RankMove(kThreeAddPair, kPoker3_RANK, kPoker3_RANK, {4}));

    chkMoveEncode(RankMove(kFourAddTwoSingles, kPoker3_RANK, kPoker3_RANK, {4, 8}));
    chkMoveEncode(RankMove(kFourAddTwoPairs, kPoker3_RANK, kPoker3_RANK, {3, 7}));

    chkMoveEncode(RankMove(kThreeStraightAddOne, kPoker3_RANK, 2, {4, 7, 9}));
    chkMoveEncode(RankMove(kThreeStraightAddPair, kPoker3_RANK, 2, {5, 8, 10}));

    chkMoveEncode(RankMove(kThreeStraightAddOne, kPoker3_RANK, 2, {0, 7, 9}));

    chkMoveEncode(RankMove(kThreeStraightAddOne, kPoker3_RANK, 4, {5, 5,7,7, 9}));
    chkMoveEncode(RankMove(kThreeStraightAddPair, kPoker3_RANK, 3, {5, 8,9, 10}));
std::cout << "action counts encode decoding success ..." << std::endl;

    // std::vector<std::array<int, 16>> handCombines = parseHandCombines(17);
    // std::cout << "叫牌前的手牌rank组合总数：" << handCombines.size() << std::endl;
    // disp(handCombines);

    // uint64_t combineCounts = parseHandCombinesCounts(20);
    // std::cout << "地主的手牌rank组合总数：" << std::to_string(combineCounts) << std::endl;

    //combineCounts = parseHandCombinesCounts(handCombines,17);
    //std::cout << "三家手牌组合总数："  << combineCounts << std::endl;

    //disp(handCombines);
}

void action_test2()
{
    std::set<int> validActions;
    for (uint i = 0; i < 0xFFFFFFFF; i++)
    {
        RankMove move = action2Move4(i);
        if (move.Type() != kInvalid)
        {
            RankCountsArray counts = rankMove2Counts(move);
            bool isValidMove = true;
            for (int j = 0; j < counts.size(); j++)
            {
                if (counts[j] > 4)
                {
                    isValidMove = false;
                }
            }
            if (isValidMove)
            {
                validActions.insert(i);
            }
        }
        if (i % 10000 == 0)
        {
            std::cout << validActions.size() - 1 << " : " << i << " ==> " << move.toString() << std::endl;
        }
    }
    int count = 0;
    std::cout << "==================" << std::endl;
    for (auto action : validActions)
    {
        std::cout << count++ << " : " << action << " ==> " << action2Move4(action).toString() << std::endl;
    }
}

void action_test3()
{
    std::set<RankMove> allMoves = buildAllValidMoves();
    std::cout << "all moves size:" << allMoves.size() << std::endl;
    int count = 0;
    for (std::set<RankMove>::iterator it = allMoves.begin(); it != allMoves.end(); ++it)
    {
        std::cout << count++ << " : " << (*it).toString() << std::endl;
    }
}
void action_test4()
{
    auto start = std::chrono::system_clock::now();
    std::vector<int> actions = buildAllValidActions();
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "花费了"
              << double(duration.count()) * std::chrono::microseconds::period::num /
                     std::chrono::microseconds::period::den
              << "秒" << std::endl;

    std::cout << "all actions size:" << actions.size() << std::endl;
    int count = 0;
    for (auto action : actions)
    {
        std::cout << count++ << " : " << action << " ===> " << action2Move4(action).toString() << std::endl;
    }

    auto start1 = std::chrono::system_clock::now();
    std::ifstream inFile("actions.bin", std::ios::in | std::ios::binary);
    if (inFile.good())
    {
        int action;
        while (inFile.read((char *)&action, sizeof(action)))
        { //一直读到文件结束
            std::cout << action << " ==> " << action2Move4(action).toString() << std::endl;
        }
        inFile.close();
    }
    else
    {
        std::ofstream outFile("actions.bin", std::ios::out | std::ios::binary);
        for (auto action : actions)
        {
            //outFile <<  action ;
            outFile.write((char *)&action, sizeof(action));
        }
        outFile.close();
    }
    auto end1 = std::chrono::system_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);
    std::cout << "花费了"
              << double(duration1.count()) * std::chrono::microseconds::period::num /
                     std::chrono::microseconds::period::den
              << "秒" << std::endl;
}

void action_test5()
{
    auto start1 = std::chrono::system_clock::now();
    std::ifstream inFile("actions.bin", std::ios::in | std::ios::binary);
    if (inFile.good())
    {
        std::vector<int> actions;
        int action;
        while (inFile.read((char *)&action, sizeof(action)))
        { //一直读到文件结束
            //std::cout << action << " ==> " << action2Move4(action).toString() << std::endl;
            actions.push_back(action);
        }
        inFile.close();
        std::cout << "restore all actions from file,size = " << actions.size();
    }
    else
    {
        auto start = std::chrono::system_clock::now();
        std::vector<int> actions = buildAllValidActions();
        auto end = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "花费了"
                  << double(duration.count()) * std::chrono::microseconds::period::num /
                         std::chrono::microseconds::period::den
                  << "秒" << std::endl;

        std::cout << "all actions size:" << actions.size() << std::endl;
        int count = 0;
        for (auto action : actions)
        {
            std::cout << count++ << " : " << action << " ===> " << action2Move4(action).toString() << std::endl;
        }
        std::ofstream outFile("actions.bin", std::ios::out | std::ios::binary);
        for (auto action : actions)
        {
            //outFile <<  action ;
            outFile.write((char *)&action, sizeof(action));
        }
        outFile.close();
    }
    auto end1 = std::chrono::system_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);
    std::cout << "花费了"
              << double(duration1.count()) * std::chrono::microseconds::period::num /
                     std::chrono::microseconds::period::den
              << "秒" << std::endl;
}

void action_test6(){
    std::vector<int> actions = buildAllValidActions7();
    for(int i = 0; i < actions.size(); i++){
        RankMove move = action2Move4(actions[i]);
        std::cout << i << " : "  << actions[i] << " <===> " << move.toString() << std::endl;
        std::tuple<bool,LandlordMoveType,int>  compAction = chkCompositeAction(move);
        // std::tuple_element<0, decltype(compAction)>::type isComposite = std::get<0>(compAction); 
        // std::tuple_element<1, decltype(compAction)>::type kickerType = std::get<1>(compAction); 
        // std::tuple_element<2, decltype(compAction)>::type kickerLen = std::get<2>(compAction); 
        auto isComposite = std::get<0>(compAction); 
        auto kickerType = std::get<1>(compAction); 
        auto kickerLen = std::get<2>(compAction); 
        std::cout << isComposite << "," << kickerType << "," << kickerLen << std::endl;
        std::cout << std::get<0>(compAction) << "," << std::get<1>(compAction) << "," << std::get<2>(compAction) << std::endl;
    }
    std::cout << " total action counts: " << actions.size() << std::endl;

}
} //namespace landlord_learning_env

int main(int argc, char **argv)
{
    //landlord_learning_env::action_test1();
    //landlord_learning_env::action_test2();
    //landlord_learning_env::action_test3();
    //landlord_learning_env::action_test4();
    //landlord_learning_env::action_test5();
    landlord_learning_env::action_test6();
}
