#pragma once
#include "algebraics.h"
#include "data_loader.h"


using namespace std;
using namespace arma;

std::vector<unsigned> sample_without_replacement(unsigned pop_size, unsigned sample_size);

//structure to hold the bootstrapped data - I chose to give it an own struct in case of running several 
//GAs in parallel each with different data

struct data_pointer
{
	data_pointer();
	~data_pointer();

	mat X_mat_b;

	mat y_mat_b;


	unsigned rows_b;
	unsigned X_cols_b;

	void print_X_b();
	void print_y_b();

	void create_bootstrap(unsigned size);
};


