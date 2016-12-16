#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

#pragma once

#include <thread>
#include <mutex>
#include <memory>
#include <future>
#include <vector>
#include <queue>
#include <condition_variable>

template<class R>
class thread_pool
{
private:

	struct work
	{
		std::promise<R> promise;
		std::function<R()> f;

		work(std::function<R()> && func) : f(func) { }
	};

	mutable std::mutex pool_m, work_m;
	std::condition_variable cv;

	bool interrupted;

	std::vector<std::thread> pool;
	std::queue<std::unique_ptr<work> > queue;

	/**
	 * Worker thread function
	 */
	void work_thread()
	{
		for(;;)
		{
			std::unique_lock<std::mutex> lock(work_m);

			cv.wait(lock, [&] { return !queue.empty() || interrupted; });

			if(interrupted)
			{
				break;
			}

			auto w = std::move(queue.front());
			queue.pop();

			lock.unlock();

			try
			{
				w->promise.set_value(w->f());
			}
			catch(...)
			{
				w->promise.set_exception(std::current_exception());
			}
		}
	}

public:

	/**
	 * Constructor
	 *
	 * @param count Number of threads to start
	 */
	thread_pool(size_t count = std::thread::hardware_concurrency()) : interrupted(false)
	{
		create_threads(count);
	}

	/**
	 * Destructor
	 */
	~thread_pool()
	{
		interrupt();
	}

	thread_pool(const thread_pool &) = delete;
	thread_pool & operator=(const thread_pool &) = delete;

	/**
	 * @return Size of the work queue
	 */
	size_t size() const
	{
		std::lock_guard<std::mutex> lock(work_m);
		return queue.size();
	}

	/**
	 * Determines if the work queue is empty
	 */
	bool empty() const
	{
		std::lock_guard<std::mutex> lock(work_m);
		return queue.empty();
	}

	/**
	 * @return Size of the thread pool
	 */
	size_t threads() const
	{
		std::lock_guard<std::mutex> lock(pool_m);
		return pool.size();
	}

	/**
	 * Adds work to the queue
	 *
	 * @param f Function
	 * @return Future from the promise or an invalid future if there are no threads in the pool
	 */
	std::future<R> push(std::function<R()> && f)
	{
		std::unique_lock<std::mutex> lock(work_m);

		auto w = std::make_unique<work>(std::move(f));
		auto future = w->promise.get_future();
		queue.push(std::move(w));

		lock.unlock();
		cv.notify_one();

		return future;
	}

	/**
	 * Clears all work from the queue
	 */
	void clear()
	{
		std::lock_guard<std::mutex> lock(work_m);
		queue = std::move(std::queue<std::unique_ptr<work> >());
	}

	/**
	 * Adds more threads to the work pool
	 *
	 * @param count Number of threads to start
	 */
	void create_threads(size_t count)
	{
		std::unique_lock<std::mutex> lock(pool_m);

		for(size_t x = 0; x < count; ++x)
		{
			pool.push_back(std::move(std::thread(std::bind(&thread_pool::work_thread, this))));
		}

		lock.unlock();

		if(!empty())
		{
			cv.notify_all();
		}
	}

	/**
	 * Stops all threads
	 */
	void interrupt()
	{
		std::lock(pool_m, work_m);

		std::lock_guard<std::mutex> lock_1(pool_m, std::adopt_lock);
		std::unique_lock<std::mutex> lock_2(work_m, std::adopt_lock);

		queue = std::move(std::queue<std::unique_ptr<work> >());

		interrupted = true;
		cv.notify_all();

		lock_2.unlock();

		for(auto && t : pool)
		{
			if(t.joinable())
			{
				t.join();
			}
		}

		pool.clear();

		lock_2.lock();

		interrupted = false;
	}
};

#endif // THREAD_POOL_H_
