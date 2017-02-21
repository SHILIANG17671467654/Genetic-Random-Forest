#pragma once
#include <iostream>
#include "node.h"
#include "calc_element.h"
#include <armadillo.h>

using namespace arma;
using namespace std;

//tree class
class tree
{
public:
	tree();
	~tree();


	unsigned id;

	//tracks the number of nodes within and the depth of the tree
	unsigned num_nodes = 0;
	unsigned depth = 0;


	std::vector<double> node_MSEs;

	//root node
	node *root;

	double get_value(mat const &input);
	double get_value(mat const &input, node *leaf);

	void destroy_tree();
	void destroy_tree(node *leaf);

	void print_tree();

	mat new_values(mat const &input);

	double new_error(mat const &X, mat const &y);

private:
	void print_tree(node *leaf, unsigned start = 0);

};

