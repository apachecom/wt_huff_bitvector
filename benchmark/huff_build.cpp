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
///////////////////// 0     1      2     3     4    5    6    7    8    9    10   11   12
double density[] = {  0.01, 0.02,  0.04, 0.08, 0.1, 0.2, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 0.99};
std::fstream hf_size;
std::fstream rf_size;
std::fstream sf_size;



void create_input_test_files(benchmark::State& state)
{
    size_t points = state.range(0);
    uint8_t d = state.range(1);
    sf_size.open("sf_size",std::ios::out|std::ios::app);
    rf_size.open("rf_size",std::ios::out|std::ios::app);
    for (auto _ : state)
    {
        srand (time(NULL));
        std::fstream f("b_"+std::to_string(points)+"(density "+std::to_string(density[d])+")",std::ios::out|std::ios::binary);
        sdsl::bit_vector B(points,0);
        for (int i = 0; i < points; ++i)
        {
            bool v =  ((rand() % 100 + 1) * 1.0 / 100) < density[d];
            B[i] = v;
        }
        sdsl::serialize(B,f);

        sdsl::rrr_vector<> rrr_v(B);
        sdsl::sd_vector<> sd_v(B);

        std::fstream f_sd("b_"+std::to_string(points)+"(density "+std::to_string(density[d])+")sd_vector",std::ios::out|std::ios::binary);
        std::fstream f_rrr("b_"+std::to_string(points)+"(density "+std::to_string(density[d])+")rrr_vector",std::ios::out|std::ios::binary);

        sdsl::serialize(rrr_v,f_rrr);
        sdsl::serialize(sd_v,f_sd);

        sf_size << points <<","<<density[d]<<","<<sdsl::size_in_bytes(sd_v) << "\n";
        rf_size << points <<","<<density[d]<<","<<sdsl::size_in_bytes(rrr_v) << "\n";

    }
    sf_size.close();
    rf_size.close();
}

void create_random_pos(benchmark::State& state)
{
    size_t points = state.range(0);
    for (auto _ : state)
    {
        srand (time(NULL));
        std::fstream f("b_"+std::to_string(points)+"_rand_pos",std::ios::out|std::ios::binary);
        std::vector<uint> X(1000000,0);
        for (int i = 0; i < 1000000; ++i){
            X[i] = rand() % points ;
        }
        sdsl::serialize(X,f);
    }
}

void build_bitvectors(benchmark::State& state)
{
    size_t points = state.range(0);
    uint8_t d = state.range(1);
    uint8_t block_size = state.range(2);

    hf_size.open("hf_size",std::ios::out|std::ios::app);

    std::string collection = "b_"+std::to_string(points)+"(density "+std::to_string(density[d])+")";
    std::fstream f(collection,std::ios::in|std::ios::binary);
    sdsl::bit_vector X(points);
    sdsl::load(X,f);

    std::cout<<"data:          "<<collection<<std::endl;
    std::cout<<"data size:     "<<X.size()<<std::endl;
    std::cout<<"data densityt: "<<density[d]<<std::endl;

    huff_vector hf_v; hf_v.build(block_size,X);
    std::cout<<"huff vector constructor"<<std::endl;
    std::fstream f_huff(collection+"huff_vector(block_size "+std::to_string(block_size)+")",std::ios::out|std::ios::binary);
    hf_v.save(f_huff);

    hf_size << points <<","<<density[d]<<","<< hf_v.size_in_bytes() <<","<<hf_v.sigma()<<","<<hf_v.wt_get_size()<<"\n";
    hf_size.close();

    for (auto _ : state)
    {
        sleep(3);
    }


}



/*BENCHMARK(create_input_test_files)->Args({2000000000,0})->Unit(benchmark::kMillisecond);
BENCHMARK(create_input_test_files)->Args({2000000000,1})->Unit(benchmark::kMillisecond);
BENCHMARK(create_input_test_files)->Args({2000000000,2})->Unit(benchmark::kMillisecond);
BENCHMARK(create_input_test_files)->Args({2000000000,3})->Unit(benchmark::kMillisecond);
BENCHMARK(create_input_test_files)->Args({2000000000,4})->Unit(benchmark::kMillisecond);
BENCHMARK(create_input_test_files)->Args({2000000000,5})->Unit(benchmark::kMillisecond);
BENCHMARK(create_input_test_files)->Args({2000000000,6})->Unit(benchmark::kMillisecond);
BENCHMARK(create_input_test_files)->Args({2000000000,7})->Unit(benchmark::kMillisecond);
BENCHMARK(create_input_test_files)->Args({2000000000,8})->Unit(benchmark::kMillisecond);
BENCHMARK(create_input_test_files)->Args({2000000000,9})->Unit(benchmark::kMillisecond);
BENCHMARK(create_input_test_files)->Args({2000000000,10})->Unit(benchmark::kMillisecond);
BENCHMARK(create_input_test_files)->Args({2000000000,11})->Unit(benchmark::kMillisecond);
BENCHMARK(create_input_test_files)->Args({2000000000,12})->Unit(benchmark::kMillisecond);*/




/*
BENCHMARK(build_bitvectors)->Args({2000000000,0 ,4})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,1 ,4})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,2 ,4})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,3 ,4})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,4 ,4})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,5 ,4})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,6 ,4})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,7 ,4})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,8 ,4})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,9 ,4})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,10,4})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,11,4})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,12,4})->Unit(benchmark::kMillisecond);
*/


BENCHMARK(build_bitvectors)->Args({2000000000,0 ,8})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,1 ,8})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,2 ,8})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,3 ,8})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,4 ,8})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,5 ,8})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,6 ,8})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,7 ,8})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,8 ,8})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,9 ,8})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,10,8})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,11,8})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,12,8})->Unit(benchmark::kMillisecond);


BENCHMARK(build_bitvectors)->Args({2000000000,0 ,16})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,1 ,16})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,2 ,16})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,3 ,16})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,4 ,16})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,5 ,16})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,6 ,16})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,7 ,16})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,8 ,16})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,9 ,16})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,10,16})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,11,16})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,12,16})->Unit(benchmark::kMillisecond);




BENCHMARK(build_bitvectors)->Args({2000000000,0 ,32})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,1 ,32})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,2 ,32})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,3 ,32})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,4 ,32})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,5 ,32})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,6 ,32})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,7 ,32})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,8 ,32})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,9 ,32})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,10,32})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,11,32})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,12,32})->Unit(benchmark::kMillisecond);


BENCHMARK(build_bitvectors)->Args({2000000000,0 ,64})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,1 ,64})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,2 ,64})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,3 ,64})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,4 ,64})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,5 ,64})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,6 ,64})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,7 ,64})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,8 ,64})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,9 ,64})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,10,64})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,11,64})->Unit(benchmark::kMillisecond);
BENCHMARK(build_bitvectors)->Args({2000000000,12,64})->Unit(benchmark::kMillisecond);




BENCHMARK(create_random_pos)->Args({2000000000})->Unit(benchmark::kMillisecond);
BENCHMARK_MAIN();