#include<iostream>
#include<thread>
#include <string>
#include <mutex>
#include <chrono>
#include <queue>
#include <condition_variable>
#include <functional>
#include <assert.h>
using namespace std;
const int task_size = 5;

queue<int> tasks;
condition_variable c;
mutex mtx;
void producer(int id)
{
	for (int i = 0; i < 10; i++) {
		unique_lock<mutex> lock(mtx);
		c.wait(lock, []()->bool {return tasks.size() < task_size; });
		int data = i;
		tasks.push(data);
		cout << "Producer : " << id << " data : " << data << endl;
		c.notify_one();
		lock.unlock();
		this_thread::sleep_for(chrono::milliseconds(1000));
	}
}

void consumer(int id)
{
	for (int i = 0; i < 10;i++) {
		unique_lock<mutex> lock(mtx);
		c.wait(lock, []()->bool {return !tasks.empty(); });
		int data = tasks.front();
		cout << "Consumer : " << id << " data : " << data << endl;
		tasks.pop();
		lock.unlock();
		c.notify_one();
		this_thread::sleep_for(chrono::milliseconds(100));
	}
}

int main()
{
	thread pro[2];
	thread con[3];
	for (int i = 0; i < 2; i++)
		pro[i] = thread(producer,i);
	for (int i = 0; i < 3; i++)
		con[i] = thread(consumer, i);

	for (int i = 0; i < 2; i++) {
		if(pro[i].joinable())
			pro[i].join();
	}
	for (int i = 0; i < 3; i++) {
			con[i].join();
	}

	return 0;
}
