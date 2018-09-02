//
// Created by alpachec on 04-06-18.
//

#ifndef CDSPROJECT_PAIR_CODER_H
#define CDSPROJECT_PAIR_CODER_H

#include <cstdint>
#include <sys/param.h>
#include <int_vector.hpp>

template < uint16_t t_b = 63 >
class pair_coder {

    public:

        pair_coder(){

            K  = new unsigned long*[t_b+1];

            for (int i = 0; i <= t_b; ++i) {
                K[i] =  new unsigned long[t_b+1];
            }

            buildCoeff();


        };
        ~pair_coder(){
            for (int i = 0; i < t_b+1 ; ++i) {
                delete K[i];
            }

            delete K;

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

        std::pair<uint, uint> encode(sdsl::bit_vector b){

            assert(b.size()==t_b);

            uint c = 0;
            for (int i = 0; i < t_b ; ++i) {
                c += b[i];
            }

            uint off = 0;
            uint c_t = c;
            uint j = 0;
            while(0 < c_t && c_t <= t_b-j){
                if(b[j]){
                    if(t_b-j-1 >= c_t)
                        off += K[t_b-j-1][c_t];
                    c_t--;
                }
                j++;
            }

            return std::make_pair(c,off+1);
        };

        sdsl::bit_vector decode(uint c, uint off){

            assert(c <= t_b);

            sdsl::bit_vector B(t_b);
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
        };

    private:

        unsigned long **K;

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
