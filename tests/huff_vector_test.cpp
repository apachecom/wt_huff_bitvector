//
// Created by alpachec on 05-06-18.
//

#include <gtest/gtest.h>
#include "../huff_vector.h"




class t_huff_vector:public ::testing::Test {


protected:

    huff_vector<4> B;
    sdsl::bit_vector A;
    virtual void SetUp() {

        A = {1,0,1,0,1,0,0,0,1,0,0,0,1,1,1,1,1,0,0,0,0,0,1,0,1,0,0,1,1,1,0,1};
        B.build(A);
    }

};


TEST_F(t_huff_vector,access){

    for (int i = 0; i < A.size(); ++i) {
        EXPECT_EQ(B[i],A[i]);
    }
}

TEST_F(t_huff_vector,rank){

    std::vector<uint> R;
    R.resize(A.size()+1);
    R[0] = 0;

    for (int i = 0; i < A.size() ; ++i) {
        R[i+1] = R[i]+A[i];
    }

    for (int i = 0; i < A.size() ; ++i) {
        EXPECT_EQ(R[i+1],B.rank(i+1));
    }
    std::cout<<std::endl;

}