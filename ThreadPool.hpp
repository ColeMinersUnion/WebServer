/*
This is a class to handle how threads are managed.
*/
#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

//Libraries!
#include <thread> //The API that I will use to create threads. 
#include <vector> //A container that will hold the threads. 
#include <queue> //A container that will hold the tasks. 
#include <mutex> //A lock that will be used to protect the queue.
#include <condition_variable>   //A condition variable that will be used to notify the threads that there is work to be done.


//I WILL ABIDE BY GOOD OBJECT ORIENTED PROGRAMMING PRACTICES RAH
//Here's a class to create, manage, and destroy threads.
class ThreadPool {
    public:
        //Constructor. Initializes the thread pool with the specified number of threads.
        ThreadPool(size_t);

        //Enqueues a task to be executed. 
        //This will be used to add requests to the queue such that
        //The threads can process them.
        template<class F>
        void enqueue(F&& f) {
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                tasks.emplace(std::forward<F>(f));
            }
            condition.notify_one();
        }

        //Destructor. Safely destroys the thread pool.
        ~ThreadPool();
    private:
        //A place to store the threads.
        std::vector<std::thread> workers;

        //A queue to store the tasks.
        std::queue<std::function<void(int)>> tasks;
        
        //A mutex to protect the queue and prevent race conditions
        std::mutex queue_mutex;

        //A condition variable to notify the threads that there is work to be done.
        std::condition_variable condition;

        //A boolean to stop the threads.
        bool stop;
};

#endif //THREADPOOL_HPP
