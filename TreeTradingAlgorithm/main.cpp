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

#include "node.hpp"
#include "tree.hpp"
#include "organism.hpp"
#include "world.hpp"
#include "world_io.hpp"

#include "testers/tester.hpp"
#include "testers/rsi_optimization_tester.hpp"

using namespace std;
 
int main() {
	const size_t POPULATION_SIZE = 1000;
    const size_t INPUTS = 5;
	const double ELITE_PERCENTILE = .03;
	const double SURVIVAL_PERCENTILE = .4;
	const double MUTATION_PERCENTAGE = .7;
	const double MUTATION_RATE = .07;
	const unsigned THREADS = 4;
	
	const string data_file_path = "intel_data.txt";
	rsi_optimization_tester::set_data(data_file_path);
	tester *t = new rsi_optimization_tester();
	
	world w(POPULATION_SIZE, INPUTS, ELITE_PERCENTILE, SURVIVAL_PERCENTILE, MUTATION_PERCENTAGE, MUTATION_RATE, THREADS, t);
	world_io io(w);
	const unsigned ITERATIONS = 1000;
	for(unsigned i = 0; i < ITERATIONS; ++i) {
		w.measure_fitness();
		w.order_fitness();
		io.save_all();
		if(i < ITERATIONS - 1) {
			w.reproduce();
			w.mutate();
		}
	}
}
