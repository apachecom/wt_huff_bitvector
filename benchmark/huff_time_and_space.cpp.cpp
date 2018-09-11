//
// Created by alpachec on 13-07-18.
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

#ifndef BLOCK_SIZE
#define BLOCK_SIZE 31
#endif

#ifndef RANK_SAMPLE_DENS
#define RANK_SAMPLE_DENS 32
#endif

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

/*

double density[]={


};
*/



static void create_input_test_files_sparse(benchmark::State& state)
{
    size_t points = state.range(0);
    double pb = state.range(1);
    for (auto _ : state)
    {
        srand (time(NULL));
        std::fstream bit_vector_f("b_"+std::to_string(points)+"_0.01",std::ios::out|std::ios::binary);

        for (int i = 0; i < points; ++i)
        {
            int v =  ((rand() % 100 + 1) * 1.0 / 100) < 0.1;
            bit_vector_f << v;
        }
    }
}
static void create_input_test_files(benchmark::State& state)
{
    size_t points = state.range(0);
    double pb = state.range(1);
    for (auto _ : state)
    {
        srand (time(NULL));
        std::fstream bit_vector_f("b_"+std::to_string(points),std::ios::out|std::ios::binary);
        std::fstream bit_vector_rank_f("b_"+std::to_string(points),std::ios::out|std::ios::binary);
        unsigned long long S = 0;
        for (int i = 0; i < points; ++i) {
            int v =  ((rand() % 100 + 1) * 1.0 / 100) < 0.5;
            bit_vector_f << v;
            /*S+=v;
            bit_vector_rank_f << S << std::endl;*/

        }
    }
}
void cmp_vector_sparse(benchmark::State& state)
{
    size_t points = state.range(0);
    size_t size_block = state.range(1);
    std::string collection = "b_"+std::to_string(points)+"_0.1";
    std::fstream bit_vector_f(collection,std::ios::in|std::ios::binary);

    std::cout<<std::endl<<"data: "<<collection<<std::endl;

    sdsl::bit_vector bit_vector1(points,0);
    uint ones = 0, zeros =0;
    for (int i = 0; i < bit_vector1.size() ; ++i) {
        unsigned char v;
        bit_vector_f >> v;
        bit_vector1[i] = v - 48;
        bit_vector1[i] == 1 ? ones++:zeros++;
    }

    huff_vector hf_v; hf_v.build(size_block,bit_vector1);
    sdsl::rrr_vector<> rrr_v(bit_vector1);
    sdsl::sd_vector<> sd_v(bit_vector1);

    ////rank
    sdsl::sd_vector<>::rank_1_type sd_rank(&sd_v);//O(log n/m)
    sdsl::rrr_vector<>::rank_1_type rrr_rank(&rrr_v);//O(k)
    sdsl::rank_support_v<> rv(&bit_vector1); //O(1)
    sdsl::rank_support_v5<> rv5(&bit_vector1);//O(1)
    sdsl::rank_support_scan<> rsc(&bit_vector1);//O(n)


    std::cout<<"Space  "<<std::endl;
    std::cout<<"space huff_v  "<<hf_v.size_in_mb()<<"(mb)   overhead space "<<(hf_v.size_in_mb()*1.0)*(8242880.0/points)<<std::endl;
    std::cout<<"space rrr_v   "<<sdsl::size_in_mega_bytes(rrr_v)<<"(mb)   overhead space "<<((sdsl::size_in_bytes(rrr_v))*1.0)*(8.0/points)<<std::endl;
    std::cout<<"space sd_v    "<<sdsl::size_in_mega_bytes(sd_v)<<"(mb)   overhead space "<<((sdsl::size_in_bytes(sd_v))*1.0)*(8.0/points)<<std::endl;
    std::cout<<"space rank_support_v    "<<sdsl::size_in_mega_bytes(rv)+sdsl::size_in_mega_bytes(bit_vector1)<<"(mb)   overhead space "<<((sdsl::size_in_bytes(rv)+sdsl::size_in_bytes(bit_vector1))*1.0)*(8.0/points)<<std::endl;
    std::cout<<"space rank_support_v5    "<<sdsl::size_in_mega_bytes(rv5)+sdsl::size_in_mega_bytes(bit_vector1)<<"(mb)   overhead space "<<((sdsl::size_in_bytes(rv5)+sdsl::size_in_bytes(bit_vector1))*1.0)*(8.0/points)<<std::endl;
    std::cout<<"space rank_support_scan    "<<sdsl::size_in_mega_bytes(rsc)+sdsl::size_in_mega_bytes(bit_vector1)<<"(mb)   overhead space "<<((sdsl::size_in_bytes(rsc)*1.0)+sdsl::size_in_bytes(bit_vector1))*(8.0/points)<<std::endl;
    std::cout<<"*******************************************************"<<std::endl;

    for (auto _ : state)
    {
        double  rrr =0,huff =0,sd =0, rv_t =0, rv5_t = 0,sc_t =0;
        double  rrr_ac =0,huff_ac =0,sd_ac =0;


        for (int i = 0; i <10000 ; ++i) {

            unsigned int v =  (rand() % points);

            auto _start = timer::now();
            rrr_rank.rank(v);
            auto _stop = timer::now();

            auto sd_start = timer::now();
            sd_rank.rank(v);
            auto sd_stop = timer::now();

            auto start = timer::now();
            hf_v.rank(v);
            auto stop = timer::now();

            auto start_rv = timer::now();
            rv.rank(v);
            auto stop_rv = timer::now();

            auto start_rv5 = timer::now();
            rv5.rank(v);
            auto stop_rv5 = timer::now();

            auto start_sc = timer::now();
            rsc.rank(v);
            auto stop_sc = timer::now();


            sc_t  += (duration_cast<nanoseconds>(stop_sc - start_sc).count())*1.0/10000;
            rv_t  += (duration_cast<nanoseconds>(stop_rv - start_rv).count())*1.0/10000;
            rv5_t += (duration_cast<nanoseconds>(stop_rv5 - start_rv5).count())*1.0/10000;
            rrr   += (duration_cast<nanoseconds>(_stop - _start).count())*1.0/10000;
            huff  += (duration_cast<nanoseconds>(stop - start).count())*1.0/10000;
            sd    += (duration_cast<nanoseconds>(sd_stop - sd_start).count())*1.0/10000;

            auto start_ac = timer::now();
            hf_v[v];
            auto stop_ac = timer::now();

            auto _start_ac = timer::now();
            rrr_v[v];
            auto _stop_ac = timer::now();

            auto sd_start_ac = timer::now();
            sd_v[v];
            auto sd_stop_ac = timer::now();

            rrr_ac  += (duration_cast<nanoseconds>(_stop_ac - _start_ac).count())*1.0/10000;
            huff_ac += (duration_cast<nanoseconds>(stop_ac - start_ac).count())*1.0/10000;
            sd_ac   += (duration_cast<nanoseconds>(sd_stop_ac - sd_start_ac).count())*1.0/10000;
        }

        cout<<"Rank\n";
        cout << "huff_v \t" <<huff<<" (nanoseconds)"<<std::endl;
        cout << "rrr_v \t" <<rrr<<" (nanoseconds)"<<std::endl;
        cout << "sd_v \t" <<sd<<" (nanoseconds)"<<std::endl;
        cout << "rank_support_v \t" <<rv_t<<" (nanoseconds)"<<std::endl;
        cout << "rank_support_v5 \t" <<rv5_t<<" (nanoseconds)"<<std::endl;
        cout << "rank_support_scan \t" <<rv_t<<" (nanoseconds)"<<std::endl;
        std::cout<<"*******************************************************"<<std::endl;

        cout<<"Access\n";
        cout << "huff_v \t" <<huff_ac<<" (nanoseconds)"<<std::endl;
        cout << "rrr_v \t" <<rrr_ac<<" (nanoseconds)"<<std::endl;
        cout << "sd_v \t" <<sd_ac<<" (nanoseconds)"<<std::endl;
        std::cout<<"*******************************************************"<<std::endl;
    }
}
void cmp_vector(benchmark::State& state)
{
    size_t points = state.range(0);
    size_t size_block = state.range(1);

    std::string collection = "b_"+std::to_string(points);
    std::fstream bit_vector_f(collection,std::ios::in|std::ios::binary);

    std::cout<<std::endl<<"data: "<<collection<<std::endl;

    sdsl::bit_vector bit_vector1(points,0);

    uint ones = 0, zeros =0;

    for (int i = 0; i < bit_vector1.size() ; ++i) {
        unsigned char v;
        bit_vector_f >> v;
        v-=48;
        bit_vector1[i] = v;
        bit_vector1[i] == 1 ? ones++:zeros++;
    }

    std::cout<<"zeros: "<<zeros<<std::endl;
    std::cout<<"ones: "<<ones<<std::endl;

    huff_vector hf_v; hf_v.build(size_block,bit_vector1);
    sdsl::rrr_vector<> rrr_v(bit_vector1);
    sdsl::sd_vector<> sd_v(bit_vector1);


    ////rank
    sdsl::sd_vector<>::rank_1_type sd_rank(&sd_v);//O(log n/m)
    sdsl::rrr_vector<>::rank_1_type rrr_rank(&rrr_v);//O(k)
    sdsl::rank_support_v<> rv(&bit_vector1); //O(1)
    sdsl::rank_support_v5<> rv5(&bit_vector1);//O(1)
    sdsl::rank_support_scan<> rsc(&bit_vector1);//O(n)


    std::cout<<"Space  "<<std::endl;
    std::cout<<"space huff_v  "<<hf_v.size_in_mb()<<"(mb)   overhead space "<<(hf_v.size_in_mb()*1.0)*(8242880.0/points)<<std::endl;
    std::cout<<"space rrr_v   "<<sdsl::size_in_mega_bytes(rrr_v)<<"(mb)   overhead space "<<((sdsl::size_in_bytes(rrr_v))*1.0)*(8.0/points)<<std::endl;
    std::cout<<"space sd_v    "<<sdsl::size_in_mega_bytes(sd_v)<<"(mb)   overhead space "<<((sdsl::size_in_bytes(sd_v))*1.0)*(8.0/points)<<std::endl;
    std::cout<<"space rank_support_v    "<<sdsl::size_in_mega_bytes(rv)+sdsl::size_in_mega_bytes(bit_vector1)<<"(mb)   overhead space "<<((sdsl::size_in_bytes(rv)+sdsl::size_in_bytes(bit_vector1))*1.0)*(8.0/points)<<std::endl;
    std::cout<<"space rank_support_v5    "<<sdsl::size_in_mega_bytes(rv5)+sdsl::size_in_mega_bytes(bit_vector1)<<"(mb)   overhead space "<<((sdsl::size_in_bytes(rv5)+sdsl::size_in_bytes(bit_vector1))*1.0)*(8.0/points)<<std::endl;
    std::cout<<"space rank_support_scan    "<<sdsl::size_in_mega_bytes(rsc)+sdsl::size_in_mega_bytes(bit_vector1)<<"(mb)   overhead space "<<((sdsl::size_in_bytes(rsc)*1.0)+sdsl::size_in_bytes(bit_vector1))*(8.0/points)<<std::endl;
    std::cout<<"*******************************************************"<<std::endl;
    /*
     *
     * std::cout<<"space huff_v  "<<hf_v.size_in_mb()<<"(mb)   overhead space "<<(hf_v.size_in_mb()*1.0)*(8242880.0/points)<<std::endl;
    std::cout<<"space rrr_v   "<<sdsl::size_in_mega_bytes(rrr_v)+sdsl::size_in_mega_bytes(rrr_rank)<<"(mb)   overhead space "<<((sdsl::size_in_bytes(rrr_v)+sdsl::size_in_bytes(rrr_rank))*1.0)*(8.0/points)<<std::endl;
    std::cout<<"space sd_v    "<<sdsl::size_in_mega_bytes(sd_v)+sdsl::size_in_mega_bytes(sd_rank)<<"(mb)   overhead space "<<((sdsl::size_in_bytes(sd_v)+sdsl::size_in_bytes(sd_rank))*1.0)*(8.0/points)<<std::endl;
    std::cout<<"space rank_support_v    "<<sdsl::size_in_mega_bytes(rv)+sdsl::size_in_mega_bytes(bit_vector1)<<"(mb)   overhead space "<<((sdsl::size_in_bytes(rv)+sdsl::size_in_bytes(bit_vector1))*1.0)*(8.0/points)<<std::endl;
    std::cout<<"space rank_support_v5    "<<sdsl::size_in_mega_bytes(rv5)+sdsl::size_in_mega_bytes(bit_vector1)<<"(mb)   overhead space "<<((sdsl::size_in_bytes(rv5)+sdsl::size_in_bytes(bit_vector1))*1.0)*(8.0/points)<<std::endl;
    std::cout<<"space rank_support_scan    "<<sdsl::size_in_mega_bytes(rsc)+sdsl::size_in_mega_bytes(bit_vector1)<<"(mb)   overhead space "<<((sdsl::size_in_bytes(rsc)*1.0)+sdsl::size_in_bytes(bit_vector1))*(8.0/points)<<std::endl;
    std::cout<<"*******************************************************"<<std::endl;*/

    for (auto _ : state)
    {
        double rrr =0,huff =0,sd =0, rv_t =0, rv5_t = 0,sc_t =0;
        double rrr_ac =0,huff_ac =0,sd_ac =0;


        for (int i = 0; i <10000 ; ++i) {

            unsigned int v =  (rand() % points);

            auto _start = timer::now();
            rrr_rank.rank(v);
            auto _stop = timer::now();

            auto sd_start = timer::now();
            sd_rank.rank(v);
            auto sd_stop = timer::now();

            auto start = timer::now();
            hf_v.rank(v);
            auto stop = timer::now();

            auto start_rv = timer::now();
            rv.rank(v);
            auto stop_rv = timer::now();

            auto start_rv5 = timer::now();
            rv5.rank(v);
            auto stop_rv5 = timer::now();

            auto start_sc = timer::now();
            rsc.rank(v);
            auto stop_sc = timer::now();


            sc_t  += ( duration_cast<nanoseconds>(stop_sc - start_sc).count() )*1.0/10000;
            rv_t  += (duration_cast<nanoseconds>(stop_rv - start_rv).count())*1.0/10000;
            rv5_t += (duration_cast<nanoseconds>(stop_rv5 - start_rv5).count())*1.0/10000;
            rrr   += (duration_cast<nanoseconds>(_stop - _start).count())*1.0/10000;
            huff  += (duration_cast<nanoseconds>(stop - start).count())*1.0/10000;
            sd    += (duration_cast<nanoseconds>(sd_stop - sd_start).count())*1.0/10000;

            auto start_ac = timer::now();
            hf_v[v];
            auto stop_ac = timer::now();

            auto _start_ac = timer::now();
            rrr_v[v];
            auto _stop_ac = timer::now();

            auto sd_start_ac = timer::now();
            sd_v[v];
            auto sd_stop_ac = timer::now();

            rrr_ac  += (duration_cast<nanoseconds>(_stop_ac - _start_ac).count())*1.0/10000;
            huff_ac += (duration_cast<nanoseconds>(stop_ac - start_ac).count())*1.0/10000;
            sd_ac   += (duration_cast<nanoseconds>(sd_stop_ac - sd_start_ac).count())*1.0/10000;
        }

        cout<<"Rank\n";
        cout << "huff_v \t" <<huff<<" (nanoseconds)"<<std::endl;
        cout << "rrr_v \t" <<rrr<<" (nanoseconds)"<<std::endl;
        cout << "sd_v \t" <<sd<<" (nanoseconds)"<<std::endl;
        cout << "rank_support_v \t" <<rv_t<<" (nanoseconds)"<<std::endl;
        cout << "rank_support_v5 \t" <<rv5_t<<" (nanoseconds)"<<std::endl;
        cout << "rank_support_scan \t" <<rv_t<<" (nanoseconds)"<<std::endl;
        std::cout<<"*******************************************************"<<std::endl;

        cout<<"Access\n";
        cout << "huff_v \t" <<huff_ac<<" (nanoseconds)"<<std::endl;
        cout << "rrr_v \t" <<rrr_ac<<" (nanoseconds)"<<std::endl;
        cout << "sd_v \t" <<sd_ac<<" (nanoseconds)"<<std::endl;
        std::cout<<"*******************************************************"<<std::endl;
    }
}



/*BENCHMARK(create_input_test_files)->Args({2000000000})->Unit(benchmark::kMillisecond);
BENCHMARK(create_input_test_files)->Args({1000000000})->Unit(benchmark::kMillisecond);
BENCHMARK(create_input_test_files)->Args({800000000})->Unit(benchmark::kMillisecond);
BENCHMARK(create_input_test_files)->Args({400000000})->Unit(benchmark::kMillisecond);
BENCHMARK(create_input_test_files)->Args({200000000})->Unit(benchmark::kMillisecond);
BENCHMARK(create_input_test_files)->Args({100000000})->Unit(benchmark::kMillisecond);

BENCHMARK(create_input_test_files_sparse)->Args({2000000000})->Unit(benchmark::kMillisecond);
BENCHMARK(create_input_test_files_sparse)->Args({1000000000})->Unit(benchmark::kMillisecond);
BENCHMARK(create_input_test_files_sparse)->Args({800000000})->Unit(benchmark::kMillisecond);
BENCHMARK(create_input_test_files_sparse)->Args({400000000})->Unit(benchmark::kMillisecond);
BENCHMARK(create_input_test_files_sparse)->Args({200000000})->Unit(benchmark::kMillisecond);
BENCHMARK(create_input_test_files_sparse)->Args({100000000})->Unit(benchmark::kMillisecond);*/

/*BENCHMARK(cmp_vector)->Args({2000000000})->Unit(benchmark::kMillisecond);
BENCHMARK(cmp_vector)->Args({1000000000})->Unit(benchmark::kMillisecond);
BENCHMARK(cmp_vector)->Args({800000000})->Unit(benchmark::kMillisecond);
BENCHMARK(cmp_vector)->Args({400000000})->Unit(benchmark::kMillisecond);
BENCHMARK(cmp_vector)->Args({200000000})->Unit(benchmark::kMillisecond);
BENCHMARK(cmp_vector)->Args({100000000})->Unit(benchmark::kMillisecond);

BENCHMARK(cmp_vector_sparse)->Args({2000000000})->Unit(benchmark::kMillisecond);
BENCHMARK(cmp_vector_sparse)->Args({1000000000})->Unit(benchmark::kMillisecond);
BENCHMARK(cmp_vector_sparse)->Args({800000000})->Unit(benchmark::kMillisecond);
BENCHMARK(cmp_vector_sparse)->Args({400000000})->Unit(benchmark::kMillisecond);
BENCHMARK(cmp_vector_sparse)->Args({200000000})->Unit(benchmark::kMillisecond);*/
BENCHMARK(cmp_vector_sparse)->Args({100000000,16})->Unit(benchmark::kMillisecond);


BENCHMARK_MAIN();

//template<int i>
//void a(){}
//
//int main(){
//
//
//    a<8>();
//    return 0;
//}