
#ifndef thdblockingqueue
#define thdblockingqueue

#include <condition_variable>
#include <deque>
#include <mutex>

template<typename T>
class BlockingQueue{
  public:
    // capacity: maximum number events in queue, pushing more is blocking
    BlockingQueue(size_t capacity);
    // enqueue item, if queue is full this will block
    void enqueue(T &&item);
    // enqueue item if queue is not full, return true
    // otherwise return false without blocking
    bool tryEnqueue(T &&item);
    // dequeue to item, if queue is empty this will block
    void dequeue(T &item);
    // dequeue to item if queue is not full, return true
    // otherwise return false without blocking
    bool tryDequeue(T &item);
    // wait (block) until queue is not empty or wake() is called
    void wait();
    // wake the thread from wating (blocking)
    void wake();
    // change the capacity
    void resize(std::size_t capacity);

  private:
    std::deque<T> content_;
    std::size_t capacity_;

    std::mutex mutex_;
    std::condition_variable notEmpty_;
    std::condition_variable notFull_;
    bool wake_;

    BlockingQueue(const BlockingQueue &) = delete;
    BlockingQueue(BlockingQueue &&) = delete;
    BlockingQueue &operator = (const BlockingQueue &) = delete;
    BlockingQueue &operator = (BlockingQueue &&) = delete;

};

template<typename T>
BlockingQueue<T>::BlockingQueue(size_t capacity): capacity_(capacity), wake_(false){
}

template<typename T>
void BlockingQueue<T>::enqueue(T &&item){
    std::unique_lock<std::mutex> lk(mutex_);
    notFull_.wait(lk, [this](){return content_.size() < capacity_;});
    content_.push_back(std::move(item));

    notEmpty_.notify_one();
}

template<typename T>
bool BlockingQueue<T>::tryEnqueue(T &&item){
    std::unique_lock<std::mutex> lk(mutex_);
    if (content_.size() == capacity_)
        return false;
    content_.push_back(std::move(item));

    notEmpty_.notify_one();
    return true;
}

template<typename T>
void BlockingQueue<T>::dequeue(T &item){
    std::unique_lock<std::mutex> lk(mutex_);
    notEmpty_.wait(lk, [this](){return !content_.empty();});
    item = std::move(content_.front());
    content_.pop_front();

    notFull_.notify_one();
}

template<typename T>
bool BlockingQueue<T>::tryDequeue(T &item){
    std::unique_lock<std::mutex> lk(mutex_);
    if (content_.empty())
        return false;
    item = std::move(content_.front());
    content_.pop_front();

    notFull_.notify_one();
    return true;
}

template<typename T>
void BlockingQueue<T>::wait(){
    std::unique_lock<std::mutex> lk(mutex_);
    notEmpty_.wait(lk, [this](){return !content_.empty() || wake_;});
    wake_ = false;
}

template<typename T>
void BlockingQueue<T>::wake(){
    std::unique_lock<std::mutex> lk(mutex_);
    wake_ = true;
    notEmpty_.notify_one();
}

template<typename T>
void BlockingQueue<T>::resize(std::size_t capacity){
    std::unique_lock<std::mutex> lk(mutex_);
    capacity_ = capacity;
    notFull_.notify_one();
}

#endif