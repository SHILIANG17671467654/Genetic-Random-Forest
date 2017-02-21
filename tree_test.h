//I used this file only for testing//

/*#pragma once
#include "tree.h"
#include "data_loader.h"
#include "init_tree.h"
#include "algebraics.h"
#include "genetic.h"
#include "bootstrap.h"
#include <iostream>
#include "forest.h"
#include <C:\Users\sarem\Documents\cpp_libs\armadillo-7.600.1\include\armadillo.h>
#include "pruning.h"
#include <windows.h>
#include <iomanip>

using namespace arma;


std::default_random_engine generator6(time(NULL));


void testdata()
{
load_data("iris_train_2.csv", 0);

genalg gentest;

std::vector<unsigned> features;

data_pointer data;
std::vector<tree*> trees;
features = random_features(X_mat);
gentest.init_alg(features, data, 10, 20, 10,300);


while (trees.size()<10)
{
features = random_features(X_mat);
gentest.new_generation(data, features, 10, 20, 1, 1, 10,300);


if (gentest.MSEs[0] < 0.09)
{
cout << trees.size() << endl;
trees.push_back(gentest.getbest());
gentest.print_best();
}
//cout << gentest.MSEs[0] << endl;

}





load_data("iris_test_2.csv", 0);

mat test(X_mat.n_rows, trees.size());

for (int i = 0; i < trees.size(); ++i)
{
test.col(i) = trees[i]->new_values(X_mat);
}


uword dime = 1;

cout << setprecision(20) << mean(pow((mean(test, dime) - y_mat), 2)) << endl;
Beep(523, 1000);



}*/