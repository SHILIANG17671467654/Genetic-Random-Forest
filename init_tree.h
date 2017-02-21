#pragma once
#include "tree.h"
#include "data_loader.h"
#include <armadillo.h>
#include <random>
#include "bootstrap.h"


using namespace std;
using namespace arma;

//Contains functions to create and manipulate the trees

void set_node(node *leaf, std::vector<unsigned> const &features, data_pointer const &data);

void add_nodes(node *leaf, tree *dtree, unsigned max_depth, unsigned current_depth, std::vector<unsigned> const &features,
	data_pointer const &data);

void init_tree(tree *temp, std::vector<unsigned> const &features, data_pointer const &data, unsigned max_depth = 5);

void deep_copy(node *leaf_child, node *leaf_par);

unsigned count_depth(node *leaf, unsigned current_depth = 1);

void set_ids_norand(node *leaf, tree *dtree);

void mean_ys(node *leaf, mat input, double y);

void mean_ys(tree *dtree, mat input, mat y, unsigned minbucket);

void middle_ys(node *leaf, tree *dtree, unsigned minbucket);

std::vector<unsigned> random_features(mat const &X_mat_b);