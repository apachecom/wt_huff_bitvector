//
// Created by alpachec on 04-06-18.
//

#ifndef CDSPROJECT_HUFF_VECTOR_H
#define CDSPROJECT_HUFF_VECTOR_H


#include <wt_huff.hpp>
#include <construct.hpp>
#include "utils/pair_coder.h"


template < uint16_t t_bs = 63>
class huff_vector {

    public:

       // typedef sdsl::wt_huff<>::value_type value_type;
        typedef sdsl::wt_huff<>::node_type node_type;
        typedef sdsl::bit_vector::size_type size_type;
        typedef sdsl::bit_vector::value_type b_value_type;


        huff_vector() = default;
        huff_vector(const huff_vector& hv)
        {
            _wt = hv._wt;
            offsets = hv.offsets;
            coder =  hv.coder;
        }

        explicit huff_vector(const sdsl::bit_vector& b)
        {
            build(b);
        }
        ~huff_vector() = default;


        void build(const sdsl::bit_vector& b)
        {
            std::string s;

            sdsl::bit_vector::rank_1_type _rank(&b);
            sdsl::bit_vector::size_type  n_b = b.size()/t_bs ;
            if(b.size()%t_bs != 0)n_b++;

            sdsl::bit_vector _b(n_b*t_bs);

            for (int j = 0; j < b.size(); ++j) {
                _b[j] = b[j];
            }

           offsets.resize(t_bs+1);

           for (auto it = _b.begin(); it !=_b.end(); it+=t_bs) {

                auto p = coder.encode(it,it+t_bs);
                uint c_i = p.first ;
                uint o_i = p.second ;
                s+= char(c_i+48);
                offsets[c_i].push_back(o_i);
            }

            sdsl::construct_im(_wt,s,1);



            /*std::cout<<std::endl;
            std::cout<<"sequence: "<<s;
             std::cout<<"offsets: "<<std::endl;

             for (int i = 0; i < offsets.size() ; ++i) {
                 std::cout<<"["<<i<<"] ";
                 for (int j = 0; j < offsets[i].size() ; ++j) {
                     std::cout<<offsets[i][j]<<" ";
                 }
                 std::cout<<std::endl;
             }

             std::cout<<std::endl;*/

        }


        const unsigned long sigma(){ return _wt.sigma; }


        b_value_type operator[](size_type i)
        {

            assert(i < t_bs*_wt.size());

            size_type p  = i/t_bs;
            size_type r  = i%t_bs;

            auto c = _wt[p];  // O(log t_bs)
            auto o_r = _wt.rank(p+1,c); // O(log t_bs)
            c-=48;
            auto b = coder.decode(c,offsets[c][o_r-1]);

            return b[r];

        }

        const unsigned long rank(size_type i)
        {

            assert(i <= t_bs*_wt.size());

            size_type p  = i/t_bs;
            size_type r  = i%t_bs;
            bool last = false;
            if(p == _wt.size()){
                p--;
                r = t_bs-1;
                last = true;
            }

            auto c = _wt[p]-48;
            unsigned long _rank = 0;

            bfs_wt_leaf([&_rank,&c,&r,&last,this](const uint node, uint off)->void {

                auto c_leaf = _wt.sym(node)-48;

                if(c_leaf == c){

                    _rank += (off)*c_leaf;

                    auto b  = coder.decode(c_leaf,offsets[c_leaf][off]);

                    sdsl::rank_support_v<1> rb(&b);

                    _rank += rb.rank(r) + last*b[b.size()-1];

                }else{
                    _rank += (off)*c_leaf ;
                }

            },p);


            return _rank;

        }

        template < typename Fn>
        void bfs_wt_leaf(Fn func,uint r)
        {

            std::queue< std::pair<node_type, uint > > Q;
            node_type node = _wt.root();
            sdsl::rank_support_v<1> bv_rank(&_wt.bv) ;
            Q.push(std::make_pair(node,r));

            while(!Q.empty()){

                std::pair<node_type, uint> p;
                p = Q.front();
                Q.pop();

                if(_wt.is_leaf(p.first)){
                    func(p.first,p.second);
                } else {
                    auto limits = _wt.bit_vec(p.first);
                    auto init = limits.begin() - _wt.bv.begin();

                    auto ones = bv_rank.rank(init + p.second) - bv_rank.rank(init);
                    auto zeros = p.second - ones;
                    auto childs = _wt.expand(p.first);

                    node_type left = childs[0];
                    p = std::make_pair(left,zeros);
                    Q.push(p);


                    node_type right = childs[1];
                    p = std::make_pair(right,ones);
                    Q.push(p);


                }

            }
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
        std::vector<std::vector<uint>>offsets;
        pair_coder<t_bs> coder;




};


#endif //CDSPROJECT_HUFF_VECTOR_H
