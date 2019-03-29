//
//  rsi_optimization_tester.cpp
//  TreeTradingAlgorithm
//
//  Created by Saaketh Vangati on 3/27/19.
//  Copyright © 2019 Svang. All rights reserved.
//

#include <fstream>
#include <limits>

#include "rsi_optimization_tester.hpp"

void rsi_optimization_tester::set_data(const string &data_file_path) {
    fstream data_file(data_file_path);
    while(data_file.good()) {
        double price;
        data_file >> price;
        prices->push_back(price);
    }
    data_file.close();
}

void rsi_optimization_tester::test(organism &o) {
    const size_t RSI_PERIOD = 13;
    
    double money_balance = 1;
    double stock_balance = 1 / (*prices)[RSI_PERIOD];
    
    double average_gain = 0, average_loss = 0;
    size_t average_gain_count = 0, average_loss_count = 0;
    for(size_t i = 0; i < RSI_PERIOD; ++i) {
        if((*prices)[i + 1] > (*prices)[i]) {
            average_gain += (*prices)[i + 1] - (*prices)[i];
            ++average_gain_count;
        }else if((*prices)[i + 1] < (*prices)[i]) {
            average_loss += (*prices)[i] - (*prices)[i + 1];
            ++average_loss_count;
        }
    }
    average_gain /= average_gain_count;
    average_loss /= average_loss_count;
    
    double original_net_worth = money_balance + stock_balance * (*prices)[RSI_PERIOD];
    
    for(size_t i = RSI_PERIOD; i < prices->size(); ++i) {
        double rsi = 100.0 - 100.0 / (1.0 + (average_gain / average_loss));
        double normalized_rsi = rsi / 100.0;
        average_gain = ((average_gain * (double) (RSI_PERIOD - 1)) + ((*prices)[i] > (*prices)[i - 1] ? (*prices)[i] - (*prices)[i - 1] : 0)) / (double) RSI_PERIOD;
        average_loss = ((average_loss * (double) (RSI_PERIOD - 1)) + ((*prices)[i] < (*prices)[i - 1] ? (*prices)[i - 1] - (*prices)[i] : 0)) / (double) RSI_PERIOD;
        
        vector<double> inputs(o.t.inputs);
        for(auto &i : inputs) {
            i = normalized_rsi;
        }
        o.set_inputs(inputs);
        
        double output = o.get_output();
        if(output < -1.0 || output > 1.0) {
            o.fitness = 0;
            return;
        }
        if(output < 0) {
            double stock_sold = stock_balance * (-output);
            
            stock_balance -= stock_sold;
            money_balance += stock_sold * (*prices)[i];
        }else {
            double money_sold = money_balance * output;
            
            money_balance -= money_sold;
            stock_balance += money_sold / (*prices)[i];
        }
    }
    
    double new_net_worth = money_balance + stock_balance * prices->back();
    
    double net_worth_percent_increase = new_net_worth / original_net_worth;
    o.fitness = net_worth_percent_increase;
}

vector<double> *rsi_optimization_tester::prices = new vector<double>();
