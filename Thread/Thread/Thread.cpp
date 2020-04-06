#include <iostream>
#include <Windows.h>
#include <ctime>

static long num_steps = 10000000000;

double step, pi;

void main()

{
	int i;

	double x, sum = 0.0;

	step = 1.0 / (double)num_steps;
	clock_t t = clock();
	for (i = 0; i < num_steps; i++) {
		x = (i + 0.5) * step;

		sum = sum + 4.0 / (1.0 + x * x);
	}

	pi = step * sum;
	t = clock() - t;
	printf_s("Pi = % f\n", pi);
	printf("%f", (double)t / CLOCKS_PER_SEC);
}