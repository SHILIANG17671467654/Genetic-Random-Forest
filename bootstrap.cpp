#include "bootstrap.h"
#include <random>


using namespace arma;
std::default_random_engine generator3(time(NULL));


//Currently not in use - sampling without replacement did not seem to improve the results
/*std::vector<unsigned> sample_without_replacement(unsigned pop_size, unsigned sample_size)
{
std::vector <unsigned> samples;
for (unsigned i = 0; i < pop_size; ++i)
samples.push_back(i);

unsigned &n = sample_size;
unsigned &N = pop_size;

unsigned t = 0;
unsigned m = 0;
double u;

while (m < n)
{
std::uniform_real_distribution<double> uni(0, 1);
u = uni(generator3);

if ((N - t)*u >= n - m)
{
t++;
}
else
{
samples[m] = t;
t++; m++;
}
}
return samples;
}*/





data_pointer::data_pointer()
{

	X_mat_b = X_mat;
	y_mat_b = y_mat;


	rows_b = rows;
	X_cols_b = X_cols;
}

//Function to create the pseudo-bootstrap
void data_pointer::create_bootstrap(unsigned size)
{
	rows_b = size;

	X_mat_b.set_size(size, X_cols);
	y_mat_b.set_size(size, 1);
	uniform_int_distribution<int> row_select(0, (rows - 1));

	//std::vector<unsigned> samples = sample_without_replacement(rows, size);

	std::uniform_int_distribution<unsigned> uni(0, rows - 1);

	for (int i = 0; i < size; ++i)
	{
		//unsigned row = samples[i];
		unsigned row = uni(generator3);


		y_mat_b.at(i, 0) = y_mat.at(row, 0);
		X_mat_b.row(i) = X_mat.row(row);
	}

}

data_pointer::~data_pointer()
{
}

//to print the resampled data
void data_pointer::print_X_b()
{
	cout << "X-Matrix Bootstrapped:" << endl;
	cout << X_mat_b << endl;
}

void data_pointer::print_y_b()
{
	cout << "y-Matrix Bootstrapped:" << endl;
	cout << y_mat_b << endl;
}