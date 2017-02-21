#pragma once
#include <iostream>
#include <vector>

struct node
{
	unsigned id;

	unsigned feature_num = 0;
	unsigned data_passed = 0;

	std::vector <double> y_values;
	double value = 0;
	double term_value = 0.0;

	double node_MSE = 0.0;


	node *left = NULL;
	node *right = NULL;

};
