#include "init_tree.h"
#include "algebraics.h"
#include <armadillo.h>


using namespace arma;

std::default_random_engine generator(time(NULL));

//Randomly selects a feature and a value to split on in a given node
void set_node(node *leaf, std::vector<unsigned> const &features, data_pointer const &data)
{

	std::uniform_int_distribution<int> unif(0, features.size() - 1);

	leaf->feature_num = features[unif(generator)];


	std::uniform_int_distribution<int> univ(0, data.X_mat_b.n_rows - 1);

	do
	{
		leaf->value = data.X_mat_b.at(univ(generator), leaf->feature_num);
	} while (leaf->value != leaf->value);

}

//Adds nodes to a root-node until a given tree depth is reached
void add_nodes(node *leaf, tree *dtree, unsigned max_depth, unsigned current_depth, std::vector<unsigned> const &features,
	data_pointer const &data)
{
	if (current_depth < max_depth)
	{
		std::uniform_int_distribution<int> unif(0, 100);
		if (unif(generator) < 75)
		{
			leaf->left = new node;
			dtree->num_nodes += 1;
			leaf->left->id = dtree->num_nodes;
			set_node(leaf->left, features, data);

			leaf->right = new node;
			dtree->num_nodes += 1;
			leaf->right->id = dtree->num_nodes;
			set_node(leaf->right, features, data);


			dtree->depth = current_depth;
			add_nodes(leaf->left, dtree, max_depth, current_depth + 1, features, data);
			add_nodes(leaf->right, dtree, max_depth, current_depth + 1, features, data);
		}

	}

}

//This starts the random initialization of a new tree
void init_tree(tree* temp, std::vector<unsigned> const &features, data_pointer const &data, unsigned max_depth)
{
	set_node(temp->root, features, data);

	add_nodes(temp->root, temp, max_depth, 1, features, data);
}

//Currently unused
/*mat squared_error(mat a, mat b)
{
mat result = (a - b) % (a - b);
return result;
}*/

//Also unused at the moment
/*void median_errors(tree *dtree)
{
double max_error;
for (int i = 0; i < dtree->num_nodes; ++i)
{
dtree->median_errors.push_back();
}
}*/

//recursive function to perform a deep copy of a tree
void deep_copy(node *leaf_child, node *leaf_par)
{

	leaf_child->feature_num = leaf_par->feature_num;
	leaf_child->id = leaf_par->id;
	leaf_child->term_value = leaf_par->term_value;
	leaf_child->value = leaf_par->value;
	leaf_child->data_passed = 0;
	leaf_child->node_MSE = leaf_par->node_MSE;


	if (leaf_par->left != NULL)
	{
		leaf_child->left = new node;
		deep_copy(leaf_child->left, leaf_par->left);
	}

	if (leaf_par->right != NULL)
	{
		leaf_child->right = new node;
		deep_copy(leaf_child->right, leaf_par->right);
	}
}

//calculates the depth of a tree
unsigned count_depth(node *leaf, unsigned current_depth)
{
	if (leaf->left == NULL)
	{
		return current_depth;
	}
	else
	{
		unsigned depth_l = count_depth(leaf->left, current_depth + 1);
		unsigned depth_r = count_depth(leaf->right, current_depth + 1);
		if (depth_l > depth_r)
			return depth_l;
		else
			return depth_r;
	}
}

//resets the nodes, used after crossover/mutation
void set_ids_norand(node *leaf, tree *dtree)
{
	if (leaf != NULL)
	{
		dtree->num_nodes += 1;
		leaf->id = dtree->num_nodes;

		set_ids_norand(leaf->left, dtree);
		set_ids_norand(leaf->right, dtree);
	}
}



//Runs each data-point through the tree until a terminal node is reached
//each y-value that passed a node is being pushed back onto "y-values"
void mean_ys(node *leaf, mat input, double y)
{
	if (leaf != NULL)
	{
		leaf->y_values.push_back(y);
		leaf->data_passed += 1;


		if (input.at(0, leaf->feature_num) != input.at(0, leaf->feature_num) | input.at(0, leaf->feature_num) <= leaf->value)
		{
			if (leaf->left != NULL)
				mean_ys(leaf->left, input, y);
		}
		else
		{
			if (leaf->right != NULL)
				mean_ys(leaf->right, input, y);
		}

	}

}

void mean_ys(tree *dtree, mat input, mat y, unsigned minbucket)
{
	dtree->node_MSEs.clear();
	for (unsigned i = 0; i < y.n_rows; ++i)
		mean_ys(dtree->root, input.row(i), y.at(i, 0));
	for (unsigned i = 0; i<dtree->num_nodes; ++i)
		dtree->node_MSEs.push_back(0);

	middle_ys(dtree->root, dtree, minbucket);

}

//This calculates the mean of "y-values" in each node, calculates the variance within the node and also deletes nodes
//below a minimum size of "y-values"
void middle_ys(node *leaf, tree *dtree, unsigned minbucket)
{
	if (leaf != NULL)
	{
		if (leaf->data_passed > 0)
		{
			if (leaf->left != NULL && leaf->left->data_passed >= minbucket)
			{
				if (leaf->right != NULL && leaf->right->data_passed >= minbucket)
				{
					middle_ys(leaf->right, dtree, minbucket);
					middle_ys(leaf->left, dtree, minbucket);
				}
				else if (leaf->right != NULL && leaf->right->data_passed <minbucket)
				{
					dtree->node_MSEs[leaf->left->id - 1] = 0;
					dtree->destroy_tree(leaf->left);
					dtree->num_nodes -= 1;
					leaf->left = NULL;
					dtree->node_MSEs[leaf->right->id - 1] = 0;
					dtree->destroy_tree(leaf->right);
					dtree->num_nodes -= 1;
					leaf->right = NULL;
				}
			}
			else if (leaf->left != NULL && leaf->left->data_passed <minbucket)
			{
				dtree->node_MSEs[leaf->left->id - 1] = 0;
				dtree->destroy_tree(leaf->left);
				leaf->left = NULL;
				dtree->num_nodes -= 1;
				dtree->node_MSEs[leaf->right->id - 1] = 0;
				dtree->destroy_tree(leaf->right);
				leaf->right = NULL;
				dtree->num_nodes -= 1;
			}



			leaf->term_value = vec_mean(leaf->y_values);
			if (leaf->left == NULL)
			{
				std::vector<double> mses;
				for (unsigned i = 0; i < leaf->y_values.size(); ++i)
				{
					mses.push_back(pow(leaf->y_values[i] - leaf->term_value, 2));
				}


				leaf->node_MSE = vec_mean(mses);

				dtree->node_MSEs[leaf->id - 1] = leaf->node_MSE;
				mses.clear();
			}
			else
			{
				leaf->node_MSE = (leaf->left->node_MSE*leaf->left->data_passed + leaf->right->node_MSE*leaf->right->data_passed) /
					leaf->data_passed;

				dtree->node_MSEs[leaf->id - 1] = leaf->node_MSE;
				if (leaf->left != NULL)
				{
					if (leaf->left->left == NULL)
					{
						leaf->left->node_MSE = 0.0;
						dtree->node_MSEs[leaf->left->id - 1] = 0.0;
					}
					if (leaf->right->left == NULL)
					{
						leaf->right->node_MSE = 0.0;
						dtree->node_MSEs[leaf->right->id - 1] = 0.0;
					}
				}



			}

			leaf->y_values.clear();
		}

	}
}


//Randomly selects the features to choose from in set_nodes
std::vector<unsigned> random_features(mat const &X_mat_b)
{
	std::vector<unsigned> features;
	std::vector<unsigned> possible_features;

	for (unsigned i = 0; i < X_mat_b.n_cols; ++i)
	{
		possible_features.push_back(i);
	}

	std::uniform_int_distribution<unsigned> num_feat(1, X_mat_b.n_cols);

	unsigned num_f = num_feat(generator);

	for (unsigned j = 0; j < num_f; ++j)
	{
		std::uniform_int_distribution<unsigned> feat(0, possible_features.size() - 1);

		unsigned sel = feat(generator);
		features.push_back(possible_features[sel]);
		swap(possible_features[sel], possible_features[possible_features.size() - 1]);
		possible_features.pop_back();
	}

	return features;
}
