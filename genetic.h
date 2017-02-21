#pragma once
#include "algebraics.h"
#include "calc_element.h"
#include <iostream>
#include <armadillo.h>
#include "tree.h"
#include "bootstrap.h"



struct tree_soil;

//The class for the genetic algorithm

class genalg
{
public:
	//manual constructor, since the tree structure is composed of pointers that need to be deleted when not needed anymore
	~genalg();

	//This vector will hold the struct for the trees and their corresponding errors
	std::vector<tree_soil*> forest;

	//another vector for the errors - this makes them more easily accessible from outside functions
	std::vector<double> MSEs;

	//see .cpp-file for the class-functions
	void init_alg(std::vector<unsigned> const &features, data_pointer &data,
		unsigned num_trees = 50, unsigned maxdepth = 5, unsigned minbucket = 5, unsigned boot_size = 300);


	unsigned draw_prob_min(std::vector<double> const &error);
	unsigned draw_prob_max(std::vector<double> const &error);
	void order_matrix();
	void new_generation(data_pointer &data, std::vector<unsigned> const &features,
		unsigned num_trees = 50, unsigned maxdepth = 5, unsigned no_elite = 5, unsigned no_new = 5, unsigned minbucket = 5,
		unsigned boot_size = 300);

	void node_copy_from(node *leaf, node *leaf_one, int id, bool start = false);
	void node_copy_to(node *leaf, node *leaf_one, node *leaf_two, int id, int id_2);

	void del_node(tree_soil *soil);
	void del_node(node *leaf, unsigned id, tree *dtree);

	void mut_node(tree_soil *soil, std::vector<unsigned> const &features, data_pointer const &data);
	void mut_node(node *leaf, unsigned id, std::vector<unsigned> const &features, data_pointer const &data);

	double calc_new(mat const &X_mat);

	void print_best();

	//void print_best_newdata(data_pointer const &data, mat const &weights);

	double new_error(mat const &X, mat const &y);

	tree* getbest();

	//void cut_node(node *leaf, unsigned min_size, tree *dtree);

	mat calc_new_data(mat const &X_mat);

	//could have used a struct instead
private:

};
