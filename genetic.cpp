#include "genetic.h"
#include "algebraics.h"
#include "calc_element.h"
#include "tree.h"
#include "init_tree.h"
#include "error_upd.h"
#include <vector>
#include <iomanip>



using namespace std;
using namespace std;


std::default_random_engine generator2(time(NULL));


//struct element that holds a tree and the corresponding error
struct tree_soil
{
	tree_soil();
	~tree_soil();

	double error;

	tree *dtree;
};

//constructor also constructs a new tree
tree_soil::tree_soil()
{
	dtree = new tree;
}

//destructor destroys the tree
tree_soil::~tree_soil()
{
	delete dtree;
}


//Destructor of the GA-class destroys all trees in the tree_soil class
genalg::~genalg()
{
	while (forest.size()>0)
	{
		delete forest[forest.size() - 1]->dtree;
		forest.pop_back();
	}
}


//Random intialization function for the first generation
void genalg::init_alg(std::vector<unsigned> const &features, data_pointer &data, unsigned num_trees, unsigned maxdepth,
	unsigned minbucket, unsigned boot_size)
{
	//random subset
	data.create_bootstrap(boot_size);

	//random creation of new trees and ca
	for (unsigned i = 0; i < num_trees; ++i)
	{
		forest.push_back(new tree_soil);
		init_tree(forest[i]->dtree, features, data, maxdepth);

		//still some elements of an early version, will make this more efficient
		mean_ys(forest[i]->dtree, data.X_mat_b, data.y_mat_b, minbucket);
		forest[i]->error = forest[i]->dtree->root->node_MSE;
		MSEs.push_back(forest[i]->error);
	}
	//Next subsample
	data.create_bootstrap(boot_size);

	//evaluation of each tree on the new subsample
	for (int i = 0; i < forest.size(); ++i)
	{
		mean_ys_upd(forest[i]->dtree, data.X_mat_b, data.y_mat_b, minbucket);
		forest[i]->error = forest[i]->dtree->root->node_MSE;
		MSEs[i] = forest[i]->error;
	}
	//This orders the trees with increasing error rate
	order_matrix();
}

//Given a vector of node-/tree-errors, this function randomly selects one index-element of the vector - the higher 
//lower error means higher probability of being drawn
//If the error-value is 0, the corresponding index has 0% chance of being selected
unsigned genalg::draw_prob_min(std::vector<double> const &error)
{
	std::vector<double> cumulatives;

	double max_err = *max_element(error.begin(), error.end());

	for (unsigned i = 0; i < error.size(); ++i)
	{
		if (i == 0)
			cumulatives.push_back((max_err + 1) - error[i]);
		else
			cumulatives.push_back(0 + cumulatives[i - 1]);
	}

	double max_el = *max_element(cumulatives.begin(), cumulatives.end());

	std::uniform_real_distribution<double> unif(0, max_el);

	double select = unif(generator2);

	for (unsigned i = 0; i < cumulatives.size(); ++i)
	{
		if (select<cumulatives[i])
			return i;
	}

}

//Same as above, but higher error-values lead to higher chance of being selected
unsigned genalg::draw_prob_max(std::vector<double> const &error)
{
	std::vector<double> cumulatives;

	double max_err = *max_element(error.begin(), error.end());

	for (unsigned i = 0; i < error.size(); ++i)
	{
		if (i == 0)
			cumulatives.push_back(error[i]);
		else
			if (error[i] != -1)
				cumulatives.push_back(error[i] + cumulatives[i - 1]);
			else
				cumulatives.push_back(0 + cumulatives[i - 1]);
	}

	double max_el = *max_element(cumulatives.begin(), cumulatives.end());

	std::uniform_real_distribution<double> unif(0, max_el);

	double select = unif(generator2);

	for (unsigned i = 0; i < cumulatives.size(); ++i)
	{
		if (select<cumulatives[i])
			return i;
	}

}

//Orders the trees with increasing error
void genalg::order_matrix()
{
	for (unsigned int i = 1; i < MSEs.size(); i++)
	{
		int a = i;

		while (MSEs[a] < MSEs[a - 1])
		{
			swap(forest[a], forest[a - 1]);
			swap(MSEs[a], MSEs[a - 1]);
			a -= 1;
			if (a < 1)
				break;
		}
	}
}

//Creates a new generation
void genalg::new_generation(data_pointer &data, std::vector<unsigned> const &features,
	unsigned num_trees, unsigned maxdepth, unsigned no_elite, unsigned no_new, unsigned minbucket, unsigned boot_size)
{
	std::vector<tree_soil*> forest_new;

	//This takes the best trees into the next generation while bypassing the crossover/mutation process
	for (unsigned i = 0; i < no_elite; ++i)
	{
		forest_new.push_back(new tree_soil);

		deep_copy(forest_new[i]->dtree->root, forest[i]->dtree->root);
		forest_new[i]->error = forest[i]->error;

		forest_new[i]->dtree->depth = forest[i]->dtree->depth;
		forest_new[i]->dtree->id = i;
		forest_new[i]->dtree->num_nodes = forest[i]->dtree->num_nodes;

		forest_new[i]->dtree->num_nodes = 0;
		set_ids_norand(forest_new[i]->dtree->root, forest_new[i]->dtree);

		mean_ys(forest_new[i]->dtree, data.X_mat_b, data.y_mat_b, minbucket);


		forest_new[i]->error = forest_new[i]->dtree->root->node_MSE;

	}

	//This generates completely new trees for the new generation to maintain diversity
	for (unsigned h = no_elite; h < no_elite + no_new; h++)
	{
		forest_new.push_back(new tree_soil);

		init_tree(forest_new[h]->dtree, features, data, maxdepth);

		forest_new[h]->dtree->num_nodes = 0;
		set_ids_norand(forest_new[h]->dtree->root, forest_new[h]->dtree);

		mean_ys(forest_new[h]->dtree, data.X_mat_b, data.y_mat_b, minbucket);

		forest_new[h]->error = forest_new[h]->dtree->root->node_MSE;
	}

	//Crossover and mutation
	for (unsigned j = no_elite + no_new; j < num_trees; j++)
	{
		forest_new.push_back(new tree_soil);

		bool check = false;
		//Only trees with depth below a maximum depth are allowed to enter the next generation
		while (check == false)
		{
			//Tree selection based on their error
			int tree_1 = draw_prob_min(MSEs);
			int tree_2 = draw_prob_min(MSEs);

			//Node selection - based on error
			int node_1 = draw_prob_max(forest[tree_1]->dtree->node_MSEs);
			int node_2 = draw_prob_min(forest[tree_2]->dtree->node_MSEs);

			//This is where the actual crossover takes place
			node_copy_to(forest_new[j]->dtree->root, forest[tree_1]->dtree->root, forest[tree_2]->dtree->root,
				node_1, node_2);

			if (count_depth(forest_new[j]->dtree->root) <= maxdepth)
			{
				check = true;
				forest_new[j]->dtree->depth = count_depth(forest_new[j]->dtree->root);

			}
			else
			{
				delete forest_new[j];
				forest_new[j] = new tree_soil;
			}
		}

		forest_new[j]->dtree->id = j;

		//This resets the node-ids in the crossovered trees
		forest_new[j]->dtree->num_nodes = 0;
		set_ids_norand(forest_new[j]->dtree->root, forest_new[j]->dtree);


		std::uniform_int_distribution<int> unif(0, 99);

		int realization = unif(generator2);

		//This is were the mutation operation takes place - either actual node mutation, deletion or both simultanously
		if (realization < 35)
		{
			mut_node(forest_new[j], features, data);
		}

		else if (realization >= 35 && realization < 70)
		{
			del_node(forest_new[j]);
			forest_new[j]->dtree->num_nodes = 0;
			set_ids_norand(forest_new[j]->dtree->root, forest_new[j]->dtree);

		}

		else if (realization >= 70 && realization < 95)
		{
			del_node(forest_new[j]);
			forest_new[j]->dtree->num_nodes = 0;
			set_ids_norand(forest_new[j]->dtree->root, forest_new[j]->dtree);

			mut_node(forest_new[j], features, data);
		}



		mean_ys(forest_new[j]->dtree, data.X_mat_b, data.y_mat_b, minbucket);
		forest_new[j]->error = forest_new[j]->dtree->root->node_MSE;
	}

	//deletes the old generation
	for (unsigned h = 0; h < forest.size(); h++)
	{
		delete forest[h];
		forest[h] = forest_new[h];
	}

	//new bootstrap and error calculation
	data.create_bootstrap(boot_size);

	for (int i = 0; i < forest.size(); ++i)
	{
		mean_ys_upd(forest[i]->dtree, data.X_mat_b, data.y_mat_b, minbucket);
		forest[i]->error = forest[i]->dtree->root->node_MSE;
		MSEs[i] = forest[i]->error;
	}


	order_matrix();

}

//crossover-algorithm to copy every node starting from the node to crossover into the first tree
void genalg::node_copy_from(node *leaf, node *leaf_one, int id, bool start)
{
	if (leaf_one != NULL)
	{
		if (leaf->id == id)
			start = true;

		if (start = true)
		{
			leaf->id = leaf_one->id;
			leaf->value = leaf_one->value;
			leaf->feature_num = leaf_one->feature_num;
			leaf->term_value = leaf_one->term_value;
			leaf->node_MSE = leaf_one->node_MSE;
			leaf->data_passed = 0;

			if (leaf_one->left != NULL)
			{
				leaf->left = new node;
				node_copy_from(leaf->left, leaf_one->left, id, start = true);
			}
			else if (leaf_one->left == NULL)
			{
				leaf->left = NULL;
			}

			if (leaf_one->right != NULL)
			{
				leaf->right = new node;
				node_copy_from(leaf->right, leaf_one->right, id, start = true);
			}
			else if (leaf_one->right == NULL)
			{
				leaf->right = NULL;
			}

		}
		else if (start == false)
		{
			node_copy_from(leaf, leaf_one->left, id, start = false);
			node_copy_from(leaf, leaf_one->right, id, start = false);
		}
	}
}

//crossover-algorithm that copies the first tree until the node that is being replaced by the node from the second tree
void genalg::node_copy_to(node *leaf, node *leaf_one, node *leaf_two, int id, int id_2)
{
	if (leaf_one != NULL)
	{
		if (leaf->id != id)
		{
			leaf->id = leaf_one->id;
			leaf->value = leaf_one->value;
			leaf->feature_num = leaf_one->feature_num;
			leaf->term_value = leaf_one->term_value;
			leaf->node_MSE = leaf_one->node_MSE;
			leaf->data_passed = 0;

			if (leaf_one->left != NULL)
			{
				leaf->left = new node;
				node_copy_to(leaf->left, leaf_one->left, leaf_two, id, id_2);
			}
			else if (leaf_one->left == NULL)
			{
				leaf->left = NULL;
			}

			if (leaf_one->right != NULL)
			{
				leaf->right = new node;
				node_copy_to(leaf->right, leaf_one->right, leaf_two, id, id_2);
			}
			else if (leaf_one->right == NULL)
			{
				leaf->right = NULL;
			}

		}
		else if (leaf->id == id)
		{
			node_copy_from(leaf, leaf_two, id);
		}

	}
}

//recursive function that deletes every node starting from a node with a given id
void genalg::del_node(node *leaf, unsigned id, tree *dtree)
{
	if (leaf != NULL)
	{
		if (leaf->id == id)
		{
			dtree->destroy_tree(leaf->left);
			dtree->destroy_tree(leaf->right);
			leaf->left = NULL;
			leaf->right = NULL;
		}
		else
		{
			del_node(leaf->left, id, dtree);
			del_node(leaf->right, id, dtree);
		}
	}
}

//function that selects the node to start the deletion with
void genalg::del_node(tree_soil* soil)
{
	std::uniform_int_distribution<int> unif(1, soil->dtree->num_nodes);
	unsigned choose = unif(generator2);
	del_node(soil->dtree->root, choose, soil->dtree);
}

//recursive function that searches and mutates a node with a given id
void genalg::mut_node(node *leaf, unsigned id, std::vector<unsigned> const &features, data_pointer const &data)
{
	if (leaf != NULL)
	{
		if (leaf->id == id)
		{
			std::uniform_int_distribution<int> prob(0, 99);
			if (prob(generator2) < 25)
				set_node(leaf, features, data);
			else
			{
				std::uniform_int_distribution<int> sel_row(0, data.rows_b - 1);
				do
				{
					leaf->value = data.X_mat_b.at(sel_row(generator2), leaf->feature_num);
				} while (leaf->value != leaf->value);
			}
		}
		else
		{
			mut_node(leaf->left, id, features, data);
			mut_node(leaf->right, id, features, data);
		}
	}
}

//selects node to start the mutation - up to half the tree's nodes will be mutated
void genalg::mut_node(tree_soil* soil, std::vector<unsigned> const &features, data_pointer const &data)
{
	std::uniform_int_distribution<int> prob(1, int(soil->dtree->num_nodes / 2));
	for (int i = 0; i < prob(generator2); ++i)
	{
		std::uniform_int_distribution<int> unif(1, soil->dtree->num_nodes);
		unsigned choose = unif(generator2);
		mut_node(soil->dtree->root, choose, features, data);
	}
}

//prints the best tree
void genalg::print_best()
{
	forest[0]->dtree->print_tree();
}

//Uses the best tree to make predictions for new data
double genalg::calc_new(mat const &X_mat)
{
	return forest[0]->dtree->get_value(X_mat);
}

mat genalg::calc_new_data(mat const &X_mat)
{
	return forest[0]->dtree->new_values(X_mat);
}

//calculates the error for new data
double genalg::new_error(mat const &X, mat const &y)
{
	return forest[0]->dtree->new_error(X, y);
}

//to get the best tree in the current generation
tree* genalg::getbest()
{
	tree *temp;
	temp = new tree;

	deep_copy(temp->root, forest[0]->dtree->root);
	temp->depth = forest[0]->dtree->depth;
	temp->id = 0;
	temp->node_MSEs = forest[0]->dtree->node_MSEs;
	temp->num_nodes = forest[0]->dtree->num_nodes;

	return temp;
}
