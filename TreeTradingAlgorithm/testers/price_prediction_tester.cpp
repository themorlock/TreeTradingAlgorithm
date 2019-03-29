//
//  price_prediction_tester.cpp
//  TreeTradingAlgorithm
//
//  Created by Saaketh Vangati on 3/28/19.
//  Copyright © 2019 Svang. All rights reserved.
//

#include <string>
#include <fstream>
#include <limits>
#include <cmath>

#include "price_prediction_tester.hpp"

using namespace std;

void price_prediction_tester::set_data(const string &data_file_path) {
    fstream data_file(data_file_path);
    while(data_file.good()) {
        double price;
        data_file >> price;
        prices->push_back(price);
    }
    data_file.close();
}

void price_prediction_tester::test(organism &o) {
    double error = 0;
    for(size_t i = 0; i < prices->size() - 1; ++i) {
        vector<double> inputs(o.t.inputs);
        for(auto &input : inputs) {
            input = (*prices)[i];
        }
        o.set_inputs(inputs);
        
        double output = o.get_output();
        if(output < 0.0 || output > 1.0) {
            o.fitness = 0;
            return;
        }
        double expected = (*prices)[i + 1];
        error += (output > expected ? output - expected : expected - output);
    }
    double fitness = (error != 0.0 ? (1.0 / error) : numeric_limits<double>::max());
    o.fitness = fitness;
}

vector<double> *price_prediction_tester::prices = new vector<double>();
