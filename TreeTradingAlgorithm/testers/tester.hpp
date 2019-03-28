//
//  tester.hpp
//  TreeTradingAlgorithm
//
//  Created by Saaketh Vangati on 3/25/19.
//  Copyright © 2019 Svang. All rights reserved.
//

#ifndef tester_hpp
#define tester_hpp

#include "organism.hpp"

struct tester {
    virtual void test(organism &o) =0;
};

#endif /* tester_hpp */
