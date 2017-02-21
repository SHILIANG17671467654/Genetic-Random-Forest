#pragma once
#include "genetic.h"
#include "data_loader.h"
#include "init_tree.h"
#include <string>



void run_algorithm(double error_threshold, string train_file, string test_file, unsigned num_trees = 10, unsigned maxdepth = 20, unsigned bootstrap_size = 300,
	unsigned min_node_size = 10, unsigned no_elite = 1, unsigned no_extra = 1, unsigned ensemble_size = 200, double cor_threshold=0.75,
	bool print = true, double max_duration = 300);

double get_cor(mat const &ensemble, mat const &new_pred, mat &const y_mat, unsigned const &cols);