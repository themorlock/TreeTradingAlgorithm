//
//  tree.cpp
//  TreeTradingAlgorithm
//
//  Created by Saaketh Vangati on 3/25/19.
//  Copyright © 2019 Svang. All rights reserved.
//

#include <cmath>

#include "tree.hpp"

tree::tree(size_t inputs) : inputs(inputs), nodes((inputs * (inputs + 1)) / 2) {
    for(size_t i = 0; i < nodes.size(); ++i) {
        node::setType(nodes[i], rand());
        node::setVariation(nodes[i], rand());
		nodes[i].id = i;
        nodes[i].parents[0] = i - (size_t) ceil((-1 + sqrt(1 + 8 * (i + 1))) / 2);
        nodes[i].parents[1] = i - 1;
    }
    for(size_t input_num = 0; input_num < inputs; ++input_num) {
        nodes[(input_num * (input_num + 1)) / 2].type = 0;
    }
}

void tree::set_input(size_t input_num, double value) {
    nodes[(input_num * (input_num + 1)) / 2].value = value;
}

double tree::out() {
    for(auto &node : nodes) {
        node.value = tree::combine_inputs(node, nodes);
    }
    return nodes.back().value;
}

double tree::combine_inputs(const node &n, const vector<node> &nodes) {
    double temp;
    switch(n.type) {
        case 0: //Input
            return n.value;
            break;
        case 1: // CONSTANT
            return n.variation;
            break;
        case 2: //COSINE
            return cos(nodes[n.parents[n.variation]].value);
            break;
        case 3: //SINE
            return sin(nodes[n.parents[n.variation]].value);
            break;
        case 4: //TANGENT
            return tan(nodes[n.parents[n.variation]].value);
            break;
        case 5: //e^x
            return exp(nodes[n.parents[n.variation]].value);
            break;
        case 6: //NATURAL LOG
            temp = nodes[n.parents[n.variation]].value;
            if(temp > 0)
                return log(temp);
            return temp;
            break;
        case 7: //ADD
            return nodes[n.parents[n.variation]].value + nodes[n.parents[n.variation ^ 1]].value;
            break;
        case 8: //SUBTRACT
            return nodes[n.parents[n.variation]].value - nodes[n.parents[n.variation ^ 1]].value;
            break;
        case 9: //MULTIPLY
            return nodes[n.parents[n.variation]].value * nodes[n.parents[n.variation ^ 1]].value;
            break;
        case 10: //DIVIDE
            temp = nodes[n.parents[n.variation ^ 1]].value;
            if(temp != 0)
                return nodes[n.parents[n.variation]].value / nodes[n.parents[n.variation ^ 1]].value;
            return nodes[n.parents[n.variation]].value;
            break;
        case 11: //EXPONENTIATE
            return pow(nodes[n.parents[n.variation]].value, nodes[n.parents[n.variation ^ 1]].value);
            break;
    }
    return -1;
}
