Simple C++14 Thread Pool
===

[![Build Status](https://travis-ci.org/ryanclouser/thread_pool.svg?branch=master)](https://travis-ci.org/ryanclouser/thread_pool)
[![Donate with Bitcoin](https://en.cryptobadges.io/badge/micro/12BMo7nBeBhDGDGagwqSRPAv3fkQi8nCfq)](https://en.cryptobadges.io/donate/12BMo7nBeBhDGDGagwqSRPAv3fkQi8nCfq)
[![Donate with Ethereum](https://en.cryptobadges.io/badge/micro/0xd163fdde358f9000A4E9290f23B84DFb6E9190D3)](https://en.cryptobadges.io/donate/0xd163fdde358f9000A4E9290f23B84DFb6E9190D3)
[![Donate with Litecoin](https://en.cryptobadges.io/badge/micro/LVSmZByqa6Cp1BFwgqeUyMjKmpfHP23ApR)](https://en.cryptobadges.io/donate/LVSmZByqa6Cp1BFwgqeUyMjKmpfHP23ApR)

Creates an arbitrary number of threads for which work can be done on. Certain tasks can be completed asynchronously for which a thread pool can be used for. This particular implementation uses the future/promise model added in C++11. A C++14 compatible compiler is required for `make_unique`.

Usage
---

```
#include "thread_pool.h"

int my_work_function(int arg)
{
	return arg;
}

// Create the thread pool. By default the thread count is the hardware concurrency value.
thread_pool<int> pool;

// Create some work. Pass any function arguments to the bind call.
std::future<int> future = pool.push(std::bind(&my_work_function, 1));

// Wait for the work to be completed
int result = future.get();
```

If the thread pool object goes out of scope, all threads will be stopped.

License
---

MIT
