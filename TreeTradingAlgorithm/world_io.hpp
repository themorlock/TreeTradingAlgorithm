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
    world_io(world &w, const string &file_path);
    void save_all();
private:
    void save_all_async();
	void save_all_async_helper(string &output, tree &t, node &n);
	string type_to_symbol(unsigned type);
    string file_path;
    world &w;
};

#endif /* world_io_hpp */
