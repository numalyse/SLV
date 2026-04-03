#ifndef TSQUEUE_H
#define TSQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

#include <opencv2/core/mat.hpp>

struct ImgData
{
    cv::Mat img;
    int64_t timeMs;
    bool isFinished;
};


template <typename T>
class TSQueue
{
public:
    /// @brief Constructor for a thread safe queue
    /// @details Initialize the queue with a maximum amout of elements, not based on the size of the elements in the queue.
    /// @param maxElemCount Max number of elements allowed in the queue
    TSQueue(size_t maxElemCount) : m_maxElemCount{maxElemCount} {};

    /// @brief Pushes if queue size < max size, otherwise waits until space available
    bool waitPush(T item);

    /// @brief Fills item with the front of queue by using std::move then pop 
    bool waitPop(T& item);
    
    size_t size();
    bool empty();

    /// @brief Call stop when needing to wake sleeping threads 
    void stop();

private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_pushWaitCond; 
    std::condition_variable m_popWaitCond; 
    size_t m_maxElemCount;
    bool m_stop{false};
};


template <typename T>
inline bool TSQueue<T>::waitPush(T item)
{
    std::unique_lock<std::mutex> lock( m_mutex );

    m_pushWaitCond.wait(lock, [this] (){
        return m_queue.size() < m_maxElemCount || m_stop;
    });

    if(m_stop) return false;

    m_queue.push(std::move(item));

    m_popWaitCond.notify_one();
    return true;
}

template <typename T>
inline bool TSQueue<T>::waitPop(T &item)
{
    std::unique_lock<std::mutex> lock( m_mutex );

    m_popWaitCond.wait(lock, [this] (){
        return m_queue.size() > 0 || m_stop;
    });

    if(m_stop) return false;

    item = std::move(m_queue.front());
    m_queue.pop();
    
    m_pushWaitCond.notify_one();
    return true;
}

template <typename T>
inline size_t TSQueue<T>::size()
{
    std::lock_guard<std::mutex> lock (m_mutex);
    return m_queue.size();
}

template <typename T>
inline bool TSQueue<T>::empty()
{
    std::lock_guard<std::mutex> lock (m_mutex);
    return m_queue.empty();
}

template <typename T>
inline void TSQueue<T>::stop()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_stop = true;
    m_pushWaitCond.notify_all(); 
    m_popWaitCond.notify_all();
}

#endif

