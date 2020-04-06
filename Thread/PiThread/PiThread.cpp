// PiThread.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <ctime>

static long num_steps = 10000000000;
double global_sum, step, pi;
float local_sum[8];

DWORD WINAPI calFunc(LPVOID start) {
	int* s = (int*)start;
	int i;
	double x;
	local_sum[*s] = 0.0;
	for (i = *s; i < num_steps; i += 8) {
		x = (i + 0.5) * step;
		local_sum[*s] = local_sum[*s] + 4.0 / (1.0 + x * x);
	}

	return 0;
}

int main()
{
	global_sum = 0.0;
	step = 1.0 / (double)num_steps;
	int s[8];
	HANDLE threads[8];
	clock_t t = clock();
	for (int i = 0; i < 8; i++) {
		s[i] = i;
		threads[i] = CreateThread(0, 0, calFunc, &s[i], 0, 0);
	}
	WaitForMultipleObjects(8, threads, true, INFINITE);
	for (int i = 0; i < 8; i++) {
		global_sum += local_sum[i];
	}
	pi = global_sum * step;
	t = clock() - t;

	printf_s("%f\n", pi);
	printf("%f", (double)t / CLOCKS_PER_SEC);
}