#include <iostream>
#include <assert.h>
#include <chrono>
#include <random>

#include "landlord_deck.h"
namespace landlord_learning_env {
    void deck_test1(){
        Deck deck1;
        std::random_device rd;  //获取随机数种子
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        deck1.Shuffle(&gen);
        std::cout << deck1.HandString(0,deck1.NumCards()) << std::endl;
        assert(deck1.NumCards()== kNumCards);

        auto deals = deck1.Deal(0,17);
        assert(deals.size() == 17);
        std::string dealsStr = cards2String(pokers2Cards(deals));
        std::cout << dealsStr << std::endl;
        assert(deck1.HandString(0,deck1.NumCards()).find(dealsStr) != std::string::npos);
    }
     void deck_test2(){
        Deck deck2(kNumCardsIncludeFlower);
        std::random_device rd;  //获取随机数种子
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        deck2.Shuffle(&gen);
        std::cout << deck2.HandString(0,deck2.NumCards()) << std::endl;
        assert(deck2.NumCards()== kNumCardsIncludeFlower);

        for (int i = 0; i < 3; i++){
            auto deals = deck2.Deal(i*17,(i+1)*17);
            assert(deals.size() == 17);
            std::string dealsStr = cards2String(pokers2Cards(deals));
            std::cout << dealsStr << std::endl;
            assert(deck2.HandString(0,deck2.NumCards()).find(dealsStr) != std::string::npos);
        }

        auto remaidPokers = deck2.Deal(51,55);
        std::string dealsStr = cards2String(pokers2Cards(remaidPokers));
        std::cout << dealsStr << std::endl;
        assert(deck2.HandString(0,deck2.NumCards()).find(dealsStr) != std::string::npos);
    }
}

int main(int argc, char** argv) {
    landlord_learning_env::deck_test1();
    landlord_learning_env::deck_test2();
}