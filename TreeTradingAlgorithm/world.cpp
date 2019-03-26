//
//  world.cpp
//  TreeTradingAlgorithm
//
//  Created by Saaketh Vangati on 3/25/19.
//  Copyright © 2019 Svang. All rights reserved.
//

#include "world.hpp"

world::world(size_t population_size, size_t inputs, double elite_percentile, double survival_percentile, double mutation_percentage, double mutation_rate, unsigned threads, tester *t) : elite_percentile(elite_percentile), survival_percentile(survival_percentile), mutation_percentage(mutation_percentage), mutation_rate(mutation_rate), threads(min((unsigned) 4, threads)), testers(threads), thread_pool(threads) {
    for(size_t i = 0; i < population_size; ++i) {
        population.push_back(organism(inputs, i + 1));
    }
    for(auto &tester : testers) {
        tester = &(*t);
    }
}

void world::measure_fitness() {
    if(threads == 1) {
        for(auto &organism : population) {
            testers[0]->test(organism);
        }
    }else {
        thread_pool.clear();
        size_t elements_per_thread = max(1UL, 1 + population.size() / threads);
        size_t first = 0;
        size_t last = min(population.size(), first + elements_per_thread);
        size_t thread_num = 0;
        while(first < population.size() && last <= population.size()) {
            thread_pool.push_back(thread(&world::measure_fitness_async_range, this, ref(testers[thread_num]), first, last));
            
            first += elements_per_thread;
            last = min(population.size(), first + elements_per_thread);
            ++thread_num;
        }
        for(auto &thread : thread_pool) {
            thread.join();
        }
    }
}

void world::order_fitness() {
    sort(population.begin(), population.end(), [](organism a, organism b) {return a.fitness < b.fitness;});
}

void world::get_organisms(vector<organism> &organisms) {
    organisms = population;
}

void world::reproduce() {
    for(size_t i = 0; i < population.size() * (1.0 - survival_percentile); ++i) {
        for(size_t j = 0; j < population[i].t.nodes.size(); ++j) {
            node &a = population[rand() % (size_t) (population.size() * survival_percentile) + population.size() * (1.0 - survival_percentile)].t.nodes[j];
            node &b = population[rand() % (size_t) (population.size() * elite_percentile) + population.size() * (1.0 - elite_percentile)].t.nodes[j];
            node &c = population[i].t.nodes[j];
            switch(rand() % 2) {
                case 0:
                    c.type = a.type;
                    break;
                case 1:
                    c.type = b.type;
                    break;
            }
            switch(rand() % 2) {
                case 0:
                    node::setVariation(c, a.type);
                    break;
                case 1:
                    node::setVariation(c, b.type);
                    break;
            }
        }
    }
}

void world::mutate() {
    for(size_t i = 0; i < population.size() * mutation_percentage; ++i) {
        for(auto &node : population[i].t.nodes) {
            if(node.type != 0 && (rand() / RAND_MAX) < mutation_rate) {
                switch (rand() % 3) {
                    case 0:
                        node::setType(node, rand());
                        node::setVariation(node, rand());
                        break;
                    case 1:
                        node::setType(node, rand());
                        break;
                    case 2:
                        node::setVariation(node, rand());
                        break;
                }
            }
        }
    }
}

void world::measure_fitness_async_range(tester *t, size_t start, size_t end) {
    for(size_t i = start; i < end; ++i) {
        t->test(population[i]);
    }
}
