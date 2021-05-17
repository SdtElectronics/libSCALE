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

#pragma once

#include "circList.h"

template<typename T, typename A = std::allocator<T> >
class ccBuf: public ccList<T, A>{
  public:
    //Explicit specification for scope of dependent names
    //See https://stackoverflow.com/a/4010291/10627291 for details
    using ccList<T, A>::head;
    using ccList<T, A>::tail;
    using ccList<T, A>::next;
    using ccList<T, A>::ator;
    using ccList<T, A>::_size;
    using ccList<T, A>::ccList;

    ccBuf(size_t size);
    ~ccBuf();

    template<typename I>
    void init(I begin, I end);

    inline bool init(T val);
};

template<typename T, typename A>
ccBuf<T, A>::ccBuf(size_t size): ccList<T, A>::ccList(size){
    head = ator.allocate(size);
    tail = head;
    next = ator.allocate(1);
}

template<typename T, typename A>
ccBuf<T, A>::~ccBuf(){
    ator.deallocate(head, _size);
    _size = 0;
}

template<typename T, typename A>
bool ccBuf<T, A>::init(T val){
    ccNode<T>* tmp = tail;
    if(++tail - head != _size){
        new(tmp) ccNode<T>(val, static_cast<ccNode<T>*>(tail));
        return true;
    }else{
        new(tmp) ccNode<T>(val, head);
        tail--;
        return false;
    }
}

template<typename T, typename A>
template<typename I>
void ccBuf<T, A>::init(I begin, I end){
    do{
        //Avoiding unsequenced modification
        ccNode<T>* tmp = tail;
        new(tmp) ccNode<T>(*begin, ++tail);
    }while(++begin != (end - 1));
    new(tail) ccNode<T>(*begin, head);
}