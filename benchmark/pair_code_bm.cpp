//
// Created by alpachec on 03-09-18.
//


#include <iostream>
#include <fstream>
#include <string>
#include <benchmark/benchmark.h>
#include "../huff_vector.h"

using namespace std;
using namespace sdsl;

#ifndef BLOCK_SIZE
#define BLOCK_SIZE 31
#endif

#ifndef RANK_SAMPLE_DENS
#define RANK_SAMPLE_DENS 32
#endif

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;


static void encode_decode(benchmark::State& state)
{
    size_t points = state.range(0);
    double pb = state.range(1);
    for (auto _ : state)
    {
        srand (time(NULL));
        pair_coder<64>coder;
        // encode;
        unsigned long enc_x64 = 0, enc = 0;
        for (int i = 0; i < points ; ++i) {

            uint64_t x = rand() % 255;
            auto start_x64 = timer::now();
            coder.encode(x);
            auto end_x64 = timer::now();

            sdsl::bit_vector B(64);

            sdsl::util::set_to_value(B,x);

            auto start = timer::now();
            coder.encode(B.begin(),B.end());
            auto end  = timer::now();


            enc_x64 += duration_cast<nanoseconds>(end_x64 - start_x64).count();
            enc   += duration_cast<nanoseconds>(end - start).count();

        }

        std::cout<<"encode x64\t"<< enc_x64*1.0/points<<std::endl;
        std::cout<<"encode    \t"<< enc*1.0/points<<std::endl;


        // decode

        unsigned long dec_x64 = 0, dec = 0;
        for (int i = 0; i < points ; ++i) {

            uint64_t x = rand() % 255;
            auto px64 = coder.encode(x);

            auto start_x64 = timer::now();
            coder.decode_int(px64.first,px64.second);
            auto end_x64 = timer::now();

            sdsl::bit_vector B(64);

            sdsl::util::set_to_value(B,x);
            auto p = coder.encode(B.begin(),B.end());

            auto start = timer::now();
            coder.decode(p.first,p.second);
            auto end  = timer::now();


            dec_x64 += duration_cast<nanoseconds>(end_x64 - start_x64).count();
            dec   += duration_cast<nanoseconds>(end - start).count();

        }

        std::cout<<"decode x64\t"<< dec_x64/points<<std::endl;
        std::cout<<"decode    \t"<< dec/points<<std::endl;

    }
}

BENCHMARK(encode_decode)->Args({6500000})->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();