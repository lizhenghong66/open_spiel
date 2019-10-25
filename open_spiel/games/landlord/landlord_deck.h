#ifndef __LANDLORD_HAND_H__
#define __LANDLORD_HAND_H__

#include <cstdint>
#include <string>
#include <vector>

#include "landlord_card.h"

namespace landlord_learning_env {
    class Deal {
        public:
            Deal() { std::iota(std::begin(cards_), std::end(cards_), 0); }
            void Shuffle(std::mt19937* rng, int begin = 0, int end = kNumCards) {
                for (int i = begin; i < end - 1; ++i) {
                    // We don't use absl::uniform_int_distribution because it behaves
                    // differently in different versions of C++, and we want reproducible
                    // tests.
                    int j = i + (*rng)() % (end - i);
                    std::swap(cards_[i], cards_[j]);
                }
            }
            
            Deal(const std::array<int, kNumCards>& cards) : cards_(cards) {}
            int Card(int i) const { return cards_[i]; }
            int Suit(int i) const { return cards_[i] % kNumSuits; }
            int Rank(int i) const { return cards_[i] / kNumSuits; }
            std::string HandString(int begin, int end) const;

        private:
            std::array<int, kNumCards> cards_;  // 0..12 are West's, then E, N, S
    };
}  // namespace landlord_learning_env

#endif