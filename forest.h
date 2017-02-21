#pragma once
#include "tree.h"
#include "genetic.h"
#include "algebraics.h"
#include "bootstrap.h"
#include "calc_element.h"
#include "init_tree.h"
#include <iostream>
#include <C:\Users\sarem\Documents\cpp_libs\armadillo-7.600.1\include\armadillo.h>

using namespace arma;


class random_forest
{
public:
	~random_forest();

	std::vector<genalg*> tree_genes;
	std::vector<data_pointer*> bootstr_data;
	std::vector<std::vector<unsigned>> features;

	mat one_weights;
	mat current_weights;

	void plant_forest(unsigned no_genetics = 10, double bootstr_size = 1);
	void grow_forest(double threshold, unsigned generations = 100,
					unsigned num_trees = 10, unsigned max_depth = 4, unsigned no_elite = 1, unsigned no_extra = 1);

	//void check_trees(unsigned tree_num);
	mat predict_one(mat const &X_mat, unsigned tree);

	mat forest_predict(mat const &X_mat);

	mat OL(mat const &new_data);

	mat create_weigths(unsigned tree);

	//double forest_error(mat const &X_mat, mat const &y_mat);
};