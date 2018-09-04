//
// Created by alpachec on 13-07-18.
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


static void create_input_test_files(benchmark::State& state)
{
    size_t points = state.range(0);
    double pb = state.range(1);
    for (auto _ : state)
    {
        srand (time(NULL));
        std::fstream bit_vector_f("b_"+std::to_string(points)+"_0.1",std::ios::out|std::ios::binary);
        std::fstream bit_vector_rank_f("b_"+std::to_string(points)+"_rank_0.1",std::ios::out|std::ios::binary);
        unsigned long long S = 0;
        for (int i = 0; i < points; ++i) {
            int v =  ((rand() % 100 + 1) * 1.0 / 100) < 0.1;
            bit_vector_f << v;
            S+=v;
            bit_vector_rank_f << S << std::endl;

        }
    }
}

static void cmp_rrr_vector(benchmark::State& state)
{
    size_t points = state.range(0);
    std::string collection = "b_"+std::to_string(points);
    std::fstream bit_vector_f(collection,std::ios::in|std::ios::binary);

    std::cout<<std::endl<<"data: "<<collection<<std::endl;

    sdsl::bit_vector bit_vector1(points,0);

    sdsl::int_vector<> rank_vector(points,0);
    for (int i = 0; i < bit_vector1.size() ; ++i) {
        unsigned char v;
        bit_vector_f >> v;
        bit_vector1[i] = v - 48;
    }

    huff_vector<32> hf_v; hf_v.build(bit_vector1);
    sdsl::rrr_vector<> rrr_v(bit_vector1);
    sdsl::sd_vector<> sd_v(bit_vector1);
    sdsl::sd_vector<>::rank_1_type sd_rank(&sd_v);
    sdsl::rrr_vector<>::rank_1_type rrr_rank(&rrr_v);

    for (auto _ : state)
    {
        unsigned long long rrr =0,huff =0,sd =0;
        unsigned long long rrr_ac =0,huff_ac =0,sd_ac =0;

        for (int i = 0; i <1000000 ; ++i) {

            size_t v =  (rand() % points);
            auto start = timer::now();
            hf_v.rank(v);
            auto stop = timer::now();

            auto _start = timer::now();
            rrr_rank.rank(v);
            auto _stop = timer::now();

            auto sd_start = timer::now();
            sd_rank.rank(v);
            auto sd_stop = timer::now();


            rrr += duration_cast<nanoseconds>(_stop - _start).count();
            huff   += duration_cast<nanoseconds>(stop - start).count();
            sd   += duration_cast<nanoseconds>(sd_stop - sd_start).count();

            auto start_ac = timer::now();
            hf_v[v];
            auto stop_ac = timer::now();

            auto _start_ac = timer::now();
            rrr_v[v];
            auto _stop_ac = timer::now();

            auto sd_start_ac = timer::now();
            sd_v[v];
            auto sd_stop_ac = timer::now();


            rrr_ac += duration_cast<nanoseconds>(_stop_ac - _start_ac).count();
            huff_ac   += duration_cast<nanoseconds>(stop_ac - start_ac).count();
            sd_ac   += duration_cast<nanoseconds>(sd_stop_ac - sd_start_ac).count();
        }

        cout<<"rank\n";
        cout << "huff_v \t" <<huff/1000000<<" (nanoseconds)"<<std::endl;
        cout << "rrr_v \t" <<rrr/1000000<<" (nanoseconds)"<<std::endl;
        cout << "sd_v \t" <<sd/1000000<<" (nanoseconds)"<<std::endl;


        cout<<"access\n";
        cout << "huff_v \t" <<huff_ac/1000000<<" (nanoseconds)"<<std::endl;
        cout << "rrr_v \t" <<rrr_ac/1000000<<" (nanoseconds)"<<std::endl;
        cout << "sd_v \t" <<sd_ac/1000000<<" (nanoseconds)"<<std::endl;
    }
}



///BENCHMARK(create_input_test_files)->Args({265000000})->Unit(benchmark::kMillisecond);
BENCHMARK(cmp_rrr_vector)->Args({265000000})->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();