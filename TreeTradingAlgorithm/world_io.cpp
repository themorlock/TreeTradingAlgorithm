//
//  world_io.cpp
//  TreeTradingAlgorithm
//
//  Created by Saaketh Vangati on 3/25/19.
//  Copyright © 2019 Svang. All rights reserved.
//

#include <string>
#include <thread>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

#include "world_io.hpp"

using namespace std;

world_io::world_io(world &w) : w(w) {
	auto start_time = time(nullptr);
	stringstream st;
	st << put_time(localtime(&start_time), "%F_%T");
	auto start_time_string = st.str();
	replace(start_time_string.begin(), start_time_string.end(), ':', '-');
	output_file_path = st.str() + ".txt";
}

void world_io::save_all() {
    thread(&world_io::save_all_async, this).detach();
}

void world_io::save_all_async() {
    ofstream file;
    file.open(output_file_path, ios::app);
	for(auto &organism : w.population) {
		string output = "";
		save_all_async_helper(output, organism.t, organism.t.nodes.back());
		file << organism.id << ": " << output << ", Genome Length = " << organism.t.nodes.size() << ", Fitness = " << organism.fitness << "." << endl;
	}
	file << endl;
    file.close();
}

void world_io::save_all_async_helper(string &output, tree &t, node &n) {
	if(n.type <= 1) {
		switch(n.type) {
			case 0:
				output += type_to_symbol(n.type);
				break;
			case 1:
				output += to_string(n.value);
				break;
		}
	}else if(n.type <= 6) {
		output += type_to_symbol(n.type) + "(";
		save_all_async_helper(output, t, t.nodes[n.parents[n.variation]]);
		output += ")";
	}else if(n.type <= 11) {
		output += "(";
		save_all_async_helper(output, t, t.nodes[n.parents[n.variation]]);
		output += type_to_symbol(n.type);
		save_all_async_helper(output, t, t.nodes[n.parents[n.variation ^ 1]]);
		output += ")";
	}
	
}

string world_io::type_to_symbol(unsigned type) {
	switch(type) {
		case 0: //Input
			return "x";
			break;
		case 1: // CONSTANT
			return "";
			break;
		case 2: //COSINE
			return "cos";
			break;
		case 3: //SINE
			return "sin";
			break;
		case 4: //TANGENT
			return "tan";
			break;
		case 5: //e^x
			return "e^";
			break;
		case 6: //NATURAL LOG
			return "ln";
			break;
		case 7: //ADD
			return "+";
			break;
		case 8: //SUBTRACT
			return "-";
			break;
		case 9: //MULTIPLY
			return "*";
			break;
		case 10: //DIVIDE
			return "/";
			break;
		case 11: //EXPONENTIATE
			return "^";
			break;
	}
	return "";
}
