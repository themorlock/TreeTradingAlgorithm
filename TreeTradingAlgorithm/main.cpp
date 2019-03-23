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

using namespace std;

class node {
public:
    static void setType(node &n, unsigned type) {
		n.type = type % 11 + 1;
        node::setVariation(n, n.variation);
    }
    static void setVariation(node &n, unsigned variation) {
        if(n.type <= 1)
            n.variation = variation;
        else
            n.variation = variation % 2;
    }
private:
    unsigned type;
    unsigned variation;
    double value;
    size_t parents[2];
    friend class tree;
	friend class organism;
	friend class world;
	
	friend class tester;
};

class tree {
public:
    tree(size_t inputs) : inputs(inputs), nodes((inputs * (inputs + 1)) / 2) {
        for(size_t i = 0; i < nodes.size(); ++i) {
            node::setType(nodes[i], rand());
            node::setVariation(nodes[i], rand());
            nodes[i].parents[0] = i - (size_t) ceil((-1 + sqrt(1 + 8 * (i + 1))) / 2);
            nodes[i].parents[1] = i - 1;
        }
        for(size_t input_num = 0; input_num < inputs; ++input_num) {
            nodes[(input_num * (input_num + 1)) / 2].type = 0;
        }
    };
	void set_input(size_t input_num, double value) {
		nodes[(input_num * (input_num + 1)) / 2].value = value;
	}
    double out() {
        for(auto &node : nodes) {
            node.value = tree::combine_inputs(node, nodes);
        }
        return nodes.back().value;
    }
private:
    static double combine_inputs(const node &n, const vector<node> &nodes) {
        double temp;
        switch(n.type) {
            case 0: //Input
                return n.value;
                break;
            case 1: // CONSTANT
                return n.variation;
                break;
            case 2: //COSINE
                return cos(nodes[n.parents[n.variation]].value);
                break;
            case 3: //SINE
                return sin(nodes[n.parents[n.variation]].value);
                break;
            case 4: //TANGENT
                return tan(nodes[n.parents[n.variation]].value);
                break;
            case 5: //e^x
                return exp(nodes[n.parents[n.variation]].value);
                break;
            case 6: //NATURAL LOG
                temp = nodes[n.parents[n.variation]].value;
                if(temp > 0)
                    return log(temp);
                return temp;
                break;
            case 7: //ADD
                return nodes[n.parents[n.variation]].value + nodes[n.parents[n.variation ^ 1]].value;
                break;
            case 8: //SUBTRACT
                return nodes[n.parents[n.variation]].value - nodes[n.parents[n.variation ^ 1]].value;
                break;
            case 9: //MULTIPLY
                return nodes[n.parents[n.variation]].value * nodes[n.parents[n.variation ^ 1]].value;
                break;
            case 10: //DIVIDE
                temp = nodes[n.parents[n.variation ^ 1]].value;
                if(temp != 0)
                    return nodes[n.parents[n.variation]].value / nodes[n.parents[n.variation ^ 1]].value;
                return nodes[n.parents[n.variation]].value;
                break;
            case 11: //EXPONENTIATE
                return pow(nodes[n.parents[n.variation]].value, nodes[n.parents[n.variation ^ 1]].value);
                break;
        }
        return -1;
    }
    size_t inputs;
    vector<node> nodes;
	friend class organism;
	friend class world;
	
	friend class tester;
};

class organism {
public:
	organism(size_t inputs, size_t id) : t(inputs), id(id) {}
	void set_inputs(const vector<double> &inputs) {
		for(size_t input_num = 0; input_num < t.inputs; ++input_num) {
			t.set_input(input_num, inputs[input_num % inputs.size()]);
		}
	}
	double get_output() {
		return t.out();
	}
	vector<unsigned>& get_node_types() {
		vector<unsigned> *node_types = new vector<unsigned>(t.nodes.size());
		for(size_t i = 0; i < node_types->size(); ++i) {
			(*node_types)[i] = t.nodes[i].type;
		}
		return *node_types;
	}
	vector<unsigned>& get_node_variations() {
		vector<unsigned> *node_variations = new vector<unsigned>(t.nodes.size());
		for(size_t i = 0; i < node_variations->size(); ++i) {
			(*node_variations)[i] = t.nodes[i].variation;
		}
		return *node_variations;
	}
	vector<double>& get_node_values() {
		vector<double> *node_values = new vector<double>(t.nodes.size());
		for(size_t i = 0; i < node_values->size(); ++i) {
			(*node_values)[i] = t.nodes[i].value;
		}
		return *node_values;
	}
	size_t id;
	double fitness;
private:
	tree t;
	friend class world;
	
	friend class tester;
};

struct tester {
	virtual void test(organism &o) =0;
};

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

class world {
public:
	world(size_t population_size, size_t inputs, double elite_percentile, double survival_percentile, double mutation_percentage, double mutation_rate, tester *t) : elite_percentile(elite_percentile), survival_percentile(survival_percentile), mutation_percentage(mutation_percentage), mutation_rate(mutation_rate), t(t) {
		for(size_t i = 0; i < population_size; ++i) {
			population.push_back(organism(inputs, i + 1));
		}
	}
	void measure_fitness() {
		for(auto &organism : population) {
			t->test(organism);
		}
	}
	void order_fitness() {
		sort(population.begin(), population.end(), [](organism a, organism b) {return a.fitness < b.fitness;});
	}
	void get_organisms(vector<organism> &organisms) {
		organisms = population;
	}
	void reproduce() {
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
	void mutate() {
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
private:
	double elite_percentile;
	double survival_percentile;
	double mutation_percentage;
	double mutation_rate;
	vector<organism> population;
	tester *t;
};

int main() {
    //srand(time(0));
	srand(13123);
	
	const size_t POPULATION_SIZE = 1000;
    const size_t INPUTS = 6;
	const double ELITE_PERCENTILE = .01;
	const double SURVIVAL_PERCENTILE = .3;
	const double MUTATION_PERCENTAGE = .7;
	const double MUTATION_RATE = .09;
	
	vector<double> prices;
	
	const string file_path = "intel_data.txt";
	ifstream data_file(file_path.c_str());
	while(data_file.good()) {
		prices.push_back(0);
		data_file >> prices.back();
	}
	
	/*
	//const string file_path = "IBM_adjusted_shortened.txt";
	const string file_path = "IBM_adjusted.txt";
	const size_t COLUMNS = 7;
	const size_t SELECTED_COLUMN = 3;
	ifstream data_file(file_path.c_str());
	string line;
	while(data_file.good()) {
		for(unsigned i = 0; i < COLUMNS; ++i) {
			if(i == COLUMNS - 1) { continue; }
			getline(data_file, line, ',');
			if((i % (COLUMNS - 1)) - (SELECTED_COLUMN - 1) == 0) {
				string::size_type sz;
				prices.push_back(stof(line, &sz));
			}
		}
	}
	prices.erase(prices.end() - 1);
	 */
	
	//tester *t = new price_prediction_tester(prices);
	//tester *t = new ichimoku_prediction_tester(prices);
	tester *t = new rsi_purchase_tester(prices);
	world w(POPULATION_SIZE, INPUTS, ELITE_PERCENTILE, SURVIVAL_PERCENTILE, MUTATION_PERCENTAGE, MUTATION_RATE, t);
	vector<organism> organisms;
	const unsigned ITERATIONS = 20;
	for(unsigned i = 0; i < ITERATIONS; ++i) {
		w.measure_fitness();
		w.order_fitness();
		w.get_organisms(organisms);
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
