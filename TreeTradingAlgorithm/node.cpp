//
//  node.cpp
//  TreeTradingAlgorithm
//
//  Created by Saaketh Vangati on 3/25/19.
//  Copyright © 2019 Svang. All rights reserved.
//

#include "node.hpp"

void node::setType(node &n, unsigned type) {
    n.type = type % 11 + 1;
    node::setVariation(n, n.variation);
}

void node::setVariation(node &n, unsigned variation) {
    if(n.type <= 1)
        n.variation = variation;
    else
        n.variation = variation % 2;
}
