#include <iostream>
#include <armadillo.h>
#include "algorithm.h"
#include "genetic.h"
#include <windows.h>

using namespace std;
using namespace arma;

//Just give the algorithm a (header-less) training and test set with the endogenous variable in the first row 
//It will print out the trees that are included in the ensemble, predict the test set and the MSE on the test set
int main()
{
	//see algorithm.cpp for explanation of the function's variables
	run_algorithm(0.09, "iris_train_1.csv", "iris_test_1.csv", 10, 20, 300, 10, 1, 1, 30, 0.7,true,600);

	Beep(1000, 2000);

	std::cout << "Press enter to continue ...";
	std::cin.get();

	return 0;
}

