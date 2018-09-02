//
// Created by alpachec on 04-06-18.
//

#include <gtest/gtest.h>
#include <fstream>
#include <wavelet_trees.hpp>

class t_wt_huff:public ::testing::Test {


protected:

    sdsl::wt_blcd < sdsl::rrr_vector<> > W;

    void SetUp() override {
        sdsl::construct_im(W,"tobeornottobethatisthequestion",1);
    }

};

TEST_F(t_wt_huff,rank){

    std::cout<<std::endl;

    std::cout<<"RANK FOR e"<<std::endl;

    for (int i = 0; i < W.size(); ++i) {
        std::cout<<W.rank(i,'e')<<" ";
    }

    std::cout<<std::endl;

}
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