#include <iostream>
#include <thread>

#include "evtEngine.h"

using dns3::EventEngine;

enum evt: unsigned char{s0, s1, s2};

struct t{	
	int zero = 0;
	int one  = 1;
	int two  = 2;

	void f0(){	
		std::cout << zero << std::flush;
	}

	void f1(){	
		std::cout << one << std::flush;
	}

	void f2(){	
		std::cout << two << std::flush;
	}

	void fin(){	
		std::cout << "\nEnter an integer 0-2: " << std::flush;
	}
};

auto arr = std::array<decltype(&t::f2), 3>{
	&t::f0, &t::f1, &t::f2
};

int main(){	
	t inst;
    EventEngine<evt> ev(64);
	std::thread et([&]{ev.ignite(arr.data(), &t::fin, inst);});

    bool run = true;
    unsigned int n;

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
