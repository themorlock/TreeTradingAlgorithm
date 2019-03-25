//
//  node.hpp
//  TreeTradingAlgorithm
//
//  Created by Saaketh Vangati on 3/25/19.
//  Copyright © 2019 Svang. All rights reserved.
//

#ifndef node_hpp
#define node_hpp

#include <stdio.h>

struct node {
    static void setType(node &n, unsigned type);
    static void setVariation(node &n, unsigned variation);
    unsigned type;
    unsigned variation;
    double value;
    size_t parents[2];
};

#endif /* node_hpp */
