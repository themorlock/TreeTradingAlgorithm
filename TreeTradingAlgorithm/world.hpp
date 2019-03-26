//
//  world.hpp
//  TreeTradingAlgorithm
//
//  Created by Saaketh Vangati on 3/25/19.
//  Copyright © 2019 Svang. All rights reserved.
//

#ifndef world_hpp
#define world_hpp

#include <vector>
#include <thread>

#include "organism.hpp"

#include "tester.hpp"

using namespace std;

class world {
public:
    world(size_t population_size, size_t inputs, double elite_percentile, double survival_percentile, double mutation_percentage, double mutation_rate, unsigned threads, tester *t);
    void measure_fitness();
    void order_fitness();
    void get_organisms(vector<organism> &organisms);
    void reproduce();
    void mutate();
private:
    void measure_fitness_async_range(tester *t, size_t start, size_t end);
    double elite_percentile;
    double survival_percentile;
    double mutation_percentage;
    double mutation_rate;
    vector<organism> population;
    unsigned threads;
    vector<tester*> testers;
    vector<thread> thread_pool;
    friend class world_io;
};

#endif /* world_hpp */
