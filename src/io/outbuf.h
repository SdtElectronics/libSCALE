/*
* Copyright (c) 2021 SdtElectronics . All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
* 
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
* 
* 3. Neither the name of the copyright holder nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/

#include <cstddef>
#include <cstdint>
#include <array>

inline std::uint8_t setbit(std::uint8_t c, int i, bool bit){
    return c |= bit << i;
}

template <std::size_t L>
class bcbuf{
  public:
    bcbuf();
    using buf_data = std::array<std::uint8_t, L>;

    // Write a single bit after the current position
    // and move the current position a bit forward
    inline void write(bool bit);

    // Write bytes from begin to end 
    // with the same bit order as input
    template <typename T>
    inline void write(T begin, T end);

    // Write bytes from begin to end 
    // with the inverse bit order as input
    template <typename T>
    inline void write_reverse(T begin, T end)

    inline buf_data& data();

    
    inline void reset();

  private:
    buf_data buf;
    typename buf_data::iterator cur;
    std::size_t bcount;
    
};

template <std::size_t L>
bcbuf<L>::bcbuf(): cur(buf.begin()){
}

template <std::size_t L>
typename bcbuf<L>::buf_data& bcbuf<L>::data(){
    return buf;
}

template <std::size_t L>
void bcbuf<L>::reset(){
    bcount = 0;
    cur = buf.begin();
}

template <std::size_t L>
void bcbuf<L>::write(bool bit){
    std::size_t mod = (bcount++) & 0x07u;
    std::uint8_t tmp = *cur;
    tmp = mod ? tmp : 0x00;
    *cur = setbit(tmp, mod, bit);
    cur += mod == 0x07u;
}

template <std::size_t L>
template <typename T>
inline void bcbuf<L>::write(T begin, T end){
    while(begin != end){
        std::uint8_t tmp = 0x00;
        // -O3 would unroll this automatically
        for(std::size_t i = 0; i != 8; ++i){
            tmp = setbit(tmp, i, *(begin++));
        }
        *(cur++) = tmp;
    }
}

template <std::size_t L>
template <typename T>
inline void bcbuf<L>::write_reverse(T begin, T end){
    while(begin != end){
        std::uint8_t tmp = 0x00;
        // -O3 would unroll this automatically
        for(std::size_t i = 7; i >= 0; --i){
            tmp = setbit(tmp, i, *(begin++));
        }
        *(cur++) = tmp;
    }
}