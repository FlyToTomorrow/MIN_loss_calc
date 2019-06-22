#include <iostream>
#include <fstream>
#include <ctime>
#include <sstream>
#include "Baseline.h"
#include "FODON.h"
#include "Main.h"

using namespace std;

int main(int argc, char *argv[])
{
    double start_s = (double) clock();
    SimParaClass sim_para;

    sim_para.GetParameterValues_FromInput(argc, argv);
    sim_para.GetParameterValues(sim_para.ParameterFileName);

    // string stage_switch_type = "PILOSS";
    // vector<int> radix = {4, 3, 4, 6};
    vector<vector<int>> radix_list;
    read_radix_from_file(sim_para.extra_setting_file, radix_list);
    MIN_Loss_Cal *p_oswitch_loss_cal;
    for (auto radix: radix_list)
    {
        p_oswitch_loss_cal = create_switch(sim_para, radix);
        p_oswitch_loss_cal->sweep_all_loss(sim_para);
        p_oswitch_loss_cal->write_result();
        delete (p_oswitch_loss_cal);
    }

    double stop_s = (double) clock();
    cout << "\n\n******* Execution Time is: " <<
         (stop_s - start_s) / double(CLOCKS_PER_SEC) <<
         " s *******" << endl;
    return 0;
}

void read_radix_from_file(string filename, vector<vector<int>> &radix_list)
{
    ifstream radix_file(filename + ".cfg");
    if (!radix_file)
    {
        std::cerr << "Error:  Can't open file to read radix info!!!" << std::endl;
    }
    else
    {
        radix_list.clear();
        string line, str;
        int index = 0;

        while (getline(radix_file, line))
        {
            if (line.size() > 2 && line.compare(0, 2, "//")
                && line[0] != '#' && line.compare(0, 2, "  "))
            {
                stringstream ss(line);
                radix_list.push_back(vector<int>());
                while (getline(ss, str, ','))
                    radix_list[index].push_back(stoi(str));
                index++;
            }
        }
        // cout << "test" << endl;
    }
}

MIN_Loss_Cal* create_switch(SimParaClass &sim_para, vector<int> &radix)
{
    if (sim_para.switch_type == "Baseline")
    {
        return new Baseline_Loss_Cal(
                radix, sim_para.switch_type, sim_para.stage_switch_type);
    }
    else if (sim_para.switch_type == "FODON")
    {
        return new FODON_Loss_Cal(
                radix, sim_para.switch_type, sim_para.stage_switch_type);
    }
    else{
        cerr << "Error: Please specify correct switch type!!!" << endl;
        exit(82);
    }
}