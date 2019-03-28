//
//  rsi_optimization_tester.hpp
//  TreeTradingAlgorithm
//
//  Created by Saaketh Vangati on 3/27/19.
//  Copyright © 2019 Svang. All rights reserved.
//

#ifndef rsi_optimization_tester_hpp
#define rsi_optimization_tester_hpp

#include <stdio.h>

#include "tester.hpp"

using namespace std;

class rsi_optimization_tester : public tester {
public:
    static void set_data(const string &data_file_path);
    void test(organism &o);
private:
    static vector<double> *prices;
};

#endif /* rsi_optimization_tester_hpp */
