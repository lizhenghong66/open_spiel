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

#include "landlord_hand.h"

#include <algorithm>
#include <cassert>

namespace landlord_learning_env {

void LandlordHand::AddCard(LandlordCard card) {
  assert(isValidCard(card));
  cards_.push_back(card);
}

void LandlordHand::RemoveFromHand(LandlordCard card,
                                std::vector<LandlordCard>* discard_pile) {
  if (discard_pile != nullptr) {
    discard_pile->push_back(card);
  }
  std::vector<LandlordCard>::iterator result = find(cards_.begin(),cards_.end(),card);
  cards_.erase(result);
}

std::string LandlordHand::ToString() const {
  std::string result = cards2String(cards_);
  return result;
}

}  // namespace landlord_learning_env
