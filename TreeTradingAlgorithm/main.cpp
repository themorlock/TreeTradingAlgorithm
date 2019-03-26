//
//  main.cpp
//  TreeTradingAlgorithm
//
//  Created by Saaketh Vangati on 3/20/19.
//  Copyright © 2019 Svang. All rights reserved.
//

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <string>
#include <thread>
#include <cstdlib>
#include <iomanip>
#include <sstream>

#include "node.hpp"
#include "tree.hpp"
#include "organism.hpp"
#include "world.hpp"
#include "world_io.hpp"

#include "tester.hpp"

using namespace std;

struct price_prediction_tester : public tester {
	price_prediction_tester(vector<double> &prices) : prices(prices) {}
	void test(organism &o) {
		unsigned const INPUTS = 5;
		double correct = 0;
		for(size_t price_idx = INPUTS - 1; price_idx < prices.size() - 1; ++price_idx) {
			vector<double> inputs(INPUTS);
			for(size_t i = 0; i < inputs.size(); ++i) {
				inputs[i] = prices[price_idx - i];
			}
			bool expected = prices[price_idx + 1] > prices[price_idx];
			o.set_inputs(inputs);
			bool output = o.get_output() > 0;
			correct += (expected == output ? 1 : -1);
		}
		o.fitness = correct / (double) (prices.size() - INPUTS);
	}
	vector<double> &prices;
};

struct ichimoku_prediction_tester : public tester {
	ichimoku_prediction_tester(vector<double> &prices) : prices(prices) {}
	void test(organism &o) {
		double correct = 0;
		for(auto price = prices.begin() + 52; price != prices.end() - 1; ++price) {
			double tenkan_sen = (*max_element(price - 9, price + 1) + *min_element(price - 9, price + 1)) / 2;
			double kijun_sen = (*max_element(price - 26, price + 1) + *min_element(price - 26, price + 1)) / 2;
			double senkou_span_a = (tenkan_sen + kijun_sen) / 2;
			double senkou_span_b = (*max_element(price - 52, price + 1) + *min_element(price - 52, price + 1)) / 2;
			double chikou_span = *(price - 26);
			vector<double> inputs = {*price, tenkan_sen, kijun_sen, senkou_span_a, senkou_span_b, chikou_span};
			bool expected = *(price + 1) > *price;
			o.set_inputs(inputs);
			bool output = o.get_output() > 0;
			correct += (expected == output ? 1 : -1);
		}
		o.fitness = correct / (double) (prices.size() - 52);
	}
	vector<double> &prices;
};

struct rsi_purchase_tester : public tester {
	rsi_purchase_tester(vector<double> &prices) : prices(prices) {}
	void test(organism &o) {
		double money_balance = 1;
		double stock_balance = 1 / prices[14];
		
		double average_gain = 0, average_loss = 0;
		size_t average_gain_count = 0, average_loss_count = 0;
		for(unsigned i = 0; i < 14; ++i) {
			if(prices[i + 1] > prices[i]) {
				average_gain += prices[i + 1] - prices[i];
				++average_gain_count;
			}else if(prices[i + 1] < prices[i]) {
				average_loss += prices[i] - prices[i + 1];
				++average_loss_count;
			}
		}
		average_gain /= average_gain_count;
		average_loss /= average_loss_count;
		double original_net_worth = money_balance + stock_balance * (prices[14]);
		for(size_t price_idx = 14; price_idx < prices.size(); ++price_idx) {
			unsigned rsi = 100.0 - 100.0 / (1.0 + (average_gain / average_loss));
			average_gain = ((average_gain * 13.0) + (prices[price_idx] > prices[price_idx - 1] ? prices[price_idx] - prices[price_idx - 1] : 0)) / 14.0;
			average_loss = ((average_loss * 13.0) + (prices[price_idx] < prices[price_idx - 1] ? prices[price_idx - 1] - prices[price_idx] : 0)) / 14.0;
			
			vector<double> inputs(6);
			for(size_t i = 0; i < inputs.size(); ++i) {
				inputs[i] = (i % 2 ? rsi : prices[price_idx]);
			}
			o.set_inputs(inputs);
			double output = o.get_output();
			if(output < 0) {
				double stock_sold = stock_balance * std::min(1.0, -output);
				
				stock_balance -= stock_sold;
				money_balance += stock_sold * prices[price_idx];
			}else {
				double money_sold = money_balance * std::min(1.0, output);
				
				money_balance -= money_sold;
				stock_balance += money_sold / prices[price_idx];
			}
		}
		double new_net_worth = money_balance + stock_balance * prices.back();
		o.fitness = new_net_worth / original_net_worth;
	}
	vector<double> &prices;
};

int main() {
	auto seed = rand();
	cout << "Seed: " << seed << "." << endl << endl;
    srand(seed);
	
	vector<double> prices;
	const string file_path = "intel_data.txt";
	ifstream data_file(file_path.c_str());
	while(data_file.good()) {
		prices.push_back(0);
		data_file >> prices.back();
	}
	data_file.close();
	
	const size_t POPULATION_SIZE = 1000;
    const size_t INPUTS = 6;
	const double ELITE_PERCENTILE = .03;
	const double SURVIVAL_PERCENTILE = .4;
	const double MUTATION_PERCENTAGE = .7;
	const double MUTATION_RATE = .07;
	const unsigned THREADS = 4;
	
	auto start_time = time(nullptr);
	stringstream st;
	st << put_time(localtime(&start_time), "%F_%T");
	auto start_time_string = st.str();
	replace(start_time_string.begin(), start_time_string.end(), ':', '-');
	const string output_file = st.str() + ".txt";
	
	//tester *t = new price_prediction_tester(prices);
	//tester *t = new ichimoku_prediction_tester(prices);
	tester *t = new rsi_purchase_tester(prices);
	world w(POPULATION_SIZE, INPUTS, ELITE_PERCENTILE, SURVIVAL_PERCENTILE, MUTATION_PERCENTAGE, MUTATION_RATE, THREADS, t);
	world_io io(w, output_file);
	vector<organism> organisms;
	const unsigned ITERATIONS = 100;
	for(unsigned i = 0; i < ITERATIONS; ++i) {
		w.measure_fitness();
		w.order_fitness();
		w.get_organisms(organisms);
		io.save_all();
		cout << "Generation " << i + 1 << " complete." << endl;
		cout << "Current Lowest Fitness:  Organism: " << organisms.front().id << ", Fitness: " << organisms.front().fitness << "." << endl;
		cout << "Current Highest Fitness: Organism: " << organisms.back().id << ", Fitness: " << organisms.back().fitness << "." << endl;
		cout << endl;
		if(i < ITERATIONS - 1) {
			w.reproduce();
			w.mutate();
		}
	}
	cout << endl;
}
