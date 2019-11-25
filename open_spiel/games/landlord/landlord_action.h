/**
 * Action 简单编码不难，但是因为游戏规则原因，出牌灵活度很高，导致可能的Action数量很大。
 * 但是测试代码要校验Action的合法性，是简单的把所有可能的Aaction作为一个完整0，1数组。
 * 因此需要考虑这个数组的大小。
 * 
 * ALT1：ActionType + 牌数量数组（位置表示rank，值表示数量）。
 * ALT2：RankMove对应的内存布局
 *             ActionType：5bit
 *             Start: 4 bit
 *             End:4bit
 *             Added: 4*5 bit              主要是3连带牌，理论上最多5连
 *           
 * ALT3：穷举所有可能的出牌组合，然后编码。 
 *      单：15
 *      双： 13
 *     三：  13
 *     炸(4) : 13
 *     单顺：1+2+3+4+5+6+7+8 = 36
 *     双顺：3+4+... + 10 = 52
 *     三顺：8+9+10+11 = 38
 *     三带单：13 * C(14,1) = 13 * 14 = 182
 *     三带对：13 * C(12,1) = 13 * 12 = 156
 *     四带单：13 * C(14,2) = 13 * 91 = 1183
 *     四带对：13 * C(12,2) = 13 * 66 = 858
 *     三顺带单：8*C(10,5)     //5连张
 *                           +9 *C(11,4)
 *                           +10 * C(12,3)
 *                            +11 * C(13,2)
 *                           =  8044
 *     三顺带对：8*C(8,5)     //5连张
 *                           +9 *C(9,4)
 *                           +10 * C(10,3)
 *                            +11 * C(11,2)
 *                           =  3377
 * 合计：12277
 * 
 *     //2019/11/20，先前分析过于理想化了，
 *    ALT1方案测试后，发现open_spiel的Action虽然是64bit，但是测试代码很多地方都是32bit，
 *         所以要修改测试代码，不确定还是否有其他影响。
 *    ALT3顺序编码，但是实现时发现遗漏了很多可能（主要是带牌先前没考虑到拆分情况，一旦考虑，action数量猛增）。
 * 
 * ALT4：RankMove对应的内存布局）（不考虑癞子牌）
 *             ActionType：4bit 
 *             Start: 4 bit
 *             End:4bit
 *             Added: 5*4 bit              主要是3连带牌，理论上最多5连
 */
#ifndef __LANDLORD_ACTION_H__
#define __LANDLORD_ACTION_H__
#include "landlord_move.h"
#include "landlord_parser.h"
#include "util.h"

#include  <iostream>

namespace landlord_learning_env
{
constexpr int Action_Base = 1000;
constexpr int Action_Single = Action_Base;
constexpr int Action_Pair = Action_Single + 15;
constexpr int Action_Three = Action_Pair + 13;
constexpr int Action_Bomb = Action_Three + 13;
constexpr int Action_KingBomb = Action_Bomb + 13;

constexpr int Action_Straight = Action_KingBomb + 1;
constexpr int Action_Straight_12 = Action_Straight;
constexpr int Action_Straight_11 = Action_Straight + 1;
constexpr int Action_Straight_10 = Action_Straight + 2;
constexpr int Action_Straight_9 = Action_Straight + 3;
constexpr int Action_Straight_8 = Action_Straight + 4;
constexpr int Action_Straight_7 = Action_Straight + 5;
constexpr int Action_Straight_6 = Action_Straight + 6;
constexpr int Action_Straight_5 = Action_Straight + 7;

constexpr int Action_TwoStraight = Action_Straight + 36;
constexpr int Action_TwoStraight_10 = Action_TwoStraight;
constexpr int Action_TwoStraight_9 = Action_TwoStraight + 3;
constexpr int Action_TwoStraight_8 = Action_TwoStraight + 4;
constexpr int Action_TwoStraight_7 = Action_TwoStraight + 5;
constexpr int Action_TwoStraight_6 = Action_TwoStraight + 6;
constexpr int Action_TwoStraight_5 = Action_TwoStraight + 7;
constexpr int Action_TwoStraight_4 = Action_TwoStraight + 8;
constexpr int Action_TwoStraight_3 = Action_TwoStraight + 9;

constexpr int Action_ThreeStraight = Action_TwoStraight + 52;
constexpr int Action_ThreeStraight_5 = Action_ThreeStraight;
constexpr int Action_ThreeStraight_4 = Action_ThreeStraight + 8;
constexpr int Action_ThreeStraight_3 = Action_ThreeStraight + 9;
constexpr int Action_ThreeStraight_2 = Action_ThreeStraight + 10;

constexpr int Action_ThreeAddSingle = Action_ThreeStraight + 38; //三带单：13 * C(14,1) = 13 * 14 = 182
constexpr int Action_ThreeAddPair = Action_ThreeStraight + 182;  //三带对：13 * C(12,1) = 13 * 12 = 156

//带牌2单，可以1对代替2单:[1，1],[2]
constexpr int Action_FourAddSingle = Action_ThreeAddPair + 156; // 四带单：13 * (C(14,2) + C(12,1)) = 13 * (91+12) = 1339
//炸弹可以拆为2对:[2，2]，[4]
constexpr int Action_FourAddPair = Action_FourAddSingle + 1339; //  四带对：13 * C(12,2) = 13 * 66 = 858

constexpr int Action_ThreeStraightAddSingle = Action_FourAddPair + 858; //8044
//5连带5单，5单组合：[1,1,1,1,1],[1,1,1,2],[1,1,3],[1,4],[2,3]
constexpr int Action_ThreeStraightAddSingle_5 = Action_ThreeStraightAddSingle; //8*C(10,5) = 8 * 252 = 2016
//4连带4单，4单组合：[1,1,1,1],[1,1,2],[1,3],[2,2],[4]
constexpr int Action_ThreeStraightAddSingle_4 = Action_ThreeStraightAddSingle + 2016; //9 *C(11,4) = 9 * 330 =  2970
//3连带3单，3单组合：[1,1,1],[1,2],[3]
constexpr int Action_ThreeStraightAddSingle_3 = Action_ThreeStraightAddSingle + 2970; // 10 *C(12,3) = 10 * 220 = 2200
//带牌2单，可以1对代替2单[1,1],[2]
constexpr int Action_ThreeStraightAddSingle_2 = Action_ThreeStraightAddSingle + 2200; // 11 *C(13.2) = 11* 78 =  858

constexpr int Action_ThreeStraightAddPair = Action_ThreeStraightAddSingle_2 + 858; // 3387
//5连带5对，5对组合：[2,2,2,2,2],[2,2,2,4],[2,4,4]
constexpr int Action_ThreeStraightAddPair_5 = Action_ThreeStraightAddPair; // 8*C(8,5) = 8 *  56 = 448
//4连带4对，4对组合：[2,2,2,2],[2,2,4],[4,4]
constexpr int Action_ThreeStraightAddPair_4 = Action_ThreeStraightAddPair + 858; // 9 * (9,4) = 9 * 126 = 1134
//3连带3对，3对组合：[2,2,2],[2,4]
constexpr int Action_ThreeStraightAddPair_3 = Action_ThreeStraightAddPair + 858; // 10* C(10,3) = 10 * 120 = 1200
//2连带2对，2对组合：[2,2],[4]
constexpr int Action_ThreeStraightAddPair_2 = Action_ThreeStraightAddPair + 858; // 11*C(11.2) = 11 * 55 = 605

int Move2Action(RankMove &move);
RankMove action2Move(int action);

constexpr int ACTION4_TYPE_FLAG_LOC = 0;
constexpr int ACTION4_TYPE_FLAG = 0x0F;
constexpr int RANK4_FLAG_LEN = 4;
constexpr int RANK4_FLAG = 0x0F;
constexpr int ACTION4_START_FLAG_LOC = 4;
constexpr int ACTION4_END_FLAG_LOC = 8;
constexpr int ACTION4_ADD_FLAG_LOC = 12;

inline int move2Action4(RankMove &move)
{
    int action = move.Type();
    action = action << ACTION4_TYPE_FLAG_LOC;
    if (move.StartRank() > 0)
    {
        action |= move.StartRank() << ACTION4_START_FLAG_LOC;
    }

    if (move.EndRank() > 0)
    {
        action |= move.EndRank() << ACTION4_END_FLAG_LOC;
    }

    for (int i = 0; i < move.AddedRanks().size(); i++)
    {
        auto add = move.AddedRanks()[i];
        if (add > 0)
        {
            action |= move.AddedRanks()[i] << ACTION4_ADD_FLAG_LOC + i * RANK4_FLAG_LEN;
        }
    }
    return action;
}

inline int decode4(int action, int offset, int flag)
{
    int value = flag << offset;
    value = action & value;
    value = value >> offset;
    return value;
}

inline LandlordMoveType decodeActionType4(int action)
{
    LandlordMoveType type;
    int value = decode4(action, ACTION4_TYPE_FLAG_LOC, ACTION4_TYPE_FLAG);
    type = (LandlordMoveType)value;
    return type;
}

inline RankMove decodeAdded(LandlordMoveType type,
                            int start, int end, int len, int action)
{
    RankMove move(kInvalid, 0);
    std::vector<int> addedRanks;
    if (len > 0)
    {
        for (int i = 0; i < len; i++)
        {
            int rank = decode4(action, ACTION4_ADD_FLAG_LOC + i * RANK4_FLAG_LEN, RANK4_FLAG);
            addedRanks.push_back(rank);
        }
        move = RankMove(type, start, end, addedRanks);
    }
    return move;
}
inline RankMove action2Move4(int action)
{
    RankMove move(kInvalid, 0);
    if (action == 0)
    {
        move = RankMove(kPass, 0);
    }
    else
    {
        LandlordMoveType type = decodeActionType4(action);
        int start = decode4(action, ACTION4_START_FLAG_LOC, RANK4_FLAG);
        int end, len;
        switch (type)
        {
        case kSingle /* 单 */:
        case kPair /* 对 */:
        case LandlordMoveType::kThree:
        case kBomb: // 炸弹，包括癞子炸弹，需要结合牌值编码比较大小。
            move = RankMove(type, start);
            break;
        case kKingBomb:      // 王炸，双王，多数玩法都是最大牌了。
        case kStraight:      //顺子
        case kTwoStraight:   //拖拉机，连对
        case kThreeStraight: //飞机，没翅膀，3连牌
            end = decode4(action, ACTION4_END_FLAG_LOC, RANK4_FLAG);
            move = RankMove(type, start, end);
            break;
        case kThreeAddSingle:       //三带一
        case kThreeAddPair:         //三带对
        case kThreeStraightAddOne:  // 飞机带单翅膀
        case kThreeStraightAddPair: // 飞机带双翅膀
            end = decode4(action, ACTION4_END_FLAG_LOC, RANK4_FLAG);
            len = end - start + 1;
            move = decodeAdded(type,start,end,len,action);
            break;
        case kFourAddTwoSingles: //四带两单牌
        case kFourAddTwoPairs:   //四带两对牌
            end = decode4(action, ACTION4_END_FLAG_LOC, RANK4_FLAG);
            len = 2;
            move = decodeAdded(type,start,end,len,action);
        default:
            break;
        }
    }
    return move;
}

inline std::vector<std::array<int,16>> parseHandCombines(int counts){
    std::vector<std::array<int,16>> handCombines;
    int c1=5;
    int c2=2;
    for (int i1=0;i1<c1;i1++){
        for (int i2=0;i2<c1;i2++){
            for (int i3=0;i3<c1;i3++){
                for (int i4=0;i4<c1;i4++){
                    for (int i5=0;i5<c1;i5++){
                        for (int i6=0;i6<c1;i6++){
                            for (int i7=0;i7<c1;i7++){
                                for (int i8=0;i8<c1;i8++){
                                    for (int i9=0;i9<c1;i9++){
                                        for (int i10=0;i10<c1;i10++){
                                            for (int i11=0;i11<c1;i11++){
                                                for (int i12=0;i12<c1;i12++){
                                                    for (int i13=0;i13<c1;i13++){
                                                        for (int i14=0;i14<c2;i14++){
                                                            for (int i15=0;i15<c2;i15++){
                                                                if (i1+i2+i3+i4+i5+i6+i7+i8+i9+i10+i11+i12+i13+i14 +i15 == counts){
                                                                    handCombines.push_back({i1,i2,i3,i4,i5,i6,i7,i8,i9,i10,i11,i12,i13,i14,i15,0});
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
             }
        }
    }

    return handCombines;
}

inline u_int64_t parseHandCombinesCounts(
            std::vector<std::array<int,16>> firstHandCombines,
            int counts){
    u_int64_t handCombinesCounts = 0;
    int c1=5;
    int c2=2;
    int sum=0;
    for (int i = 0; i < firstHandCombines.size();i++){
        std::cout << i << ",counts:"<< handCombinesCounts<< ",前一手玩家手牌rank：" << rankCountsArray2String(firstHandCombines[i]) << std::endl;
        for (int i1=0;i1<c1-firstHandCombines[i][0];i1++){
            for (int i2=0;i2<c1-firstHandCombines[i][1];i2++){
                for (int i3=0;i3<c1-firstHandCombines[i][2];i3++){
                    for (int i4=0;i4<c1-firstHandCombines[i][3];i4++){
                        for (int i5=0;i5<c1-firstHandCombines[i][4];i5++){
                            for (int i6=0;i6<c1-firstHandCombines[i][5];i6++){
                                for (int i7=0;i7<c1-firstHandCombines[i][6];i7++){
                                    for (int i8=0;i8<c1-firstHandCombines[i][7];i8++){
                                        for (int i9=0;i9<c1-firstHandCombines[i][8];i9++){
                                            for (int i10=0;i10<c1-firstHandCombines[i][9];i10++){
                                                for (int i11=0;i11<c1-firstHandCombines[i][10];i11++){
                                                    for (int i12=0;i12<c1-firstHandCombines[i][11];i12++){
                                                        for (int i13=0;i13<c1-firstHandCombines[i][12];i13++){
                                                            for (int i14=0;i14<c2-firstHandCombines[i][13];i14++){
                                                                for (int i15=0;i15<c2-firstHandCombines[i][14];i15++){
                                                                    if (i1+i2+i3+i4+i5+i6+i7+i8+i9+i10+i11+i12+i13+i14 +i15 == counts){
                                                                        handCombinesCounts++;
                                                                        if (handCombinesCounts < 40){
                                                                            std::cout << handCombinesCounts<<":" << pokers2String({i1,i2,i3,i4,i5,i6,i7,i8,i9,i10,i11,i12,i13,i14,i15}) << std::endl;
                                                                        }
                                                                        if (handCombinesCounts % (10000 * 1000) == 0){
                                                                            std::cout << handCombinesCounts<<":" << pokers2String({i1,i2,i3,i4,i5,i6,i7,i8,i9,i10,i11,i12,i13,i14,i15}) << std::endl;
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return handCombinesCounts;
}

inline u_int64_t parseHandCombinesCounts(int counts){
    u_int64_t handCombinesCounts=0;
    int c1=5;
    int c2=2;
    for (int i1=0;i1<c1;i1++){
        for (int i2=0;i2<c1;i2++){
            for (int i3=0;i3<c1;i3++){
                for (int i4=0;i4<c1;i4++){
                    for (int i5=0;i5<c1;i5++){
                        for (int i6=0;i6<c1;i6++){
                            for (int i7=0;i7<c1;i7++){
                                for (int i8=0;i8<c1;i8++){
                                    for (int i9=0;i9<c1;i9++){
                                        for (int i10=0;i10<c1;i10++){
                                            for (int i11=0;i11<c1;i11++){
                                                for (int i12=0;i12<c1;i12++){
                                                    for (int i13=0;i13<c1;i13++){
                                                        for (int i14=0;i14<c2;i14++){
                                                            for (int i15=0;i15<c2;i15++){
                                                                if (i1+i2+i3+i4+i5+i6+i7+i8+i9+i10+i11+i12+i13+i14 +i15 == counts){
                                                                    handCombinesCounts++;
                                                                    if (handCombinesCounts < 40){
                                                                            std::cout << handCombinesCounts<<":" << pokers2String({i1,i2,i3,i4,i5,i6,i7,i8,i9,i10,i11,i12,i13,i14,i15}) << std::endl;
                                                                    }
                                                                    if (handCombinesCounts % (10000*1000) == 0){
                                                                            std::cout << handCombinesCounts<<":" << pokers2String({i1,i2,i3,i4,i5,i6,i7,i8,i9,i10,i11,i12,i13,i14,i15}) << std::endl;
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
             }
        }
    }

    return handCombinesCounts;
}


} // namespace landlord_learning_env
#endif // ACTION
