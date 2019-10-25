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

#include "landlord_card.h"
namespace landlord_learning_env {
   void card_test1(){
      std::vector<Poker> pokers = {33,53,13,41,43,36,9};
      std::vector<LandlordCard> cards = pokers2Cards(pokers);
      std::cout << " pokers: " << pokers2String(pokers) <<  std::endl;
      std::cout << " cards: " << cards2String(cards) <<  std::endl;
      std::vector<Poker> pokersFromCards = cards2Pokers(cards);
      std::cout << " pokers: " << pokers2String(pokersFromCards) <<  std::endl;

   }
}
int main(int argc, char** argv) {
   std::string cardStr = landlord_learning_env::card2String(
      landlord_learning_env::poker2Card(54));
   std::cout << "poker 54  = ["  << cardStr << "]" << std::endl;
   assert(cardStr=="F");

   cardStr = landlord_learning_env::card2String(
      landlord_learning_env::poker2Card(0));
   std::cout << "poker 0  = ["  << cardStr  << "]" << std::endl;
   assert(cardStr =="D3");
   
   cardStr = landlord_learning_env::card2String(
      landlord_learning_env::poker2Card(11));
   std::cout << "poker 11  = ["  << cardStr << "]" << std::endl;
   assert(cardStr =="DA");

   cardStr = landlord_learning_env::card2String(
      landlord_learning_env::poker2Card(12));
   std::cout << "poker 12  = ["  << cardStr << "]" << std::endl;
   assert(cardStr =="D2");

   cardStr = landlord_learning_env::card2String(
      landlord_learning_env::poker2Card(13));
   std::cout << "poker 13  = ["  << cardStr << "]" << std::endl;
   assert(cardStr=="C3");
   
    cardStr = landlord_learning_env::card2String(
      landlord_learning_env::poker2Card(52));
   std::cout << "poker 52  = ["  << cardStr << "]" << std::endl;
   assert(cardStr =="m");

   cardStr = landlord_learning_env::card2String(
      landlord_learning_env::poker2Card(53));
   std::cout << "poker 53  = ["  << cardStr << "]" << std::endl;
   assert(cardStr=="M");
   
   landlord_learning_env::card_test1();
}
