//
// Created by alpachec on 04-06-18.
//

#include <gtest/gtest.h>
#include <fstream>
#include <sdsl/wavelet_trees.hpp>
#include "../utils/pair_coder.h"

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

class t_wt_huff:public ::testing::Test {


protected:

    sdsl::wt_huff<>  W;

    void SetUp() override {
        /*srand (time(NULL));
        size_t p = rand() % W.size();
        std::string s;
        std::string alp = "abcdefghijklmnopqrstuvwxyz*+-";

        for (int i = 0; i < 10000000; ++i)
        {
            s+= alp[rand() % alp.length()];
        }*/

        sdsl::construct(W,"seq620000000",1);
        std::cout<<"W size "<<W.size()<<std::endl;
        std::cout<<"W sigma "<<W.sigma<<std::endl;
    }

};
TEST_F(t_wt_huff,dfs){

    srand (time(NULL));
    pair_coder coder(32);

    double mean = 0;
    for (int i = 0; i < 1000000; ++i) {
        auto start = timer::now();
        size_t p = rand() % W.size();
        W.dfs(W.root(),p,[&coder](const uint64_t & node, const uint64_t &  off){


            ///coder.decode_int(node,1);
        });
        auto stop = timer::now();
        mean += (duration_cast<nanoseconds>(stop - start).count())*1.0/1000000;
    }

    std::cout<<"mean dfs: "<<mean<<std::endl;


}

TEST_F(t_wt_huff,rank){

    std::cout<<std::endl;

    std::cout<<"RANK FOR e"<<std::endl;
    double mean = 0;
    for (int i = 0; i < 1000000; ++i) {
        auto start = timer::now();
        size_t p = rand() % W.size();
        W.rank(p,'e');
        auto stop = timer::now();
        mean += (duration_cast<nanoseconds>(stop - start).count())*1.0/1000000;
    }

    std::cout<<"mean rank: "<<mean<<std::endl;

}
/*
TEST_F(t_wt_huff,root){

    std::cout<<std::endl;

    std::cout<<"Root: "<<W.root()<<std::endl;
}

TEST_F(t_wt_huff,sigma){
    std::cout<<std::endl;
    std::cout<<"Sigma: "<<W.sigma<<std::endl;

}

TEST_F(t_wt_huff,is_leaf){
    std::cout<<std::endl;

    for(int i = 0 ; i <= 22; i++){
        std::cout<<i<<" is leaf? "<<W.is_leaf(i)<<std::endl;
    }

}

TEST_F(t_wt_huff,path){

    std::cout<<std::endl;

    auto s = "abehinoqrstu";

    for (int i = 0; i < std::strlen(s); ++i) {
        auto pth  = W.path(s[i]);
        std::cout<<"sym:"<<s[i]<<" len:"<<pth.first<<" path:"<<pth.second<<std::endl;
    }
}


TEST_F(t_wt_huff,bit_vector){

    std::cout<<std::endl;

    std::cout<<"root's bitvector "<<std::endl;

    auto b = W.bit_vec(0);
    for (int i = 0; i < b.size() ; ++i) {
        std::cout<<b[i]<<" ";
    }

    std::cout<<std::endl;

    auto a = W.bv;

    for (int i = 0; i < a.size() ; ++i) {
        std::cout<<a[i]<<" ";
    }
    std::cout<<std::endl;

}

TEST_F(t_wt_huff,expand){


    std::cout<<"Expand root"<<std::endl;

    std::cout<<W.expand(W.root())[0]<<" "<<W.expand(W.root())[1];

    std::cout<<std::endl;

}
 */