//
//  organism.hpp
//  TreeTradingAlgorithm
//
//  Created by Saaketh Vangati on 3/25/19.
//  Copyright © 2019 Svang. All rights reserved.
//

#ifndef organism_hpp
#define organism_hpp

#include <stdio.h>

#include "tree.hpp"

struct organism {
    organism(size_t inputs, size_t id);
    void set_inputs(const vector<double> &inputs);
    double get_output();
    size_t id;
    double fitness;
    tree t;
};

#endif /* organism_hpp */
