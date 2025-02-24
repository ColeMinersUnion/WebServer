//Implementation of the ThreadPool class
#include "ThreadPool.hpp"

//Constructor. Initializes the thread pool with the specified number of threads.
ThreadPool::ThreadPool(size_t num_threads) {
    for (size_t i = 0; i < num_threads; ++i) {
        workers.emplace_back([this] {
            while (true) {
                std::unique_lock<std::mutex> lock(queue_mutex);
                condition.wait(lock, [this] { return !tasks.empty() || stop; });
                if (stop && tasks.empty()) return;
                auto task = std::move(tasks.front());
                tasks.pop();
                lock.unlock();
                task();
            }
        });
    }
}

template<class F>
void ThreadPool::enqueue(F&& f) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.emplace(std::forward<F>(f));
    }
    condition.notify_one();
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread& worker : workers) {
        worker.join();
    }
}