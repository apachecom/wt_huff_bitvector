//
// Created by alpachec on 10-09-18.
//
#include <iostream>
#include <fstream>
#include <string>
#include <benchmark/benchmark.h>
#include <sdsl/vlc_vector.hpp>
#include <sdsl/enc_vector.hpp>
#include "../huff_vector.h"

using namespace std;
using namespace sdsl;

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;
///////////////////// 0     2   3   4    5   6   7   8   9   10  11  12  13

double density[] = {0.01,0.02,0.04,0.08,0.1,0.2,0.4,0.5,0.6,0.7,0.8,0.9,0.99};
std::vector<uint> X;
uint pos = 0;

huff_vector hf_v;
sdsl::rrr_vector<> rrr_v;
sdsl::sd_vector<> sd_v;

sdsl::sd_vector<>::rank_1_type sd_rank;//O(log n/m)
sdsl::rrr_vector<>::rank_1_type rrr_rank;//O(k)



void perform_rank_hf(benchmark::State& state)
{
    srand (time(NULL));
    for (auto _ : state) {
        hf_v.rank(X[rand()%X.size()]);
    }
}

void perform_rank_rrr(benchmark::State& state)
{
    srand (time(NULL));
    for (auto _ : state) {
        rrr_rank.rank(X[rand()%X.size()]);
    }
}

void perform_rank_sd(benchmark::State& state)
{
    srand (time(NULL));
    for (auto _ : state) {
        sd_rank.rank(X[rand()%X.size()]);
    }
}



void perform_access_hf(benchmark::State& state)
{
    srand (time(NULL));
    for (auto _ : state) {
        hf_v[X[rand()%X.size()]];
    }
}

void perform_access_rrr(benchmark::State& state)
{
    srand (time(NULL));
    for (auto _ : state) {
        rrr_v[X[rand()%X.size()]];
    }
}

void perform_access_sd(benchmark::State& state)
{
    srand (time(NULL));
    for (auto _ : state) {
        sd_v[X[rand()%X.size()]];
    }
}







BENCHMARK(perform_rank_hf)->Unit(benchmark::kNanosecond);
BENCHMARK(perform_rank_rrr)->Unit(benchmark::kNanosecond);
BENCHMARK(perform_rank_sd)->Unit(benchmark::kNanosecond);
BENCHMARK(perform_access_hf)->Unit(benchmark::kNanosecond);
BENCHMARK(perform_access_rrr)->Unit(benchmark::kNanosecond);
BENCHMARK(perform_access_sd)->Unit(benchmark::kNanosecond);


int main (int argc, char *argv[] ){

    if(argc < 3){
        std::cout<<"Error en los par'ametros";
        return 0;
    }
    unsigned int points = std::atoi(argv[1]);
    unsigned int d_p = std::atoi(argv[2]);
    unsigned int block_size = std::atoi(argv[3]);

    std::cout<<"points     "<<points<<std::endl;
    std::cout<<"density    "<<density[d_p]<<std::endl;
    std::cout<<"block_size "<<block_size<<std::endl;

    std::string collection = "b_"+std::to_string(points)+"(density "+std::to_string(density[d_p])+")";

    std::fstream f_huff(collection+"huff_vector(block_size "+std::to_string(block_size)+")",std::ios::in|std::ios::binary);
    std::fstream f_sd(collection+"sd_vector",std::ios::in|std::ios::binary);
    std::fstream f_rrr(collection+"rrr_vector",std::ios::in|std::ios::binary);

    std::fstream f_sd_r(collection+"sd_vector_rank",std::ios::in|std::ios::binary);
    std::fstream f_rrr_r(collection+"rrr_vector_rank",std::ios::in|std::ios::binary);

    hf_v.load(f_huff);
    std::cout<<"huffman bitvector loaded("<<hf_v.sigma()<<")\n";

    sdsl::load(rrr_v,f_rrr);
    rrr_rank = rrr_vector<>::rank_1_type(&rrr_v);
    std::cout<<"rrr bitvector loaded("<<rrr_v.size()<<")\n";

    sdsl::load(sd_v,f_sd);
    sd_rank = sd_vector<>::rank_1_type(&sd_v);
    std::cout<<"sd bitvector loaded("<<sd_v.size()<<")\n";

    std::fstream f("b_"+std::to_string(points)+"_rand_pos",std::ios::in|std::ios::binary);
    sdsl::load(X,f);
    std::cout<<"random pos loaded\n";

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

    return 0;
}