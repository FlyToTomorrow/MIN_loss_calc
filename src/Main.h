#pragma once

#include <vector>
#include <string>
#include "BasicElements.h"

void read_radix_from_file(std::string filename, std::vector<std::vector<int>> &radix_list);
MIN_Loss_Cal* create_switch(SimParaClass &sim_para, std::vector<int> &radix);