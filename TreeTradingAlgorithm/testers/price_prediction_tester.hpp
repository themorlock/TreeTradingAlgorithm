//
//  price_prediction_tester.hpp
//  TreeTradingAlgorithm
//
//  Created by Saaketh Vangati on 3/28/19.
//  Copyright © 2019 Svang. All rights reserved.
//

#ifndef price_prediction_tester_hpp
#define price_prediction_tester_hpp

#include <stdio.h>

#include "tester.hpp"

class price_prediction_tester : public tester {
public:
    static void set_data(const string &data_file_path);
    void test(organism &o);
private:
    static vector<double> *prices;
};

#endif /* price_prediction_tester_hpp */
