#include "algorithm.h"
#include <iomanip>


//error_threshold -> threshold error value to select a tree
//train_file/test_file -> .csv-files to perform the algorithm on - endogenous variable has to be in the first column
//num_trees -> population size
//maxdepth -> maximum depth of a tree
//bootstrap_size -> size of the pseudo bootstrap - absolute number
//min_node_size -> minimum number of datapoints in each node
//no_elite -> size of the elite
//no_extra -> number of newly generated trees for each generation
//ensemble_size -> self explanatory
//cor_threshold -> correlation threshold for selecting which tree to include into the ensemble
//print -> wether to print the trees selected for the ensemble or not
//max_duration -> duration after which to stop the algorithm - in case it does not finish early (which still happens
//quite often)



void run_algorithm(double error_threshold, string train_file, string test_file, unsigned num_trees, unsigned maxdepth,
	unsigned bootstrap_size, unsigned min_node_size, unsigned no_elite, unsigned no_extra, unsigned ensemble_size, double cor_threshold,
	bool print, double max_duration)
{

	load_data(train_file, 0);
	genalg gentest;

	std::vector<unsigned> features;

	data_pointer data;
	std::vector<tree*> trees;
	features = random_features(X_mat);
	gentest.init_alg(features, data, num_trees, maxdepth, min_node_size, bootstrap_size);

	mat train_pred = zeros(rows, ensemble_size);
	mat curr_pred(rows, 1);
	
	wall_clock timer;
	timer.tic();

	while (trees.size() < ensemble_size)
	{
		features = random_features(X_mat);
		gentest.new_generation(data, features, num_trees, maxdepth, no_elite, no_extra, min_node_size, bootstrap_size);


		if (gentest.MSEs[0] < error_threshold)
		{

			if (trees.size() == 0)
			{
				cout << "Tree No.: " << trees.size() + 1 << "\n" << endl;

				train_pred.col(trees.size()) = y_mat-gentest.calc_new_data(X_mat);

				trees.push_back(gentest.getbest());
				if (print == true)
					gentest.print_best();
			}
			else
			{
				curr_pred.col(0) = y_mat-gentest.calc_new_data(X_mat);
				double corr = get_cor(train_pred, curr_pred, y_mat, trees.size());
				//cout << "Correlation: " << corr << "\n" << endl;

				if (abs(corr) < cor_threshold)
				{
					

					//train_pred.col(trees.size()-1) = gentest.calc_new_data(X_mat);
					train_pred.col(trees.size()) = y_mat - gentest.calc_new_data(X_mat);


					trees.push_back(gentest.getbest());
					if (print == true)
						cout << "\n\nTree No.: " << trees.size()<< "\n" << endl;
						cout << "Correlation: " << corr << "\n" << endl;
						gentest.print_best();
				}

			}
		
			if (timer.toc() > max_duration)
			{
				cout << "Elapsed time is above threshold of " << int(max_duration) << " seconds - will stop now!" << endl;
				break;
			}
				

			
		}
		//cout << gentest.MSEs[0] << endl;

	}





	load_data(test_file, 0);

	mat test(X_mat.n_rows, trees.size());

	for (int i = 0; i < trees.size(); ++i)
	{
		test.col(i) = trees[i]->new_values(X_mat);
	}


	uword dime = 1;

	
	cout << "Predicted values: \n" << endl;
	cout << mean(test, dime) << "\n\n" << endl;
	cout << setprecision(10) << mat(mean(pow((mean(test, dime) - y_mat), 2))).at(0,0) << endl;
	cout << "\nTime elapsed: " << int(timer.toc()) << " seconds" << endl;

}


double get_cor(mat const &ensemble, mat const &new_pred, mat &const y_mat, unsigned const &cols)
{
	mat dummy(ensemble.n_rows, cols+1);
	dummy(span(0, rows - 1), span(0, cols - 1)) = ensemble(span(0, rows-1), span(0, cols-1));
	dummy(span(0, rows - 1), cols ) = new_pred;

	//cout << ensemble << endl;
	//cout << new_pred << endl;

	//mat deviance(ensemble.n_rows, 2);

	mat corr = cor(dummy);

	//deviance.col(0) = pow(mean(dummy, dimd)-y_mat,2);
	//deviance.col(1) = pow(new_pred - y_mat, 2);
	//cout << corr << endl;
	//cout << abs(corr(cols,span(0,cols-1))) << endl;

	return mat(mean(corr(cols, span(0, cols - 1)), uword(1))).at(0, 0);
	
}