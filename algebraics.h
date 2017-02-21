#pragma once
#include<vector>
#include <armadillo.h>

using namespace std;
using namespace arma;

//some functions for c++ vector algebra - median is not used right now

double vec_mean(std::vector<double> const &input);

double vec_sum(std::vector<double> input);

double vec_median(std::vector<double> input);
