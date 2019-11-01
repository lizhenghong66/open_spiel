// Game of landlord:
// https://baike.baidu.com/item/%E6%96%97%E5%9C%B0%E4%B8%BB/177997?fr=aladdin
// 斗地主玩法比较多，可以考虑通过配置选择。
//  这里只考虑3人斗地主
 // Parameters: 
//  玩法选项:
 //1. isAllowAddPair                            true   #是否允许带对（默认允许），个别地方玩法不允许带对。
 //2. isAllowContinuableBomb      fasle  # 是否允许连炸，如  44445555，最大炸弹，结算时算3炸。
 //3. isThreeNoAddMustLastPut    false  # 三不带牌是否必须最后一手出，中途不允许。
 //4. isUsedWildCard                          false  # 是否使用百搭牌（花牌，55张牌）,线下癞子玩法，默认为false。花牌代替任意牌，不能单出。
 //                                                                            # 使用花牌时，容易出现重炸（炸弹+花牌，或普通炸弹+花牌），结算时算2炸。
 //5. isUseLaizi                                      false    # 王以外的牌选一个做癞子，可以 代替王以外的任意牌，单独出时只能是本牌
 //6. isUseTiandiLaizi                        false    # 天地癞子，王以外的牌选两个癞子，可以 代替王以外的任意牌，单独出时只能是本牌                            
 // 洗牌方位：第一次随机选择开始洗牌方位，后面都是上一局地主方位洗牌。
 // 洗牌方式： 1.  每次随机洗牌    
//                         2.  不洗牌，即第一次随机洗牌，后面将玩家剩余手牌（保留顺序）追加到打出的牌中，然后随机倒几次牌。
//                         3.  倒牌参数：最小次数（0），最大次数（0），最小牌数（1），最大牌数（牌张数-1）。                     
 //  开始发牌方位：1.  洗牌方位的下家。   2.  发牌前随机翻一张牌(地主牌))，根据牌面值确定发牌方位。
 //  发牌方式：1.  每次发牌1张   
//                         2.   每次直接发牌17张   
 //                        3.  如有翻地主牌，可分两次发牌，第一次发翻出地主牌面值张数的牌，后面再发剩余的牌。
 //                        4.  如有翻地主牌，先取出地主补充牌，第一次发翻出地主牌面值张数的牌，后面再发剩余的牌。
//  开始叫牌方位：1.  开始发牌方位。   2.  如有翻出的地主牌，则获得地主牌的玩家开始叫牌。
 //  叫牌选项： 1.   典型网络叫牌，1---3分，分最高者当地主。
//                          2.   线下叫法，只有一次机会，叫地主后其他玩家没机会了。
//                          3.   是否允许闷（不给其他玩家看底牌机会），输赢加倍。
//                          4.  是否尾家必闷（允许闷牌的前提下）
//                          5.  是否允许倒牌（加倍玩家输赢加倍，没没到牌玩家不变）
//                          6.  弃庄的农民是否允许跟倒   
//                          7.  庄家（地主）是否允许拉牌（再加倍，只针对参与倒牌玩家）。


#ifndef __LANDLORD_MOVE_H__
#define __LANDLORD_MOVE_H__

#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <map>

#include "landlord_card.h"

namespace landlord_learning_env {
  enum  LandlordMoveType { 
      kPass = 0,                      //过
      kSingle = 1,                  //单 
      kPair = 2,                       //对
      kThree = 3,                   //三不带 （有些地方中途不允许，只能最后一手）
      kThreeAddSingle = 4,                               //三带一
      kThreeAddPair = 5,                                //三带对
      kFourAddTwoSingles = 6,                   //四带两单牌
      kFourAddTwoPairs = 7,                       //四带两对牌
      kStraight = 8,                                           //顺子 
      kTwoStraight = 9,                                 //拖拉机，连对
      kThreeStraight = 10,                             //飞机，没翅膀，3连牌
      kThreeStraightAddOne = 11,             // 飞机带单翅膀
      kThreeStraightAddPair = 12,           // 飞机带双翅膀

      kBomb = 13,           // 炸弹，包括癞子炸弹，需要结合牌值编码比较大小。
      kKingBomb = 14,           // 王炸，双王，多数玩法都是最大牌了。
      kWeightBomb = 15,           // 重炸，炸弹+花牌，允许花牌时最大牌了。
      kKingWeightBomb = 16,           // 重炸，双王+花牌，允许花牌时都是最大牌了。
      kContinueBomb = 17,           // 连炸（航天飞机）。允许连炸时最大牌。航天飞机
      kContinueBombAddOne = 18,           // 航天飞机带单。允许连炸时最大牌。航天飞机
      kContinueBombAddPair = 19,           // 航天飞机带对。允许连炸时最大牌。航天飞机
      kInvalid,    //无效
};

constexpr int MOVE_COUNTS = 21;

// 斗地主出牌方式比较复杂，每种出牌方式的数量也不一样:
class LandlordMove {
  // LandlordMove is small, and intended to be passed by value.
 public:
  LandlordMove(LandlordMoveType move_type, std::vector <Poker>  pokers,int8_t laizi_rank = -1,int8_t tian_laizi_rank = -1);
  // Tests whether two moves are functionally equivalent.
  bool operator==(const LandlordMove& other_move) const;
  std::string ToString() const;

  LandlordMoveType MoveType() const { return move_type_; }
  bool IsValid() const { return move_type_ != kInvalid; }

 private:
  void chkValid();
  LandlordMoveType move_type_ = kInvalid;
  int8_t rank_ = -1; //每种出牌的等级值，顺子取最大值
  int8_t size_ = -1;   //主要记录顺子或天地癞子炸弹时牌数量
  int8_t laizi_rank_ = -1; // 癞子牌或天地癞子时的地癞牌的等级值。
  int8_t tian_laizi_rank_ = -1; // 天地癞子时的天癞牌的等级值。
  std::vector <Poker>  pokers_;
  std::array<RankType,RANK_COUNTS>  rankCounts_ {0}; //每种等级（rank）牌的数量数组。

  //主牌中癞子牌的使用情况（花牌特殊情况不记录）
  int laizhi_count; //癞子或地癞牌数量
  int tian_laizi_count; //天癞牌数量
   
  //主牌带牌时可能带牌的癞子牌数量。
  int added_laizhi_count; //癞子或地癞牌数量
  int added_tian_laizi_count; //天癞牌数量
};

}  // namespace landlord_learning_env

#endif
