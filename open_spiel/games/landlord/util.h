#ifndef __LANDLORD_UTIL_H__
#define __LANDLORD_UTIL_H__
#include <vector>

namespace landlord_learning_env {
    //一个简单的组合算法
    //从n中选取k数字的所有组合
   std::vector<std::vector<int>>  combine(int n,int k);

    
}//namespace landlord_learning_env
#endif  //__LANDLORD_UTIL_H__