//
// Created by alpachec on 04-06-18.
//

#ifndef CDSPROJECT_HUFF_VECTOR_H
#define CDSPROJECT_HUFF_VECTOR_H

#include <sdsl/wavelet_trees.hpp>
#include <sdsl/wt_huff.hpp>
#include <sdsl/construct.hpp>
#include "utils/pair_coder.h"

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;


template < uint16_t t_bs = 63>
class huff_vector {

    public:

       // typedef sdsl::wt_huff<>::value_type value_type;
        typedef sdsl::wt_hutu<>::node_type node_type;
        typedef sdsl::bit_vector::size_type size_type;
        typedef sdsl::bit_vector::value_type b_value_type;


        huff_vector() = default;
        huff_vector(const huff_vector& hv)
        {
            _wt = hv._wt;
            offsets = hv.offsets;
            coder =  hv.coder;
            rank_1_wt = sdsl::bit_vector::rank_1_type(&_wt.bv);
        }

        explicit huff_vector(const sdsl::bit_vector& b)
        {
            build(b);
        }
        ~huff_vector() = default;


        void build(const sdsl::bit_vector& b)
        {
            std::string s;

            ///sdsl::bit_vector::rank_1_type _rank(&b);
            //sdsl::bit_vector::size_type  n_b = b.size()/t_bs ;
           /* if(b.size()%t_bs != 0)
                n_b++;*/

            ///sdsl::bit_vector _b(n_b*t_bs);

            offsets.resize(t_bs+1);
            uint64_t x = 0;

            for (int j = 0; j < b.size(); ++j) {
                if((j%t_bs == 0 )&& j != 0 )
                {
                    auto p = coder.encode(x);
                    s+= char(p.first +48);
                    offsets[p.first].push_back(p.second);
                    x = 0;
                }
                uint64_t _b = b[j];
                x = x | (_b << (j%t_bs));
                ///std::cout<<x<<std::endl;
            }

            if((b.size()-1)%t_bs != 0)
            {
                auto p = coder.encode(x);
                s+= char(p.first +48);
                offsets[p.first].push_back(p.second);
            }



           /*for (auto it = _b.begin(); it !=_b.end(); it+=t_bs) {

                auto p = coder.encode(it,it+t_bs);
                uint c_i = p.first ;
                uint o_i = p.second ;
                s+= char(c_i+48);
                offsets[c_i].push_back(o_i);
            }
*/
            sdsl::construct_im(_wt,s,1);

            std::cout<<"_wt.size              \t"<<_wt.size()<<std::endl;
            std::cout<<"_wt.sigma             \t"<<_wt.sigma<<std::endl;
            std::cout<<"_wt.size_in_mega_bytes\t"<<sdsl::size_in_mega_bytes(_wt)<<std::endl;


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


        b_value_type operator[](size_type i)
        {

            assert(i < t_bs*_wt.size());

            size_type p  = i/t_bs;
            size_type r  = i%t_bs;

            unsigned char c = _wt[p];  // O(log t_bs)
            auto o_r = _wt.rank(p+1,c); // O(log t_bs)
            c -= 48;
           // sdsl::bit_vector b(t_bs,0);
            auto ppp = offsets[c][o_r-1];
            /*coder.decode(c,ppp,b);
            return b[r];*/
            uint c_ = c;
            uint64_t x = coder.decode_int(c_,ppp);

            return ((x & (1 << r)) == 0)?0:1;
        }

        unsigned long rank_c(size_type i, const char & c)
        {
            return _wt.rank(i,c);
        }

        unsigned long wt_get_sigma(){ return _wt.sigma;}

        unsigned long wt_get_size(){ return _wt.size();}

        unsigned long rank(size_type i)
        {
            assert(i <= t_bs*_wt.size());

            size_type p  = i/t_bs;
            size_type r  = i%t_bs;

            int c = _wt[p]-48; // log sigma
            unsigned long _rank = 0;

            _wt.dfs(_wt.root(),p+1,[&_rank,&c,&r,this](const uint64_t & node, const uint64_t &  off)->void {

               //auto start = timer::now();

                uint c_leaf = (uint)_wt.sym(node)-48;

                if(c_leaf == c){

                    _rank += (off-1)*c_leaf;

                    uint64_t x = coder.decode_int(c_leaf,offsets[c_leaf][off-1]);

                    x = x & sdsl::bits::lo_set[r];

                    _rank += sdsl::bits::cnt(x);

                }else{
                    _rank += (off)*c_leaf ;
                }
            });

           ////// _wt.bfs(p,[/*&_rank,&c,&r,this*/](const uint64_t & node, const uint64_t &  off)->void {

                /*//auto start = timer::now();

                uint c_leaf = (uint)_wt.sym(node)-48;

                if(c_leaf == c){

                    _rank += (off-1)*c_leaf;

                    uint64_t x = coder.decode_int(c_leaf,offsets[c_leaf][off-1]);

                    x = x & sdsl::bits::lo_set[r];

                    _rank += sdsl::bits::cnt(x);

                }else{
                    _rank += (off)*c_leaf ;
                }*/

            /////});

            return _rank;
        }


        template < typename Fn>
        void bfs_wt_leaf(const Fn& func,const uint &r)
        {


            std::vector< std::pair<node_type, uint > > Q;
            unsigned long nn = pow(2,(int)log2(_wt.sigma)+1)-1;
            Q.reserve(nn);
            node_type node = _wt.root();
            unsigned long top = 0,end = 1;
            //sdsl::rank_support_v<1> bv_rank(&_wt.bv) ;
            Q.emplace_back(std::make_pair(node,r+1));

            while(top < end){

                std::pair<node_type, uint> p;
                p = Q[top];

                if(_wt.is_leaf(p.first)){
                    func(p.first,p.second);
                } else
                {
                    auto limits = _wt.bit_vec(p.first);
                    auto init = limits.begin() - _wt.bv.begin();

                    auto ones = rank_1_wt.rank(init + p.second) - rank_1_wt.rank(init);
                    auto zeros = p.second - ones;
                    auto childs = _wt.expand(p.first);

                    node_type left = childs[0];
                    if(zeros > 0)
                    {
                        p = std::make_pair(left,zeros);
                        Q.emplace_back(p);
                        end++;
                    }



                    node_type right = childs[1];

                    if(ones > 0)
                    {
                        p = std::make_pair(right,ones);
                        Q.emplace_back(p);
                        end++;
                    }

                }
                top++;

            }


            // auto start = timer::now();
           /* unsigned long nn = pow(2,(int)log2(_wt.sigma)+1)-1;
            std::vector< std::pair<node_type, std::pair<std::pair<uint,uint>,uint> > > Q;
            Q.reserve(nn);
            node_type node = _wt.root();
            Q.emplace_back(std::make_pair(node,std::make_pair(std::make_pair(0,_wt.size()-1),r)));
            unsigned long top = 0,end = 1, current_ch =1, current_pos = _wt.size()-1;
            auto pz = std::make_pair<uint ,uint>(0,0);

            while(top < end)
            {
                if(Q[top].second.first == pz)
                {
                    func(Q[top].first,Q[top].second.second);
                }
                else
                {
                    //auto limits = _wt.bit_vec(Q[top].first);
                    //auto init = limits.begin() - _wt.bv.begin();
                    uint init_node = Q[top].second.first.first; //begin interval
                    uint end_node = Q[top].second.first.second; //end interval
                    uint _size = end_node - init_node + 1;
                    uint jj = Q[top].second.second; //begin interval

                    auto ones = rank_1_wt.rank(init_node + jj) - rank_1_wt.rank(init_node);
                    auto zeros = jj - ones;
                    //auto childs = _wt.expand(Q[top].first);

                    auto size_right = rank_1_wt.rank(end_node + 1) - rank_1_wt.rank(init_node);
                    auto size_left = _size - size_right;



                        if (_wt.is_leaf(current_ch)) {
                            Q.emplace_back(std::make_pair(current_ch, std::make_pair(std::make_pair(0, 0), zeros)));
                            ++current_ch;
                        } else {
                            auto p = std::make_pair(current_ch, std::make_pair(
                                    std::make_pair(current_pos + 1, current_pos + size_left), zeros));
                            Q.emplace_back(p);
                            ++current_ch;
                            current_pos += size_left;
                        }

                        end++;



                        if(_wt.is_leaf(current_ch))
                        {
                            Q.emplace_back(std::make_pair(current_ch,std::make_pair(std::make_pair(0,0),ones)));
                            ++current_ch;
                        }
                        else
                        {
                            if(ones > 0)
                            {
                                auto p = std::make_pair(current_ch,std::make_pair(std::make_pair(current_pos+1,current_pos+size_right),ones));
                                Q.emplace_back(p);
                                current_pos += size_right;
                            }
                            ++current_ch;
                        }
                        end++;

                }

                top++;

            }


       /*     std::vector< std::pair<node_type, uint > > Q;
            unsigned long nn = pow(2,(int)log2(_wt.sigma)+1)-1;
            Q.reserve(nn);
            node_type node = _wt.root();
            Q.push_back(std::make_pair(node,r));
            unsigned int  top = 0;

            while(top < nn ){



                if(_wt.is_leaf(Q[top].first)){
                    func(Q[top].first,Q[top].second);
                } else {
                    auto limits = _wt.bit_vec(Q[top].first);
                    auto init = limits.begin() - _wt.bv.begin();

                    auto ones = rank_1_wt.rank(init + Q[top].second) - rank_1_wt.rank(init);
                    auto zeros = Q[top].second - ones;
                    auto childs = _wt.expand(Q[top].first);

                    node_type left = childs[0];
                    auto p = std::make_pair(left,zeros);
                    Q.emplace_back(p);

                    node_type right = childs[1];
                    p = std::make_pair(right,ones);
                    Q.emplace_back(p);


                }
                ++top;

            }*/

/*
            std::vector< std::pair<node_type, unsigned int > > Q;
            node_type node = _wt.root();
            unsigned long nn = pow(2,(int)log2(_wt.sigma)+1)-1;
            Q.reserve(nn);
            Q.emplace_back(std::make_pair(node,r));
            unsigned long top = 0;
            unsigned long current_ch = 0;


            while(top < nn){
                if(_wt.is_leaf(Q[top].first)){
                    func(Q[top].first,Q[top].second);
                } else {
                    auto p = Q[top];
                    auto limits = _wt.bit_vec(Q[top].first);
                    auto init = limits.begin() - _wt.bv.begin();

                    auto ones = rank_1_wt.rank(init + Q[top].second) - rank_1_wt.rank(init);
                    auto zeros = Q[top].second - ones;
                    auto childs = _wt.expand(Q[top].first);

                    Q.emplace_back(childs[0],zeros);
                    Q.emplace_back(childs[1],ones);
                }
                top++;
            }
*/
         //   auto stop = timer::now();

            //std::cout<<"bfs_wt_leaf\t"<<duration_cast<nanoseconds>(stop - start).count()<<std::endl;
        }


        double size_in_mb(){
            double c = 0;
            for (auto &&item : offsets) {
                c += item.size()*sizeof(int)*1.0/1024*1024;
            }

            return sdsl::size_in_mega_bytes(_wt) +
                   sdsl::size_in_mega_bytes(rank_1_wt) + c;

        }

/*
        void bfs(){


            std::queue<node_type> Q;

            Q.push(0);
            sdsl::rank_support_v<1> bv_rank(&_wt.bv) ;


            while(!Q.empty()) {

                auto p = Q.front();
                Q.pop();


                std::cout<<std::endl;
                std::cout<<"bitmap node: "<<p<<" "<<std::endl;
                auto b = _wt.bit_vec(p);

                auto init = b.begin() - _wt.bv.begin();
                auto end  = b.end() - _wt.bv.begin();
                for (int i = init; i < end && i < _wt.bv.end()-_wt.bv.begin() ; ++i) {
                    std::cout<<_wt.bv[i]<<" ";
                }
                std::cout<<std::endl;

                if(!_wt.is_leaf(p)){
                    auto childs  = _wt.expand(p);
                    if(childs[0] < _wt.size())Q.push(childs[0]);
                    if(childs[1] < _wt.size())Q.push(childs[1]);
                }


            }
        }
*/
    private:
        sdsl::wt_huff<> _wt;
        sdsl::bit_vector::rank_1_type rank_1_wt;
        std::vector<std::vector<uint>>offsets;
        pair_coder<t_bs> coder;




};


#endif //CDSPROJECT_HUFF_VECTOR_H
