// SET(CMAKE_CXX_FLAGS "-fcoroutines -std=c++2a")

#include <iostream>
#include <thread>
#include <coroutine>
#include <chrono>
#include <functional>

using call_back = std::function<void(int)>;
void Add100ByCallback(int init, call_back f) // 异步调用
{
	std::thread t([init, f]()
	{
		std::this_thread::sleep_for(std::chrono::seconds(3));
		f(init + 100);
	});
	t.detach();
}

struct Add100AWaitable
{
	Add100AWaitable(int init)
			:init_(init)
	{
	}

	/**
	 * 返回 awaitable是否已经ready
	 * @return
	 */
	bool await_ready() const
	{
		return false; // 返回true说明结果已经得到 不需要执行协程了
	}
	/**
	 * 挂起 awaitable 通过handle.resume() 恢复协程
	 * @param handle
	 */
	void await_suspend(std::coroutine_handle<> handle)
	{
		auto f = [handle, this](int value) mutable
		{
			result_ = value;
			handle.resume();
		};
		Add100ByCallback(init_, f); // 调用原来的异步调用
	}
	/**
	 * 协程恢复后 会调用此函数 返回结果即为co_wait的返回值
	 * @return
	 */
	int await_resume()
	{
		return result_;
	}
	int init_; // Add100ByCallback的参数
	int result_; // Add100ByCallback的结果
};

/**
 * 最简单的Promise规范的类型
 */
struct Task
{
	struct promise_type
	{
		auto get_return_object()
		{
			return Task{};
		}
		auto initial_suspend()
		{
			return std::suspend_never{};
		}
		auto final_suspend()
		{
			return std::suspend_never{};
		}
		void unhandled_exception()
		{
			std::terminate();
		}
		void return_void()
		{
		}
	};
};

/**
 * 协程的入口函数 必须是在某个函数中. 函数的返回值需要满足Promise规范
 * @return
 */
Task Add100ByCoroutine(int init, call_back f)
{
	/**
	 * 协程可以解放异步函数的组织
	 * 否则多个异步回调实现同步 需要嵌套调用
	 * int ret = 0;
	 * Add100ByCallback(5, [&](int value)
	 * {
	 * 		ret = value;
	 * 		Add100ByCallback(ret, [&](int value)
	 * 		{
	 * 			ret += value;
	 * 		});
	 * });
	 */
	int ret = co_await Add100AWaitable(init); // 连续调用co_await
	ret = co_await Add100AWaitable(ret); // 将多个异步调用转换为串行化的同步调用

	f(ret);
}

int main()
{
	Add100ByCallback(5, [](int value)
	{ std::cout << "get result: " << value << "\n"; });
	Add100ByCoroutine(10, [](int value) // 启动协程
	{ std::cout << "get result from coroutine1: " << value << "\n"; });
	Add100ByCoroutine(20, [](int value) // 启动协程
	{ std::cout << "get result from coroutine2: " << value << "\n"; });
}