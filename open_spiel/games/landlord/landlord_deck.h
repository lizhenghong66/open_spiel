#ifndef __LANDLORD_HAND_H__
#define __LANDLORD_HAND_H__

#include <cstdint>
#include <string>
#include <vector>
#include <random>

#include "landlord_card.h"

namespace landlord_learning_env {
    class Deck {
        public:
            Deck() { 
                cards_ .resize(numCards_);
                std::iota(std::begin(cards_), std::end(cards_), 0); 
            }
            Deck(const int numCards) : numCards_(numCards){ 
                cards_ .resize(numCards_);
                std::iota(std::begin(cards_), std::end(cards_), 0); 
            }
            void Shuffle(std::mt19937* rng, int begin = 0) {
                for (int i = begin; i < numCards_ - 1; ++i) {
                    // We don't use absl::uniform_int_distribution because it behaves
                    // differently in different versions of C++, and we want reproducible
                    // tests.
                    int j = i + (*rng)() % (numCards_ - i);
                    std::swap(cards_[i], cards_[j]);
                }
            }
            
            Deck(const  std::vector<int>& cards) : cards_(cards) {numCards_ = cards_.size();}
            int Card(int i) const { return cards_[i]; }
            int Suit(int i) const { return  poker2Card(cards_[i]).first; }
            int Rank(int i) const { return poker2Card(cards_[i]).second; }
            int NumCards() const{ return numCards_;}
            std::string HandString(int begin, int end) const;
            std::vector<int> Deal(int begin, int end) const;
        private:
            int numCards_ = kNumCards;
            std::vector<int> cards_;  // 0..12 are West's, then E, N, S
    };
}  // namespace landlord_learning_env

#endif