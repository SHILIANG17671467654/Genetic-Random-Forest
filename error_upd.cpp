#include "error_upd.h"


//This calculates the error within each node given tree and training data
void mean_ys_upd(node *leaf, mat input, double y)
{
	if (leaf != NULL)
	{
		leaf->y_values.push_back(y);


		if (input.at(0, leaf->feature_num) <= leaf->value)
		{
			if (leaf->left != NULL)
				mean_ys_upd(leaf->left, input, y);
		}
		else if (input.at(0, leaf->feature_num)>leaf->value)
		{
			if (leaf->right != NULL)
				mean_ys_upd(leaf->right, input, y);
		}

	}

}

void mean_ys_upd(tree *dtree, mat input, mat y, unsigned minbucket)
{
	dtree->node_MSEs.clear();
	for (unsigned i = 0; i < y.n_rows; ++i)
		mean_ys_upd(dtree->root, input.row(i), y.at(i, 0));
	for (unsigned i = 0; i<dtree->num_nodes; ++i)
		dtree->node_MSEs.push_back(0);
	//cout << "tree before:" << endl;
	//dtree->print_tree();
	middle_ys_upd(dtree->root, dtree, minbucket);
	//cout << "tree after:" << endl;
	//dtree->print_tree();

}

void middle_ys_upd(node *leaf, tree *dtree, unsigned minbucket)
{
	if (leaf != NULL)
	{
		if (leaf->y_values.size() > 0)
		{
			if (leaf->left != NULL)
			{
				middle_ys_upd(leaf->right, dtree, minbucket);
				middle_ys_upd(leaf->left, dtree, minbucket);
			}

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

