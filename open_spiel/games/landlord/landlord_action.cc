#include "landlord_action.h"

#include <algorithm>
#include <cassert>

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

} // namespace landlord_learning_env