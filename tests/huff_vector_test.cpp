//
// Created by alpachec on 05-06-18.
//

#include <gtest/gtest.h>
#include "../huff_vector.h"
#include <ctime>
#include <cstdio>
#include <iostream>

#define MAX_SIZE 10000000;
#define prob 0.5;


using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

class t_huff_vector:public ::testing::Test {


protected:

    huff_vector B;
    sdsl::bit_vector A;
    virtual void SetUp() {

        /* initialize random seed: */
        srand (time(NULL));
        A = sdsl::bit_vector(100000000,0);
        for (int i = 0; i < A.size(); ++i) {
            A[i] = ((rand() % 100 + 1) * 1.0 / 100) < 0.4;
        }

        //A = {1,0,1,0,1,0,0,0,1,0,0,0,1,1,1,1,1,0,0,0,0,0,1,0,1,0,0,1,1,1,0,1};

        B.build(16,A);
        std::ofstream ff("huff_vect",std::ios::out|std::ios::binary);
        B.save(ff);
    }

};


TEST_F(t_huff_vector,access){
    huff_vector X;
    std::fstream ff("huff_vect",std::ios::in|std::ios::binary);
    X.load(ff);
    for (int i = 0; i < A.size(); ++i) {
        EXPECT_EQ(X[i],A[i]);
    }

}

/*TEST_F(t_huff_vector,wt_rank){

    auto wt_size = B.wt_get_size();
    auto wt_sigma = B.wt_get_sigma();
    double t = 0;
    for (int i = 0; i <10 ; ++i) {

        size_t j =  (rand() % wt_size)+1;
        size_t c =  (rand() % wt_sigma);
        auto cc = char(c+48);
        auto start = timer::now();
        auto r1=B.rank_c(j,cc);
        auto stop = timer::now();
        std::cout<<r1<<" ";
        t += duration_cast<nanoseconds>(stop - start).count()*1.0/A.size();

    }

    std::cout<<"rank_wt mean "<<t<<"(ns)"<<std::endl;

}*/



TEST_F(t_huff_vector,rank){


    huff_vector X;
    std::fstream ff("huff_vect",std::ios::in|std::ios::binary);
    X.load(ff);

    std::vector<uint> R;
    R.resize(A.size()+1);
    R[0] = 0;

    double t = 0;

    for (int i = 0; i < A.size() ; ++i) {
        R[i+1] = R[i]+A[i];
    }

    for (int i = 0; i < A.size() ; ++i) {
        auto start = timer::now();
        auto r1 = X.rank(i+1);
        auto stop = timer::now();
        t += duration_cast<nanoseconds>(stop - start).count()*1.0/A.size();
        auto r2 = R[i+1];
        EXPECT_EQ(r2,r1);
    }
    std::cout<<"rank mean "<<t<<"(ns)"<<std::endl;
}

