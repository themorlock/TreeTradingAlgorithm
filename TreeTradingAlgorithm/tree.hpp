//
//  tree.hpp
//  TreeTradingAlgorithm
//
//  Created by Saaketh Vangati on 3/25/19.
//  Copyright © 2019 Svang. All rights reserved.
//

#ifndef tree_hpp
#define tree_hpp

#include <vector>

#include "node.hpp"

using namespace std;

struct tree {
    tree(size_t inputs);
    void set_input(size_t input_num, double value);
    double out();
    static double combine_inputs(const node &n, const vector<node> &nodes);
	void addNode(node *new_node);
	void deleteNode();
    size_t inputs;
    vector<node> nodes;
};

#endif /* tree_hpp */
