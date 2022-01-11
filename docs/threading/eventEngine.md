# Event Engine
`EventEngine` implements a minimalist event-loop engine, which listens events represented by an `enum` type from other threads and calls corresponding handlers. When all queued events are handled, a completion callback, if provided, is invoked and then the thread is put into sleep (blocked), waiting for subsequent incoming events.

## Quick Reference
### Initialization
```C++
enum EvType{s0, s1, s2};
constexpr std::size_t capacity = 128;
EventEngine<EvType> ev(capacity);
```

### Start the Event Loop
```C++
auto callbacks = std::array<void(*)(), 3>{
    []{std::cout << "0" << std::endl;},
    []{std::cout << "1" << std::endl;}, 
    []{std::cout << "2" << std::endl;}, 
};

ev.ignite(callbacks.data());
```
This call will block until `stall()` is called.

`ignite()` can also accept a second parameter as the completion callback, which is invoked each time all events in the queue is handled. The provided callable must be the same type as handlers in the first parameter. 

### Fire an Event
```C++
ev.emit(event);
```
If number of queued events has exceeded the designated value, this call will block until there is a space in the queue.

### Stop the Event Loop
```C++
ev.stall();
```
If there is no unhandled events, `ignite()` will return immediately after `stall()` is called. Otherwise `ignite()` will return after all queued events are handled and completion callback is invoked.

## Member Function Pointers as Callbacks
`ignite()` has two overloads to accept member function pointers as callbacks. Things are slightly different here as member functions can only be called on an instance, which is passed as a reference to `ignite()`. This allows using function objects as callbacks without the heavy overhead introduced by `std::function`. For detailed examples, see [memberFp](../../examples/threading/eventEngine/memberFp.cc).
