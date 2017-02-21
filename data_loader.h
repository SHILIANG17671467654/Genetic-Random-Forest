#pragma once
#include <iostream>
#include <armadillo.h>
#include <string>
#include <vector>

using namespace std;
using namespace arma;


//functions and objects to load and hold the data

extern mat X_mat;
extern mat y_mat;

extern unsigned rows;
extern unsigned X_cols;

void load_data(string filename, unsigned y);
void print_X();
void print_y();

