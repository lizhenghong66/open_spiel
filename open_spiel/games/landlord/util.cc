#include "util.h"

namespace landlord_learning_env {
        //深度优先查找可能的组合
    void dfs(int i,int n,int k,std::vector<std::vector<int>> &result,std::vector<int> &v){
        while(i < n){
            v[v.size()-k] = i++;
            if (k>1){
                dfs(i,n,k-1,result,v);
            }else{
                result.push_back(v);
            }
        }
    }

    //一个简单的组合算法
    //从n中选取k数字的所有组合
   std::vector<std::vector<int>>  combine(int n,int k){
        std::vector<std::vector<int>> result;
        std::vector<int> v(k);
        dfs(0,n,k,result,v);
        return result;
    }
}//landlord_learning_env