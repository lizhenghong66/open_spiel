#include "landlord_action.h"

#include <algorithm>
#include <cassert>
#include <fstream>

namespace landlord_learning_env
{
int encodeStraight(RankMove &move)
{
    int action = -1;
    int len = move.EndRank() - move.StartRank() + 1;
    switch (len)
    {
    case 12:
        action = Action_Straight_12;
        break;
    case 11:
        action = Action_Straight_11 + move.StartRank();
        break;
    case 10:
        action = Action_Straight_10 + move.StartRank();
        break;
    case 9:
        action = Action_Straight_9 + move.StartRank();
        break;
    case 8:
        action = Action_Straight_8 + move.StartRank();
        break;
    case 7:
        action = Action_Straight_7 + move.StartRank();
        break;
    case 6:
        action = Action_Straight_6 + move.StartRank();
        break;
    case 5:
        action = Action_Straight_5 + move.StartRank();
        break;
    }
    return action;
}

int encodeTwoStraight(RankMove &move)
{
    int action = -1;
    int len = move.EndRank() - move.StartRank() + 1;
        switch (len)
        {
        case 10:
            action = Action_TwoStraight_10 + move.StartRank();
            break;
        case 9:
            action = Action_TwoStraight_9 + move.StartRank();
            break;
        case 8:
            action = Action_TwoStraight_8 + move.StartRank();
            break;
        case 7:
            action = Action_TwoStraight_7 + move.StartRank();
            break;
        case 6:
            action = Action_TwoStraight_6 + move.StartRank();
            break;
        case 5:
            action = Action_TwoStraight_5 + move.StartRank();
            break;
        case 4:
            action = Action_TwoStraight_4 + move.StartRank();
            break;
        case 3:
            action = Action_TwoStraight_3 + move.StartRank();
            break;
        }
    return action;
}

int encodeThreeStraight(RankMove &move)
{
    int action = -1;
     int len = move.EndRank() - move.StartRank() + 1;
        switch (len)
        {
        case 2:
            action = Action_ThreeStraight_2 + move.StartRank();
            break;
        case 3:
            action = Action_ThreeStraight_3 + move.StartRank();
            break;
        case 4:
            action = Action_ThreeStraight_4 + move.StartRank();
            break;
        case 5:
            action = Action_ThreeStraight_5 + move.StartRank();
            break;
        }
    return action;
}


int encodeThreeAdd(RankMove &move,int base)
{
    int action = -1;
     int code = move.StartRank() * 13;
        if (move.AddedRanks().size() == 1)
        {
            if (move.AddedRanks()[0] < move.StartRank())
            {
                code += move.AddedRanks()[0];
                action = base + code;
            }
            else if (move.AddedRanks()[0] < move.StartRank())
            {
                code += move.AddedRanks()[0] - 1;
                action = base + code;
            }
        }
    return action;
}

int encodeFourAddTwo(RankMove &move,int base)
{
    int action = -1;
     int code = move.StartRank() * 13;
     if (move.AddedRanks().size() == 2)
        {
            std::vector<std::vector<int>> cmbs = landlord_learning_env::combine(4,2);
            
            if (move.AddedRanks()[0] < move.StartRank())
            {
                code += move.AddedRanks()[0];
                action = base + code;
            }
            else if (move.AddedRanks()[0] < move.StartRank())
            {
                code += move.AddedRanks()[0] - 1;
                action = base + code;
            }
        }
    return action;
}

int encodeThreeStraightAddSingles(RankMove &move)
{
    int action = -1;
    return action;
}

int encodeThreeStraightAddPairs(RankMove &move)
{
    int action = -1;
    return action;
}

int rankMove2Action(RankMove &move)
{
    int action = -1;
    switch (move.Type())
    {
    case kSingle:
        action = Action_Single + move.StartRank();
        break;
    case kPair:
        action = Action_Pair + move.StartRank();
        break;
    case kThree:
        action = Action_Three + move.StartRank();
        break;
    case kBomb:
        action = Action_Bomb + move.StartRank();
        break;
    case kKingBomb:
        action = Action_KingBomb;
        break;
    case kStraight:
        action = encodeStraight(move);
        break;
    case kTwoStraight:
        action = encodeTwoStraight(move);
        break;
    case kThreeStraight:
       action = encodeThreeStraight(move);
        break;
    case kThreeAddSingle:
        action = encodeThreeAdd(move,Action_ThreeAddSingle);
        break;
    case kThreeAddPair:
        action = encodeThreeAdd(move,Action_ThreeAddPair);
        break;
    case kFourAddTwoSingles:
        int code = move.StartRank() * 13;
        if (move.AddedRanks().size() == 1)
        {
            if (move.AddedRanks()[0] < move.StartRank())
            {
                code += move.AddedRanks()[0];
                action = Action_ThreeAddPair + code;
            }
            else if (move.AddedRanks()[0] < move.StartRank())
            {
                code += move.AddedRanks()[0] - 1;
                action = Action_ThreeAddPair + code;
            }
        }
        break;
    }
}

RankMove action2RankMove(int action)
{
}
/**
 * 斗地主Action过大，主要是因为飞机带牌。
 * 简单编码虽然可行，但是python部分也有Action合法性检测，还是必须把Action数量降低。
 * 所有穷举了所有合法Action。然后构造一个数组，用下标作为Action，
 * 这样可以把Action数量（OpenSpiel计算的最大Action作为Acion数量）尽可能限定在较小的范围。
 */
std::vector<int> initUserActions()
{
    std::vector<int> actions;
    std::ifstream inFile("actions.bin", std::ios::in | std::ios::binary);
    if (inFile.good())
    {
        int action;
        while (inFile.read((char *)&action, sizeof(action)))
        { //一直读到文件结束
            actions.push_back(action);
        }
        inFile.close();
    }
    else
    {
        actions = buildAllValidActions();
        std::ofstream outFile("actions.bin", std::ios::out | std::ios::binary);
        for (auto action : actions)
        {
            //outFile <<  action ;
            outFile.write((char *)&action, sizeof(action));
        }
        outFile.close();
    }
    return actions;
}

std::vector<int> AllValidUserActions = initUserActions();
int toAction4(int action){
    RankMove move(kInvalid,0);
    int action4 = move2Action4(move);
    if (action >= 0 && action < AllValidUserActions.size()){
        action4 = AllValidUserActions[action];
    }
    return action4;
}
LandlordMoveType decodeActionType5(int action){
    int action4 = toAction4(action);
    return decodeActionType4(action4);
}
RankMove action2Move5(int action){
    int action4 = toAction4(action);
    return action2Move4(action4);
}
int move2Action5(RankMove &move){
    int action5 = -1;
    int action4 = move2Action4(move);
    auto itr = std::find(AllValidUserActions.begin(),AllValidUserActions.end(),action4);
    if (itr != AllValidUserActions.end()) {
       action5 = std::distance(AllValidUserActions.begin(), itr);
    }else{
        int count = 0;
        for (auto action : AllValidUserActions){
            std::cout << count++ << " : " << action << " ===> " << action2Move4(action).toString() << std::endl;
        }
        std::cout << "unrecognized move:" << move.toString()  << " ," << action4 << std::endl;
        exit(-1);
    } 
    return action5;
}

} // namespace landlord_learning_env