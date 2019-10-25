// Copyright 2018 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software,
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <iostream>
#include <assert.h>
#include <chrono>
#include <random>

#include "landlord_hand.h"
namespace landlord_learning_env {
   void hand_test1(){
      LandlordHand hand;
      assert(hand.Cards().size()==0);

      for (int i =0; i< kNumCardsIncludeFlower;i++){
          //deck.push_back(i);
          hand.AddCard(poker2Card(i));
      }
      std::string deckString = "D3 D4 D5 D6 D7 D8 D9 DT DJ DQ DK DA D2 " \
              "C3 C4 C5 C6 C7 C8 C9 CT CJ CQ CK CA C2 " \
              "H3 H4 H5 H6 H7 H8 H9 HT HJ HQ HK HA H2 " \
              "S3 S4 S5 S6 S7 S8 S9 ST SJ SQ SK SA S2 m M F";

      std::cout << "deckString:" << deckString  << std::endl;
      std::cout << "init deck__:"  << hand.ToString() << std::endl;
      int tmp = hand.ToString().compare(deckString);
      std::cout << " hand.ToString().compare(deckString) = " << tmp << std::endl;
      std::cout << " hand.ToString() == deckString = " << 
            (hand.ToString() == deckString) << std::endl;

      assert(!hand.ToString().compare(deckString));
      assert(hand.ToString() == deckString);
   }

    void hand_test2(){
      LandlordHand hand;
      assert(hand.Cards().size()==0);

      std::vector<Poker> pokers = {33,53,13,41,43,36,9};
      for (auto poker : pokers){
         hand.AddCard(poker2Card(poker));
      }
      std::cout << "hand pokers:" << hand.ToString() << std::endl;

      unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	   std::mt19937 rand_num(seed);  // 大随机数
	   
      std::vector<Poker> tmp =  pokers;
      std::vector<LandlordCard> discard_piles;
      while(tmp.size()>0){
         std::uniform_int_distribution<long long> dist(0, tmp.size()-1);  // 给定范围
         int loc = dist(rand_num);
         Poker poker = tmp[loc];
         tmp.erase(tmp.begin()+loc);
         std::cout << "delete loc:" << loc << "," << card2String(poker2Card(poker)) << ",tmp pokers:"  << 
         (tmp.size()>0?cards2String(pokers2Cards(tmp)):"") << std::endl;
         hand.RemoveFromHand(poker2Card(poker),&discard_piles);
         std::cout << "new hands: " << hand.ToString() << std::endl;
         std::cout << "has discarded pokers:"  << cards2String(discard_piles) << std::endl;
      }
       
    }
}

int main(int argc, char** argv) {
   landlord_learning_env::hand_test1();
    landlord_learning_env::hand_test2();
}

