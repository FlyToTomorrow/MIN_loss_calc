#pragma once

#include <vector>
#include <numeric>
#include <algorithm>
#include <string>
#include "Baseline.h"





class FODON_Loss_Cal: public MIN_Loss_Cal
{
public:
    FODON_Loss_Cal(std::vector<int> &radix_arg,
                   std::string switch_type_arg, std::string stage_switch_type_arg):
                   MIN_Loss_Cal(radix_arg, switch_type_arg, stage_switch_type_arg){};
    ~FODON_Loss_Cal(){};
    double cal_loss(int s, int d, PathClass &path, SimParaClass &sim_para);
    double cal_loss(std::vector<int> &ss_index_wobias,
                    PathClass &path,
                    SimParaClass &sim_para,
                    int &num_crossing_out);
    void get_path(int s, int d, PathClass &path, std::vector<int> &ss_index_wobias);
};

