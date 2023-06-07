#include <iostream>
#include "mThreadPool.h"

struct fun
{
	int x;
	fun(int _x) : x(_x) {}
	void operator()()
	{
		std::cout << x << std::endl;
	}
};

void prt()
{
	std::cout << "hello" << std::endl;
}

int main()
{
	mThreadPool pl(2);
	for (int i = 0; i < 30; ++i)
		pl.addtask(prt);
	pl.start();
	for (int i = 0; i < 30; ++i)
		pl.addtask(prt);
	pl.shutdown();

	std::cout << pl.exc_num << std::endl;

	return 0;
}