//
// Created by alpachec on 04-06-18.
//


#include <gtest/gtest.h>
#include <fstream>
#include <wt_huff.hpp>
#include <vlc_vector.hpp>






TEST(CDSProject,sdsl_utils){


    uint64_t x = 0;

    std::cout<< std::endl << (x | sdsl::bits::lo_set[2]) <<std::endl;

};
