
#ifndef dns3evtengine
#define dns3evtengine

#include <array>
#include <atomic>
#include <cstdint>
#include <exception>
#include <type_traits>

#include "queue.h"

namespace dns3{

// Type traits to deduce the instance type of member function pointer
template <typename> struct member_function_traits;

template<typename T>
class EventEngine{
    static_assert(std::is_enum<T>::value, "T must be an enum type");
    template<typename FP>
    using instanceType = typename member_function_traits<FP>::instance_type;

  public:
    // capacity: maximum number events in queue, pushing more is blocking
    EventEngine(std::size_t capacity);

    // Start the engine
    // cbList: Pointer to the first element in callback list
    template<typename F>
    void ignite(F* cbList);
    // Start the engine
    // cbList: Pointer to the first element in callback list
    // onFinish: Callback to be invoked when a loop is finished
    template<typename F>
    void ignite(F* cbList, F onFinish);
    // Start the engine, with member function pointer as callback
    // cbList: Pointer to the first element in callback list
    // instance: instance that member function to be called on
    template<typename F>
    void ignite(F* cbList, instanceType<F>& instance);
    // Start the engine, with member function pointer as callback
    // cbList: Pointer to the first element in callback list
    // instance: instance that member function to be called on
    // onFinish: Callback to be invoked when a loop is finished
    template<typename F>
    void ignite(F* cbList, F onFinish, instanceType<F>& instance);

    // Stop the engine
    void stall(); 
    // Push a new event to queue
    inline void emit(T event);

  private:
    std::atomic_bool run_;
    BlockingQueue<T> events_;
};

template<typename T>
EventEngine<T>::EventEngine(std::size_t capacity):
    events_(capacity){
}

template<typename T>
template<typename F>
void EventEngine<T>::ignite(F* cbList){
    run_ = true;
    while(run_){
        T event;
        while(events_.tryDequeue(event)){
            cbList[event]();
        }
        events_.wait();
    }
}

template<typename T>
template<typename F>
void EventEngine<T>::ignite(F* cbList, F onFinish){
    run_ = true;
    while(run_){
        T event;
        while(events_.tryDequeue(event)){
            cbList[event]();
        }
        onFinish();
        events_.wait();
    }
}

template<typename T>
template<typename F>
void EventEngine<T>::ignite(F* cbList, instanceType<F>& instance){
    run_ = true;
    while(run_){
        T event;
        while(events_.tryDequeue(event)){
            (instance.*(cbList[event]))();
        }
        events_.wait();
    }
}

template<typename T>
template<typename F>
void EventEngine<T>::ignite(F* cbList, F onFinish, instanceType<F>& instance){
    run_ = true;
    while(run_){
        T event;
        while(events_.tryDequeue(event)){
            (instance.*(cbList[event]))();
        }
        (instance.*onFinish)();
        events_.wait();
    }
}

template<typename T>
void EventEngine<T>::stall(){
    run_ = false;
    events_.wake();
}

template<typename T>
void EventEngine<T>::emit(T event){
    events_.enqueue(std::move(event));
}

template <typename Return, typename Object, typename... Args>
struct member_function_traits<Return (Object::*)(Args...)>{
    typedef Return return_type;
    typedef Object instance_type;
    typedef Object & instance_reference;
};

// Const member functions specialization
template <typename Return, typename Object, typename... Args>
struct member_function_traits<Return (Object::*)(Args...) const>{
    typedef Return return_type;
    typedef Object instance_type;
    typedef Object const & instance_reference;
};

}
#endif