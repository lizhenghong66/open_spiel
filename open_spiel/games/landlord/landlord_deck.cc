#include "landlord_deck.h"

#include <algorithm>
#include <cassert>

namespace landlord_learning_env {
    std::string Deck::HandString(int begin, int end) const{
        std::vector<int> deals (cards_.begin()+begin,cards_.begin()+end);
        return cards2String(pokers2Cards(deals));
    }
    std::vector<int> Deck::Deal(int begin, int end) const{
        std::vector<int> deals (cards_.begin()+begin,cards_.begin()+end);
        return deals;
    }
 }  // namespace landlord_learning_env