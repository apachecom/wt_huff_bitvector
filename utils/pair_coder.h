//
// Created by alpachec on 04-06-18.
//

#ifndef CDSPROJECT_PAIR_CODER_H
#define CDSPROJECT_PAIR_CODER_H

#include <cstdint>
#include <sys/param.h>
#include <int_vector.hpp>


using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;



class pair_coder {

    public:

        pair_coder() = default;

        pair_coder(const uint8_t &block){

            t_b = block;

            K.resize(t_b+1);
            for (auto &&item : K) {
                item.resize(t_b+1);
            }

            buildCoeff();


        };

        ~pair_coder(){

            for (int i = 0; i < K.size() ; ++i) {
                K[i].clear();
            }
            K.clear();

        }

        pair_coder&operator=(const pair_coder& P)
        {
            t_b = P.t_b;
            K = P.K;

        }

        std::pair<uint, uint> encode(uint64_t &v_t)
        {
            auto c = sdsl::bits::cnt(v_t);
            uint off = 0;
            uint c_t = c;
            uint jj = 0;

            uint64_t v = v_t;
            while(0 < c_t && c_t <= t_b-jj)
            {
                if((v & 1) == 1)
                {
                    if(t_b-jj-1 >= c_t)
                        off += K[t_b-jj-1][c_t];
                    c_t--;

                }
                v = v >> 1;
                jj++;
            }
            return std::make_pair(c,off+1);
        }


        uint64_t decode_int(const uint &_c,  const uint &_off)
        {
            uint64_t c = _c;
            uint64_t off = _off;
            uint64_t q = 0;
            uint64_t j = 0;
            while ( c )
            {
                uint64_t C = (t_b-j-1 >= c)? K[t_b-j-1][c]:0;
                if(off-1 >= C)
                {
                    q = q | (1 << j);
                    off-= C;
                    c--;
                }
                j++;
            }
            return q;
        }



        std::pair<uint, uint> encode(sdsl::bit_vector::iterator i, sdsl::bit_vector::iterator j){
            assert(j > i);
            assert(j-i == t_b);

            uint c = 0;

            auto it = i;

            for (it; it < j ; ++it) {
                c += *(it);

            }

            uint off = 0;
            uint c_t = c;
            uint jj = 0;
            while(0 < c_t && c_t <= t_b-jj){
                if(*(i+jj) == 1){
                    if(t_b-jj-1 >= c_t)
                        off += K[t_b-jj-1][c_t];
                    c_t--;
                }
                jj++;
            }

            return std::make_pair(c,off+1);


        };

        std::pair<uint, uint> encode(const sdsl::bit_vector &b){

            assert(b.size()==t_b);

            uint c = 0;
            for (int i = 0; i < t_b ; ++i) {
                c += b[i];
            }

            uint off = 0, c_t = c, j = 0;

            while(0 < c_t && c_t <= t_b-j)
            {
                if(b[j])
                {
                    if(t_b-j-1 >= c_t)
                        off += K[t_b-j-1][c_t];
                    c_t--;
                }
                j++;
            }

            return std::make_pair(c,off+1);
        }

        void decode(unsigned char &_c,  uint &_off, sdsl::bit_vector& B){

            unsigned char c = _c;
            uint off = _off;

            uint j = 0;
            while ( c ){
                uint C = (t_b-j-1 >= c)? K[t_b-j-1][c]:0;
                if(off-1 >= C){
                    B[j] = 1;
                    off-= C;
                    c--;
                }

                j++;
            }

/*
            ///B = sdsl::bit_vector(t_b,0);
            uint j = 0;

            ///auto start = timer::now();
            while ( c > 0 )
            {
                size_t C = (t_b-j-1 >= c)? K[t_b-j-1][c]:0;
                    if(off-1 >= C)
                    {
                        B[j] = 1;
                        off-= C;
                        c--;
                    }
                j++;
            }
            */
            ///auto stop = timer::now();
            ///std::cout<<"decode\t\t\t"<<duration_cast<nanoseconds>(stop - start).count()<<std::endl;
        }

        sdsl::bit_vector decode(uint &_c,  uint &_off){

            sdsl::bit_vector B(t_b,0);

            unsigned char c = _c;
            uint off = _off;

            uint j = 0;
            while ( c ){
                uint C = (t_b-j-1 >= c)? K[t_b-j-1][c]:0;
                if(off-1 >= C){
                    B[j] = 1;
                    off-= C;
                    c--;
                }

                j++;
            }

            return B;

        }

        double size_in_mb()
        {
            return   (sizeof(long)*1.0)*(t_b/1024)*(1.0*t_b/1024);
        }

        unsigned long  size_in_bytes()
        {
            return  sizeof(long)*t_b*t_b;
        }

    private:
        uint8_t t_b;

        std::vector<std::vector<unsigned long> > K;

        void buildCoeff(){

            for (int k = 0; k <= t_b; ++k) {
                for (int i = 0; i <= t_b; ++i) {
                    K[k][i] =  0;
                }
            }
            std::cout<<std::endl;

            for (int i=0; i <= t_b; i++) {
                K[i][0] = 1;
            }

            for (int i=1; i<=t_b; i++) {
                K[i][1] = i;
            }

            for (int i=2; i<=t_b; i++) {
                K[i][i] = 1;
            }

            for (int i=3; i <= t_b;i++) {
                for (int j=2; j < t_b; j++) {
                    if(j<=t_b) {
                        K[i][j] = K[i-1][j-1] + K[i-1][j];
                    }
                }
            }



        }


};


#endif //CDSPROJECT_PAIR_CODER_H
