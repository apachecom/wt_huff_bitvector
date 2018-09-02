//
// Created by alpachec on 05-06-18.
//


#include <gtest/gtest.h>
#include "../utils/pair_coder.h"

class t_pair_coder:public ::testing::Test {


protected:

    pair_coder<4> coder;
    t_pair_coder() = default;


    void SetUp() override {};

};


TEST_F(t_pair_coder,encode){

    //C1

    sdsl::bit_vector  B = {0,0,0,1};
    auto p = coder.encode(B);
    EXPECT_EQ(p.first,1);
    EXPECT_EQ(p.second,1);

    B = {0,0,1,0};
    p = coder.encode(B);
    EXPECT_EQ(p.first,1);
    EXPECT_EQ(p.second,2);

    B = {0,1,0,0};
    p = coder.encode(B);
    EXPECT_EQ(p.first,1);
    EXPECT_EQ(p.second,3);

    B = {1,0,0,0};
    p = coder.encode(B);
    EXPECT_EQ(p.first,1);
    EXPECT_EQ(p.second,4);



    //C2

    B = {0,0,1,1};
    p = coder.encode(B);
    EXPECT_EQ(p.first,2);
    EXPECT_EQ(p.second,1);

    B = {0,1,0,1};
    p = coder.encode(B);
    EXPECT_EQ(p.first,2);
    EXPECT_EQ(p.second,2);

    B = {0,1,1,0};
    p = coder.encode(B);
    EXPECT_EQ(p.first,2);
    EXPECT_EQ(p.second,3);

    B = {1,0,0,1};
    p = coder.encode(B);
    EXPECT_EQ(p.first,2);
    EXPECT_EQ(p.second,4);

    B = {1,0,1,0};
    p = coder.encode(B);
    EXPECT_EQ(p.first,2);
    EXPECT_EQ(p.second,5);

    B = {1,1,0,0};
    p = coder.encode(B);
    EXPECT_EQ(p.first,2);
    EXPECT_EQ(p.second,6);



    //C3

    B = {0,1,1,1};
    p = coder.encode(B);
    EXPECT_EQ(p.first,3);
    EXPECT_EQ(p.second,1);

    B = {1,0,1,1};
    p = coder.encode(B);
    EXPECT_EQ(p.first,3);
    EXPECT_EQ(p.second,2);

    B = {1,1,0,1};
    p = coder.encode(B);
    EXPECT_EQ(p.first,3);
    EXPECT_EQ(p.second,3);

    B = {1,1,1,0};
    p = coder.encode(B);
    EXPECT_EQ(p.first,3);
    EXPECT_EQ(p.second,4);




    //C4

    B = {1,1,1,1};
    p = coder.encode(B);
    EXPECT_EQ(p.first,4);
    EXPECT_EQ(p.second,1);


    //C0 Special case

    B = {0,0,0,0};
    p = coder.encode(B);
    EXPECT_EQ(p.first,0);
    EXPECT_EQ(p.second,1);

}

TEST_F(t_pair_coder,decode){

    sdsl::bit_vector  B = {0,0,0,0};
    auto p = coder.decode(0,1);
    EXPECT_EQ(p,B);


    // C1

    B = {0,0,0,1};
    p = coder.decode(1,1);
    EXPECT_EQ(p,B);

    B = {0,0,1,0};
    p = coder.decode(1,2);
    EXPECT_EQ(p,B);


    B = {0,1,0,0};
    p = coder.decode(1,3);
    EXPECT_EQ(p,B);


    B = {1,0,0,0};
    p = coder.decode(1,4);
    EXPECT_EQ(p,B);


    // C2

    B = {0,0,1,1};
    p = coder.decode(2,1);
    EXPECT_EQ(p,B);

    B = {0,1,0,1};
    p = coder.decode(2,2);
    EXPECT_EQ(p,B);


    B = {0,1,1,0};
    p = coder.decode(2,3);
    EXPECT_EQ(p,B);


    B = {1,0,0,1};
    p = coder.decode(2,4);
    EXPECT_EQ(p,B);


    B = {1,0,1,0};
    p = coder.decode(2,5);
    EXPECT_EQ(p,B);


    B = {1,1,0,0};
    p = coder.decode(2,6);
    EXPECT_EQ(p,B);

    //C3


    B = {0,1,1,1};
    p = coder.decode(3,1);
    EXPECT_EQ(p,B);


    B = {1,0,1,1};
    p = coder.decode(3,2);
    EXPECT_EQ(p,B);


    B = {1,1,0,1};
    p = coder.decode(3,3);
    EXPECT_EQ(p,B);


    B = {1,1,1,0};
    p = coder.decode(3,4);
    EXPECT_EQ(p,B);

    //C4


    B = {1,1,1,1};
    p = coder.decode(4,1);
    EXPECT_EQ(p,B);



}