#pragma once

#include <vector>
#include <numeric>
#include <algorithm>
#include <string>
#include "StageSwitch.h"


class PathClass;
void copy_path(PathClass &left, const PathClass &right);
void append_path(std::vector<PathClass> &path_list, const PathClass &right);
// void print_paths(std::ostream out, std::vector<PathClass> &path_list);

class PathClass
{
public:
    int s, d;
    std::vector<int> ss_index, in_port_index, out_port_index;

    PathClass(int s_arg, int d_arg) :
            s(s_arg), d(d_arg)
    {
        ss_index.clear();
        in_port_index.clear();
        out_port_index.clear();
    }

    PathClass() : s(-1), d(-1)
    {
        ss_index.clear();
        in_port_index.clear();
        out_port_index.clear();
    }

    ~PathClass(){};

    void refresh(int stage_num, int s_arg=-1, int d_arg=-1);


};



class FODON_Loss_Cal
{
private:
    /* data */
    std::vector<PathClass> worst_crossing_paths;
    std::vector<PathClass> worst_loss_paths;

public:
    int port, stage_num;
    std::vector<int> radix;
    std::string stage_switch_type;
    std::vector<BasicStageSwitch *> p_stage_switches;
    std::vector<std::vector<double>> loss;
    std::vector<std::vector<int>> num_crossing_out;
    double total_loss, max_loss, ave_loss;
    int max_num_crossing_out;
    std::string result_filename0;

    FODON_Loss_Cal(std::vector<int> &radix_arg,
                   std::string stage_switch_type_arg);
    ~FODON_Loss_Cal();
    void sweep_all_loss(SimParaClass &sim_para);
    double cal_loss(int s, int d, PathClass &path, SimParaClass &sim_para);
    // double cal_loss(std::vector<int> &ss_index_wobias,
    //                 std::vector<int> &in_port_index,
    //                 std::vector<int> &out_port_index,
    //                 SimParaClass &sim_para,
    //                 int &num_crossing_out);
    double cal_loss(std::vector<int> &ss_index_wobias,
                    PathClass &path,
                    SimParaClass &sim_para,
                    int &num_crossing_out);
    // void get_path(int s, int d, std::vector<int> &ss_index, std::vector<int> &ss_index_wobias,
    //               std::vector<int> &in_port_index, std::vector<int> &out_port_index);
    void get_path(int s, int d, PathClass &path, std::vector<int> &ss_index_wobias);
    void write_result();
};

