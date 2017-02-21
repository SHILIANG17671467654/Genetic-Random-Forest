#include "tree.h"
#include "init_tree.h"
#include "calc_element.h"
#include <iostream>
#include <armadillo.h>
#include "algebraics.h"
#include <iomanip>

using namespace std;
using namespace arma;

//constructor creates root node
tree::tree()
{
	root = new node;
	num_nodes += 1;
	root->id = num_nodes;
	depth += 1;
}


tree::~tree()
{
	destroy_tree();
}

void tree::destroy_tree()
{
	destroy_tree(root);
}

//destroys the tree starting from a given node
void tree::destroy_tree(node *leaf)
{
	if (leaf != NULL)
	{
		destroy_tree(leaf->left);
		destroy_tree(leaf->right);
		delete(leaf);
	}
}

//recursive function to calculate the value for given data-point
double tree::get_value(mat const &input, node *leaf)
{
	if (leaf->left != NULL)
	{
		if (input.at(0, leaf->feature_num) != input.at(0, leaf->feature_num) | input.at(0, leaf->feature_num) <= leaf->value)
			return get_value(input, leaf->left);
		else
			return get_value(input, leaf->right);

	}
	else
		return leaf->term_value;
}

//starts get_value above from the root node
double tree::get_value(mat const &input)
{
	return get_value(input, root);
}

//Prints the tree + the number of nodes
void tree::print_tree()
{
	print_tree(root);
	cout << "\nNumber of nodes: " << num_nodes << "\n" << endl;

}

//recursive function to print the tree
void tree::print_tree(node *leaf, unsigned start)
{
	if (leaf != NULL)
	{
		unsigned begin = start;
		for (int i = 0; i < begin; ++i)
			cout << " ";

		if (leaf->left != NULL)
		{
			cout << "ID: " << leaf->id << " | ";
			cout << "X" << leaf->feature_num + 1 << " <= " << std::fixed << std::setprecision(5) << leaf->value << " | ";
			cout << "Data Passed: " << leaf->data_passed << " | ";
			cout << "Error in Node: " << leaf->node_MSE << endl;
		}
		else
		{
			cout << "ID: " << leaf->id << " | ";
			cout << "=>" << std::fixed << std::setprecision(5) << leaf->term_value << " | ";
			cout << "Data Passed: " << leaf->data_passed << " | ";
			cout << "Error in Node: " << leaf->node_MSE << endl;

		}

		begin += 1;

		print_tree(leaf->left, begin);
		print_tree(leaf->right, begin);
	}


}

//calculates the predictions for a new data-matrix
mat tree::new_values(mat const &input)
{
	mat result(input.n_rows, 1);

	for (unsigned i = 0; i < input.n_rows; ++i)
	{
		result.at(i, 0) = get_value(input.row(i));
	}

	return result;
}

//calculates the error for a new data-matrix
double tree::new_error(mat const &X, mat const &y)
{
	mat errors = pow(y - new_values(X), 2);

	return mean(errors.col(0));
}
