/*
 * @Author: your name
 * @Date: 2020-02-10 21:08:23
 * @LastEditTime: 2020-02-10 21:13:15
 * @LastEditors: your name
 * @Description: In User Settings Edit
 * @FilePath: /open_spiel/open_spiel/games/landlord/landlord_move_test.cc
 */
#include <iostream>
#include <assert.h>
#include <chrono>
#include <random>

#include "landlord_move.h"
namespace landlord_learning_env {
   void move_test1(){
      RankMove move(kStraight,0,7);
      std::cout << move.toString() << std::endl << move.toArrayString() << std::endl;
      assert(move.toArrayString()=="[3,4,5,6,7,8,9,T]");
   }

    
}

int main(int argc, char** argv) {
   landlord_learning_env::move_test1();
}