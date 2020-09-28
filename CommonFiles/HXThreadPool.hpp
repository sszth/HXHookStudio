#pragma once
// 使用C++17实现线程池
#include <condition_variable>
#include <list>
#include <Thread>
#include <type_traits>
#include <queue>
#include <functional>
#include <mutex>
#include <algorithm>
#include <memory>
#include <future>
class HXThreadPool
{
public:
	HXThreadPool(size_t size):m_stop(false)
	{
		for (size_t i = 0; i < size; i++)
		{
			m_workers.emplace_back(
				[this]()
			{
				while (true)
				{
					std::function<void()> task;
					// 为了防止线程退出过快导致返回空线程句柄
					{
						std::unique_lock<std::mutex> lock(m_mutex);
						m_cond.wait(lock, [this] {return this->m_stop || !this->m_tasks.empty(); });//条件变量的wait函数 防止虚假唤醒
						if (this->m_stop && this->m_tasks.empty())
						{
							return;//线程池销毁 跳出循环
						}
						task = std::move(this->m_tasks.front());
						this->m_tasks.pop(); //记得出队列
					}
					task();
				}
			}
			);
		}
	}

	~HXThreadPool()
	{
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_stop = true;
		}
		m_cond.notify_all();
		for_each(m_workers.begin(), m_workers.end() std::mem_fn(&std::thread::join));
	}
	template<class F,typename ... Args>
	decltype(auto) Enqueue(F&&, Args&& ... args)
	{
		//获取返回值类型
		using return_type = typename std::result_of_t<f(Args...)>;
		auto task = std::make_shared<std::packaged_task<return_type()> >(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)//通过bind和完美转发将函数和参数绑定
			);
		std::future<return_type> res = task->get_future();
		{
			std::unique<std::mutex> > lock(m_mutex);
			if (m_stop)
			{
				throw std::runtime_error(L"enqueue on stopped ThreadPool");
			}
			m_tasks.emplace([task]() {(*task)(); });
		}
		m_cond.notify_all();
		return res;
	}
	
private:
	std::list<std::thread> m_workers;	//工作线程
	std::queue< std::function<void> > m_tasks; //线程池的任务队列
	std::condition_variable	m_cond;
	std::mutex	m_mutex;
	volatile bool m_stop;
};