#include <QtCore/QCoreApplication>

#include <iostream>
#include <Windows.h>
#include <string>
#include "../Interactive/Interactive.h"
#include <functional>
#include <QLibrary>
#include <QSharedPointer> 
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
typedef QSharedPointer<QLibrary> QLibraryPtr;

using namespace std;

int TestFunc(int a, char c, float f)
{
	cout << a << endl;
	cout << c << endl;
	cout << f << endl;

	return a;
}

void TestBind(std::string str)
{
	std::cout << "111\n" << str;
}

std::function<void()> g_c;
void SetFunc(std::function<void()> func)
{
	g_c = func;
}

template<class F, class... Args>
auto enqueue(F&& f, Args&&... args)
->std::future<typename std::result_of<F(Args...)>::type>;
template<class F, class... Args> 
auto enqueue(F&& f, Args&&... args)-> std::future<typename std::result_of<F(Args...)>::type>
{
	using return_type = typename std::result_of<F(Args...)>::type;

	auto task = std::make_shared< std::packaged_task<return_type()> >(
		std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);

	std::future<return_type> res = task->get_future();
	{
		g_c = [task]() { (*task)(); };
	}
	return res;
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	enqueue(TestBind, "22");

	g_c();
	int x = 0;
	std::cin >> x;
	return a.exec();
}
