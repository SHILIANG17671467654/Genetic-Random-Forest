#include "data_loader.h"

using namespace arma;


mat X_mat;


mat y_mat;


unsigned rows;
unsigned X_cols;

//This loads a csv-file without header and assigns columns "y" as endogenous variable
void load_data(string filename, unsigned y = 0)
{
	mat temp_mat;
	temp_mat.load(filename);

	y_mat.set_size(temp_mat.n_rows, 1);
	y_mat.col(0) = temp_mat.col(y);


	X_mat.set_size(temp_mat.n_rows, temp_mat.n_cols - 1);

	unsigned checker = 0;
	for (int i = 0; i < temp_mat.n_cols; ++i)
	{
		if (i != y)
		{
			X_mat.col(checker) = temp_mat.col(i);
			checker += 1;
		}
	}

	rows = y_mat.n_rows;
	X_cols = X_mat.n_cols;
}

//functions to print the data
void print_X()
{
	cout << "X-Matrix:" << endl;
	cout << X_mat << endl;
}

void print_y()
{
	cout << "y-Matrix:" << endl;
	cout << y_mat << endl;
}