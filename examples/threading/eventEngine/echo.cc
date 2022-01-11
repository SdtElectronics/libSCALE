#include <iostream>
#include <thread>

#include "evtEngine.h"

using dns3::EventEngine;
enum evt: unsigned char{s0, s1, s2};

auto arr = std::array<void(*)(), 3>{
    []{std::cout << "0\n" << "Enter an integer 0-2: "<< std::flush;},
    []{std::cout << "1\n" << "Enter an integer 0-2: "<< std::flush;}, 
    []{std::cout << "2\n" << "Enter an integer 0-2: "<< std::flush;}, 
};

int main(){
    EventEngine<evt> ev(64);
    std::thread et([&]{ev.ignite(arr.data());});

    bool run = true;
    unsigned int n;

    std::cout << "Enter an integer 0-2: ";
    while(run){
        std::cin >> n;
        switch (n){
        case 0:
            ev.emit(s0);
            break;
        case 1:
            ev.emit(s1);
            break;
        case 2:
            ev.emit(s2);
            break;
        
        default:
            run = false;
            ev.stall();
            break;
        }
    }

    et.join();
}
