#include "FODON.h"

using namespace std;



double FODON_Loss_Cal::cal_loss(int s, int d, PathClass &path, SimParaClass &sim_para)
{
    // s = 1039;
    // d = 0;
    vector<int> ss_index_wobias;
    get_path(s, d, path, ss_index_wobias);
    return cal_loss(ss_index_wobias, path, sim_para, num_crossing_out[s][d]);
}

double FODON_Loss_Cal::cal_loss(vector<int> &ss_index_wobias,
                                PathClass &path,
                                SimParaClass &sim_para,
                                int &num_crossing_out)
{
    // int Li_num = 0, Lp_num = 0, Lc_num = 0;
    double loss = 0;
    // calculate the loss inside stage switches
    for (int i = 0; i < stage_num; ++i)
    {
        loss += p_stage_switches[i]->cal_loss(
                path.in_port_index[i], path.out_port_index[i], sim_para);
    }
    num_crossing_out = 0;
    int crossing_num;
    int R;
    // calculate the total number of crossings outside stage switches
    // first stage <-> second stage
    R = port / 2 / radix[0];
    crossing_num = ss_index_wobias[0] % R * (
            (radix[0] / 2) - 1 - path.out_port_index[0] % (radix[0] / 2))
                   + (R - 1 - ss_index_wobias[0] % R)
                     * (path.out_port_index[0] % (radix[0] / 2));
    num_crossing_out += crossing_num;
    // stage 1 to (n-3)
    for (int i = 1; i < stage_num - 2; i++)
    {
        R /= radix[i];
        crossing_num = ss_index_wobias[i]%R * (radix[i] - 1 - path.out_port_index[i])
                       + (R - 1 - ss_index_wobias[i]%R) * path.out_port_index[i];
        num_crossing_out += crossing_num;
    }
    // stage (n-2) <-> stage (n-3)
    R /= radix[stage_num - 2];
    crossing_num = ss_index_wobias[stage_num - 2]%R
                   * (radix[stage_num - 2] - 1 - path.out_port_index[stage_num - 2])
                   + ((radix[stage_num - 1] / 2) - 1 - ss_index_wobias[stage_num - 2]%R)
                     * path.out_port_index[stage_num - 2];
    num_crossing_out += crossing_num;

    loss += sim_para.Lc_dB * num_crossing_out;
    return loss;

}

void FODON_Loss_Cal::get_path(int s, int d, PathClass &path, vector<int> &ss_index_wobias)
{
    int residual_s = s;
    int residual_d = d;
    int bias = 0;
    int factor = port / radix[0];
    path.refresh(stage_num, s, d);
    ss_index_wobias.resize(stage_num);
    for (int i = 0; i < stage_num - 1; i++)
    {
        ss_index_wobias[i] = residual_s / radix[i];
        path.ss_index[i] = residual_s / radix[i] + bias;
        path.in_port_index[i] = residual_s % radix[i];
        path.out_port_index[i] = residual_d / factor;
        residual_s /= radix[i];
        residual_d %= factor;
        factor /= radix[i + 1];
        bias = path.ss_index[i] * radix[i] / radix[i + 1] + path.out_port_index[i] * factor;    // ToDo: check further
    }
    ss_index_wobias[stage_num - 1] = residual_s / radix[stage_num - 1];
    path.ss_index[stage_num - 1] = residual_s / radix[stage_num - 1] + bias;
    path.in_port_index[stage_num - 1] = residual_s % radix[stage_num - 1];
    path.out_port_index[stage_num - 1] = residual_d / factor;
}