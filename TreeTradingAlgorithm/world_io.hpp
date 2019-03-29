//
//  world_io.hpp
//  TreeTradingAlgorithm
//
//  Created by Saaketh Vangati on 3/25/19.
//  Copyright © 2019 Svang. All rights reserved.
//

#ifndef world_io_hpp
#define world_io_hpp

#include "world.hpp"

using namespace std;

class world_io {
public:
    world_io(world &w);
    void save_all();
	void save_best();
private:
    void save_all_async();
	void save_best_async();
	void save_async_helper(string &output, tree &t, node &n);
	string type_to_symbol(unsigned type);
    string output_file_path;
    world &w;
};

#endif /* world_io_hpp */
