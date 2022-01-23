#include <iostream>

#include "micoro.h"

const char* generator(int& state){
    const char* out = "out";
	COSTART(state)
    COYIELD("first")
    COYIELD("second")
    COFINAL(out)
}

int main(){
    int state = 0;
    std::cout << generator(state) << std::endl;
    std::cout << generator(state) << std::endl;
    std::cout << generator(state) << std::endl;
    std::cout << generator(state) << std::endl;
}
