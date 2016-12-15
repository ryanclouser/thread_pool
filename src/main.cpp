#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "thread_pool.h"

#include <chrono>

int test_1(int val)
{
	return val;
}

int test_2(std::shared_ptr<int> val)
{
	return *val;
}

int test_3(int val)
{
	std::this_thread::sleep_for(std::chrono::seconds(1));
	return val;
}

TEST_CASE("thread_pool", "[thread_pool]")
{
	thread_pool<int> pool;

	SECTION("constructor thread creation")
	{
		REQUIRE(pool.threads() == std::thread::hardware_concurrency());
	}

	SECTION("stop pool")
	{
		pool.interrupt();

		REQUIRE(pool.threads() == 0);
		REQUIRE(pool.empty());
	}

	SECTION("reset pool")
	{
		pool.interrupt();
		pool.create_threads(4);

		REQUIRE(pool.threads() == 4);
		REQUIRE(pool.empty());

		pool.create_threads(1);

		REQUIRE(pool.threads() == 5);
	}

	SECTION("task schedule")
	{
		int result = 0;
		std::vector<std::future<int> > futures;

		for(int x = 0; x < 128; ++x)
		{
			futures.push_back(pool.push(std::bind(&test_1, 1)));
		}

		for(auto && f : futures)
		{
			result += f.get();
		}

		REQUIRE(result == 128);

		futures.clear();

		for(int x = 0; x < 128; ++x)
		{
			futures.push_back(pool.push(std::bind(&test_2, std::make_shared<int>(2))));
		}

		result = 0;
		for(auto && f : futures)
		{
			result += f.get();
		}

		REQUIRE(result == 256);
		REQUIRE(pool.empty());
	}

	SECTION("task interruption")
	{
		pool.push(std::bind(&test_3, 1));
		pool.interrupt();

		REQUIRE(pool.empty());
		REQUIRE(pool.threads() == 0);
	}

	SECTION("delayed thread start")
	{
		pool.interrupt();
		REQUIRE(pool.empty());
		REQUIRE(pool.threads() == 0);

		int result = 0;
		std::vector<std::future<int> > futures;

		for(int x = 0; x < 128; ++x)
		{
			futures.push_back(pool.push(std::bind(&test_1, 1)));
		}

		REQUIRE(pool.size() == 128);

		pool.create_threads(4);

		for(auto && f : futures)
		{
			result += f.get();
		}

		REQUIRE(result == 128);
	}
}
