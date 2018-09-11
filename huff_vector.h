//
// Created by alpachec on 04-06-18.
//

#ifndef CDSPROJECT_HUFF_VECTOR_H
#define CDSPROJECT_HUFF_VECTOR_H


#include <sdsl/wt_huff.hpp>
#include <sdsl/construct.hpp>
#include "utils/pair_coder.h"
#include <ostream>

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;



class huff_vector {

    public:

       // typedef sdsl::wt_huff<>::value_type value_type;
        typedef sdsl::wt_huff<>::node_type node_type;
        typedef sdsl::bit_vector::size_type size_type;
        typedef sdsl::bit_vector::value_type b_value_type;


        huff_vector() = default;

        huff_vector(const huff_vector& hv)
        {

            t_bs = hv.t_bs;
            _wt = hv._wt;
            __off = hv.__off;
            coder =  hv.coder;

        }

        explicit huff_vector(const uint8_t& block, const sdsl::bit_vector& b)
        {
            build(block,b);
        }
        ~huff_vector() = default;


        void build(const uint8_t& block,const sdsl::bit_vector& b)
        {
            t_bs = block;
            std::string s;
            coder = pair_coder(t_bs);
            ///sdsl::bit_vector::rank_1_type _rank(&b);
            //sdsl::bit_vector::size_type  n_b = b.size()/t_bs ;
           /* if(b.size()%t_bs != 0)
                n_b++;*/

            ///sdsl::bit_vector _b(n_b*t_bs);
            std::vector<std::vector<uint>>offsets;
            offsets.resize(t_bs+1);
            uint64_t x = 0;

            std::fstream ff("seq620000000",std::ios::out|std::ios::binary);

            for (int j = 0; j < b.size(); ++j) {
                if((j%t_bs == 0 )&& j != 0 )
                {
                    auto p = coder.encode(x);
                    ff << char(p.first +48);
                    s+= char(p.first +48);
                    offsets[p.first].push_back(p.second);
                    x = 0;
                }
                uint64_t _b = b[j];
                x = x | (_b << j%t_bs);
                ///std::cout<<x<<std::endl;
            }


            if((b.size()-1)%t_bs != 0)
            {
                auto p = coder.encode(x);

                ff << char(p.first +48);

                s+= char(p.first +48);

                offsets[p.first].push_back(p.second);
            }


            ff.close();

            for (int i = 0; i < offsets.size(); ++i) {

                auto m = offsets[i].size();
                if(m > 0)
                {
                    __off[i] = sdsl::int_vector<>(m,0);

                    for (int j = 0; j < m; ++j) {
                        __off[i][j] = offsets[i][j];
                    }
                    sdsl::util::bit_compress(__off[i]);
                }
            }

            sdsl::construct(_wt,"seq620000000",1);

            std::cout<<std::endl;
            std::cout<<"_wt.size              \t"<<_wt.size()<<std::endl;
            std::cout<<"_wt.sigma             \t"<<_wt.sigma<<std::endl;
            std::cout<<"_wt.size_in_mega_bytes\t"<<sdsl::size_in_mega_bytes(_wt)<<std::endl<<std::endl;


            /*std::cout<<std::endl;
            std::cout<<"sequence: "<<s<<std::endl;
            std::cout<<"offsets: "<<std::endl;

             for (int i = 0; i < offsets.size() ; ++i) {
                 std::cout<<"["<<i<<"] ";
                 for (int j = 0; j < offsets[i].size() ; ++j) {
                     std::cout<<offsets[i][j]<<" ";
                 }
                 std::cout<<std::endl;
             }

            std::cout<<std::endl;
            std::cout<<"bitvector"<<std::endl;
            for (int k = 0; k < _wt.bv.size(); ++k) {
                std::cout<<_wt.bv[k];
            }
            std::cout<<std::endl;*/


        }


        const unsigned long sigma(){ return _wt.sigma; }


        b_value_type operator[](const size_type & i)
        {

            assert(i < t_bs*_wt.size());

            size_type p  = i/t_bs;
            size_type r  = i%t_bs;

            unsigned char c = _wt[p];  // O(log t_bs)
            auto o_r = _wt.rank(p+1,c); // O(log t_bs)
            c -= 48;
           // sdsl::bit_vector b(t_bs,0);
            uint64_t ppp = __off[c][o_r-1];
            /*coder.decode(c,ppp,b);
            return b[r];*/
            uint c_ = c;
            uint64_t x = coder.decode_int(c_,ppp);
            auto rr = ((x & (1 << r)) == 0)?0:1;
            return rr;
        }

        unsigned long rank_c(size_type i, const char & c)
        {
            return _wt.rank(i,c);
        }

        unsigned long wt_get_sigma(){ return _wt.sigma;}

        unsigned long wt_get_size(){ return _wt.size();}



        unsigned long rank(const size_type& i)
        {
            assert(i <= t_bs*_wt.size());

            size_type p  = i/t_bs;
            size_type r  = i%t_bs;

            if(p >= _wt.size())
            {
                p  = _wt.size()-1;
                r  = t_bs;
            }

            int c = _wt[p]-48; // log sigma
            unsigned long _rank = 0;

            _wt.dfs(_wt.root(),p+1,[&_rank,&c,&r,this](const uint64_t & node, const uint64_t &  off)->void {

               //auto start = timer::now();

                uint c_leaf = (uint)_wt.sym(node)-48;

                if(c_leaf == c){

                    _rank += (off-1)*c_leaf;

                    uint64_t x = coder.decode_int(c_leaf,__off[c_leaf][off-1]);

                    x = x & sdsl::bits::lo_set[r];

                    _rank += sdsl::bits::cnt(x);

                }else{
                    _rank += (off)*c_leaf ;
                }
            });

            return _rank;
        }

        double size_in_mb()
        {

            double c = 0;

            for (auto &&item : __off)
            {
                c += sdsl::size_in_mega_bytes(item.second)+(sizeof(int)/1024)/1024;
            }

            return sdsl::size_in_mega_bytes(_wt) + c + coder.size_in_mb() + (sizeof(__off)/1024)/1024;
        }

        unsigned long size_in_bytes()
        {

            unsigned long c = 0;

            for (auto &&item : __off)
            {
                c += sdsl::size_in_bytes(item.second)+sizeof(int);
            }

            return sdsl::size_in_bytes(_wt) + c + coder.size_in_bytes()+sizeof(__off);
        }

        void save(std::ostream &f) const{
            sdsl::serialize(t_bs,f);
            sdsl::serialize(_wt,f);
            /*std::cout<<"wt :\t";
            for (auto &&wt : _wt) {
                std::cout<<wt<<" ";
            }
            std::cout<<std::endl;*/
            sdsl::serialize(__off.size(),f);
            for (auto &&item : __off) {

//                std::cout<<item.first<<":\t";
                sdsl::serialize(item.first,f);
                /*for (auto &&item2 : item.second) {
                    std::cout<<item2<<" ";
                }
                std::cout<<std::endl;*/

                sdsl::serialize(item.second,f);
            }
        }

        void load(std::fstream &f){

            sdsl::load(t_bs,f);
            sdsl::load(_wt,f);
            /*std::cout<<"wt :\t";
            for (auto &&wt : _wt) {
                std::cout<<wt<<" ";
            }
            std::cout<<std::endl;*/

            size_t map_size;
            sdsl::load(map_size,f);
            for (size_t i = 0; i < map_size ; ++i) {
                unsigned int first;
                sdsl::load(first,f);
                sdsl::int_vector<> X;
                sdsl::load(X,f);

                /*std::cout<<first<<":\t";
                for (auto &&item : X) {
                    std::cout<<item<<" ";
                }
                std::cout<<std::endl;*/
                __off[first] = X;
            }

            coder = pair_coder(t_bs);
        }

    private:
        sdsl::wt_huff<> _wt;
        std::unordered_map<unsigned int,sdsl::int_vector<>> __off;
        pair_coder coder;
        uint8_t t_bs;



};


#endif //CDSPROJECT_HUFF_VECTOR_H
