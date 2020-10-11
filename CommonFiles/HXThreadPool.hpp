#ifndef THREAD_POOL_H 
#define THREAD_POOL_H
#include <condition_variable>
#include <vector>
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
	HXThreadPool(size_t size);
	~HXThreadPool();
	template<class F, typename ... Args>
	decltype(auto) Enqueue(F&&, Args&& ... args); //ͨ������ת���������������
private:
	std::vector<std::thread> workers; //�����������߳�
	std::queue<std::function<void()>> tasks; //�̳߳ص��������
	std::condition_variable cond_;
	std::mutex mutex_;
	bool stop;
};

inline HXThreadPool::HXThreadPool(size_t size) :stop(false)
{
	for (int i = 0; i < size; i++)
	{
		workers.emplace_back([this]()
		{
			while (true) //��ѭ���ȴ�
			{
				std::function<void()> task;
				{
					//�ٽ�����ʼ �ȴ��̳߳ص����ٻ���������Ĵ���
					std::unique_lock<std::mutex> lock(mutex_);
					cond_.wait(lock, [this] { return this->stop || !this->tasks.empty(); }); //����������wait���� ��ֹ��ٻ���

					if (this->stop && this->tasks.empty())
					{
						return; //�̳߳����� ����ѭ��
					}

					task = std::move(this->tasks.front()); //�����е�ͷ���ƶ���task
					this->tasks.pop(); //�ǵó�����

					//�ٽ������� �Զ��ͷ��� mutex_
				}
				task(); //ִ������
			}
		});
	}
}

inline HXThreadPool::~HXThreadPool()
{
	{
		//�ٽ�����ʼ  ��סstop ���丳ֵΪtrue ֪ͨ�߳��˳�ѭ��
		std::unique_lock<std::mutex> lock(mutex_);
		stop = true;
		//�ٽ������� �Զ��ͷ��� mutex_
	}
	cond_.notify_all();//֪ͨ���е��߳��˳�ѭ��
	for_each(workers.begin(), workers.end(), std::mem_fn(&std::thread::join)); //�ȴ�
}

//ԭ���ĺ������÷���ֵβ������ȡ���� ��C++14֮��֧��decltype(auto)�������͵��Ƶ�������Ϥdecltype�Ƶ�������Կ���Effective Modern C++��
template<class F, typename ...Args>
decltype(auto) HXThreadPool::Enqueue(F&& f, Args && ...args)
{
	using return_type = typename std::invoke_result_t<F, Args...>; //��ȡ����ֵ����

	auto task = std::make_shared< std::packaged_task<return_type()> >(
		std::bind(std::forward<F>(f), std::forward<Args>(args)...) //ͨ��bind������ת���������Ͳ�����
		);

	std::future<return_type> res = task->get_future();

	{
		//�ٽ�����ʼ ���������
		std::unique_lock<std::mutex> lock(mutex_);

		//����Ѿ����̳߳�����,�Ͳ������������
		if (stop)
			throw std::runtime_error("enqueue on stopped ThreadPool");

		tasks.emplace([task]() { (*task)(); });
	}
	cond_.notify_one(); //֪ͨ����һ���߳̽�������
	return res;
}

#endif // THREAD_POOL_H
