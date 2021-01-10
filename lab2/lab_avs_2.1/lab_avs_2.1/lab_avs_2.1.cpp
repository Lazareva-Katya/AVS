#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>

using namespace std;

const int NumTasks = 1024 * 1024;
int counter1 = 0;
atomic <unsigned int> counter2 = 0;
mutex m;

void mutex_func(int8_t* arr) 
{
	int counter = 0;
	while (counter < NumTasks)
	{
		m.lock();
		counter = counter1++;
		m.unlock();
		if (counter < NumTasks)
			arr[counter]++;
		else
			break;
	}
}

void atomic_func(int8_t* arr)
{
	int counter = 0;
	while (counter < NumTasks)
	{
		counter = counter2++;
		if (counter < NumTasks) 
			arr[counter]++;
		else
			break;
	}
}

void mutex_sleep(int8_t* arr)
{
	int counter = 0;
	while (counter < NumTasks)
	{
		m.lock();
		counter = counter1++;
		m.unlock();
		if (counter < NumTasks)
		{
			arr[counter]++;
			this_thread::sleep_for(chrono::nanoseconds(10));
		}
		else
			break;
	}
}

void atomic_sleep(int8_t* arr)
{
	int counter = 0;
	while (counter < NumTasks)
	{
		counter = counter2++;
		if (counter < NumTasks)
		{
			arr[counter]++;
			this_thread::sleep_for(chrono::nanoseconds(10));
		}
		else
			break;
	}
}

void show(int NumThreads, float duration)
{
	cout << "threads: " << NumThreads << " time: " << duration << "\n\n";
}

void action(thread* arr_t, int8_t* arr, unsigned int NumThreads, bool flag, bool sleep)
{
	if (flag == true && sleep == false)
	{
		cout << "mutex:   ";
	}
	else if (flag == false && sleep == false)
	{
		cout << "atomic:   ";
	}
	else if (flag == true && sleep == true)
	{
		cout << "mutex sleep:   ";
	}
	else
		cout << "atomic sleep:   ";

	for (unsigned int i = 0; i < NumThreads; i++)
	{
		if (flag == true && sleep == false)
		{
			arr_t[i] = thread(mutex_func, arr);
		}
		else if (flag == false && sleep == false)
		{
			arr_t[i] = thread(atomic_func, arr);
		}
		else if (flag == true && sleep == true)
		{
			arr_t[i] = thread(mutex_sleep, arr);
		}
		else
			arr_t[i] = thread(atomic_sleep, arr);
		arr_t[i].join();
	}
}

void act(int8_t* arr, thread* arr_t, unsigned int NumThreads, int switcher)
{
	chrono::high_resolution_clock::time_point first;
	chrono::high_resolution_clock::time_point second;
	chrono::duration<float> duration;
	first = chrono::high_resolution_clock::now();
	switch (switcher)
	{
	case 0: action(arr_t, arr, NumThreads, true, false);
		break;
	case 1: action(arr_t, arr, NumThreads, false, false);
		break;
	case 2: action(arr_t, arr, NumThreads, true, true);
		break;
	case 3: action(arr_t, arr, NumThreads, false, true);
	default:
		break;
	}
	second = chrono::high_resolution_clock::now();
	duration = second - first;
	show(NumThreads, duration.count());
	counter1 = 0;
	counter2 = 0;
}

int8_t activity(int8_t* arr, unsigned int NumThreads)
{
	thread* arr_t = new thread[NumThreads];
	act(arr, arr_t, NumThreads, 0);
	act(arr, arr_t, NumThreads, 1);
	act(arr, arr_t, NumThreads, 2);
	act(arr, arr_t, NumThreads, 3);
	return *arr;
}

int main()
{
	int NumTreads[] = { 4, 8, 16, 32 };
	int8_t* arr = new int8_t[NumTasks];

	for (int i = 0; i < 4; i++)
		activity(arr, NumTreads[i]);
}