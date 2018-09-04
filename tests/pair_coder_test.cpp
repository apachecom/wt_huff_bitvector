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
    uint p1 = 0, p2 = 1;
    sdsl::bit_vector p = coder.decode(p1,p2);
    EXPECT_EQ(p,B);


    // C1

    B = {0,0,0,1};
    p1 = 1; p2 = 1;
    p = coder.decode(p1,p2);
    EXPECT_EQ(p,B);

    B = {0,0,1,0};
    p1 = 1; p2 = 2;
    p = coder.decode(p1,p2);
    EXPECT_EQ(p,B);


    B = {0,1,0,0};
    p1 = 1; p2 = 3;
    p = coder.decode(p1,p2);
    EXPECT_EQ(p,B);


    B = {1,0,0,0};
    p1 = 1; p2 = 4;
    p = coder.decode(p1,p2);
    EXPECT_EQ(p,B);


    // C2

    B = {0,0,1,1};
    p1 = 2; p2 = 1;
    p = coder.decode(p1,p2);
    EXPECT_EQ(p,B);

    B = {0,1,0,1};
    p1 = 2; p2 = 2;
    p = coder.decode(p1,p2);
    EXPECT_EQ(p,B);


    B = {0,1,1,0};
    p1 = 2; p2 = 3;
    p = coder.decode(p1,p2);
    EXPECT_EQ(p,B);


    B = {1,0,0,1};
    p1 = 2; p2 = 4;
    p = coder.decode(p1,p2);
    EXPECT_EQ(p,B);


    B = {1,0,1,0};
    p1 = 2; p2 = 5;
    p = coder.decode(p1,p2);
    EXPECT_EQ(p,B);


    B = {1,1,0,0};
    p1 = 2; p2 = 6;
    p = coder.decode(p1,p2);
    EXPECT_EQ(p,B);

    //C3


    B = {0,1,1,1};
    p1 = 3; p2 = 1;
    p = coder.decode(p1,p2);
    EXPECT_EQ(p,B);


    B = {1,0,1,1};
    p1 = 3; p2 = 2;
    p = coder.decode(p1,p2);
    EXPECT_EQ(p,B);


    B = {1,1,0,1};
    p1 = 3; p2 = 3;
    p = coder.decode(p1,p2);
    EXPECT_EQ(p,B);


    B = {1,1,1,0};
    p1 = 3; p2 = 4;
    p = coder.decode(p1,p2);
    EXPECT_EQ(p,B);

    //C4


    B = {1,1,1,1};
    p1 = 4; p2 = 4;
    p = coder.decode(p1,p2);
    EXPECT_EQ(p,B);



}

TEST_F(t_pair_coder,encode_int)
{
    uint64_t x = 13;
    auto ppp = std::make_pair<uint,uint >(3,2);
    auto pp = coder.encode(x);
    EXPECT_EQ(pp,ppp);

    x = 11;
    ppp = std::make_pair<uint,uint >(3,3);
    pp = coder.encode(x);
    EXPECT_EQ(pp,ppp);

    x = 7;
    ppp = std::make_pair<uint,uint >(3,4);
    pp = coder.encode(x);
    EXPECT_EQ(pp,ppp);

}

TEST_F(t_pair_coder,decode_int)
{
    uint64_t x = 13;
    auto ppp = std::make_pair<uint,uint >(3,2);
    auto pp = coder.decode_int(ppp.first,ppp.second);
    EXPECT_EQ(pp,x);

    x = 11;
    ppp = std::make_pair<uint,uint >(3,3);
    pp = coder.decode_int(ppp.first,ppp.second);
    EXPECT_EQ(pp,x);

    x = 7;
    ppp = std::make_pair<uint,uint >(3,4);
    pp = coder.decode_int(ppp.first,ppp.second);
    EXPECT_EQ(pp,x);

}