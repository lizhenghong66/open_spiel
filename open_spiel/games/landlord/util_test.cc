#include "util.h"
#include <iostream>
#include <cassert>

void print(std::vector<std::vector<int>> cmbs){
    int count =0;
   for (auto cmb : cmbs){
       std::cout << "count:" << ++count << std::endl;
       for (auto v : cmb){
           std::cout << std::to_string(v) + " ";
       }
       std::cout << std::endl;
   }
}
int main(int argc, char** argv) {
   std::vector<std::vector<int>> cmbs = landlord_learning_env::combine(5,3);
   assert(cmbs.size() == 10);
   assert(cmbs[0].size() == 3);
    print(cmbs);

   cmbs = landlord_learning_env::combine(5,1);
   assert(cmbs.size() == 5);
   assert(cmbs[0].size() == 1);
   print(cmbs);

   cmbs = landlord_learning_env::combine(10,2);
   assert(cmbs.size() == 45);
   assert(cmbs[0].size() == 2);
   print(cmbs);
}
