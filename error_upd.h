#pragma once
#include "tree.h"
#include "data_loader.h"
#include <armadillo.h>
#include <random>
#include "bootstrap.h"

void mean_ys_upd(node *leaf, mat input, double y);
void mean_ys_upd(tree *dtree, mat input, mat y, unsigned minbucket);
void middle_ys_upd(node *leaf, tree *dtree, unsigned minbucket);