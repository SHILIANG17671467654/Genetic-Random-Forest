#include "algebraics.h"



double vec_mean(std::vector<double> const &input)
{
	double sum = 0;
	for (int i = 0; i < input.size(); ++i)
		sum += input[i];

	return (sum / input.size());
}

double vec_sum(std::vector<double> input)
{
	double sum = 0;
	for (int i = 0; i < input.size(); ++i)
		sum += input[i];

	return sum;
}

double vec_median(std::vector<double> input)
{
	double median;
	size_t size = input.size();

	sort(input.begin(), input.end());

	if (size % 2 == 0)
	{
		median = (input[size / 2 - 1] + input[size / 2]) / 2;
	}
	else
	{
		median = input[size / 2];
	}

	return median;
}