//
//  organism.cpp
//  TreeTradingAlgorithm
//
//  Created by Saaketh Vangati on 3/25/19.
//  Copyright © 2019 Svang. All rights reserved.
//

#include <vector>

#include "organism.hpp"
#include "tree.hpp"

organism::organism(size_t inputs, size_t id) : t(inputs), id(id) {}

void organism::set_inputs(const vector<double> &inputs) {
    for(size_t input_num = 0; input_num < t.inputs; ++input_num) {
        t.set_input(input_num, inputs[input_num % inputs.size()]);
    }
}

double organism::get_output() {
    return t.out();
}
