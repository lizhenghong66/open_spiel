// Copyright 2018 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __LANDLORD_HAND_H__
#define __LANDLORD_HAND_H__

#include <cstdint>
#include <string>
#include <vector>

#include "landlord_card.h"

namespace landlord_learning_env {
class LandlordHand {
 public:
  LandlordHand() {}
  // Copy hand. 
  LandlordHand(const LandlordHand& hand): 
      cards_(hand.cards_) {}
      
  // Cards and corresponding card knowledge are always arranged from oldest to
  // newest, with the oldest card or knowledge at index 0.
  const std::vector<LandlordCard>& Cards() const { return cards_; }
  
  void AddCard(LandlordCard card);
  void AddCards(std::vector<LandlordCard> cards);

  // Remove  card from hand. Put in discard_pile if not nullptr
  // (pushes the card to the back of the discard_pile vector).
  void RemoveFromHand(LandlordCard card, std::vector<LandlordCard>* discard_pile);
  
  std::string ToString() const;

 private:
  // A set of cards.
  std::vector<LandlordCard> cards_;
};

}  // namespace landlord_learning_env

#endif
