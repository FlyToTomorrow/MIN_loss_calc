#include "FODON.h"

using namespace std;

ostream &operator<<(ostream &out, const PathClass &obj)
{
    return out << obj.s << ',' << obj.d << ';';
}

ostream &operator<<(ostream &out, const vector<PathClass> &path_list)
{
    if (path_list.empty())
    {
        cerr << "Error: the path list is empty" << endl;
        return out;
    }
    for (auto item: path_list)
    {
        out << item;
    }
    out << '\n';
    return out;
}

void PathClass::refresh(int stage_num, int s_arg, int d_arg)
{
    s = s_arg;
    d = d_arg;
    ss_index.resize(stage_num);
    in_port_index.resize(stage_num);
    out_port_index.resize(stage_num);
}

void copy_path(PathClass &left, const PathClass &right)
{
    left.s = right.s;
    left.d = right.d;
    left.ss_index = right.ss_index;
    left.in_port_index = right.in_port_index;
    left.out_port_index = right.out_port_index;
}

void append_path(vector<PathClass> &path_list, const PathClass &right)
{
    path_list.push_back(right);
}

FODON_Loss_Cal::FODON_Loss_Cal(
        vector<int> &radix_arg, string stage_switch_type_arg) :
        stage_switch_type(stage_switch_type_arg),
        total_loss(0), max_loss(-1), ave_loss(0),
        max_num_crossing_out(0)
{
    stage_num = radix_arg.size();
    port = accumulate(
            radix_arg.begin(), radix_arg.end(), 1, multiplies<int>());
    radix = radix_arg;
    if (stage_switch_type == "LoopedCrossbar")
    {
        for (int i = 0; i < stage_num; i++)
        {
            BasicStageSwitch *p_stage_switch =
                    new LoopedCrossbar(stage_switch_type, radix[i]);
            p_stage_switches.push_back(p_stage_switch);
        }

    }
    else if (stage_switch_type == "PILOSS")
    {
        for (int i = 0; i < stage_num; i++)
        {
            BasicStageSwitch *p_stage_switch =
                    new PILOSS(stage_switch_type, radix[i]);
            p_stage_switches.push_back(p_stage_switch);
        }
    }
    else
    {
        cerr << "Error: Incorrect stage switch type!!!" << endl;
    }

    vector<double> tmp1;
    vector<int> tmp2;
    for (int s = 0; s < port; ++s)
    {
        loss.push_back(tmp1);
        num_crossing_out.push_back(tmp2);
        for (int j = 0; j < port; ++j)
        {
            loss[s].push_back(-1);
            num_crossing_out[s].push_back(0);
        }
    }
    result_filename0 = to_string(port) + "port_FODON_" + p_stage_switches[0]->type;
    for (int i = 0; i < radix.size(); ++i)
    {
        result_filename0 += '_' + to_string(radix[i]);
    }

}

FODON_Loss_Cal::~FODON_Loss_Cal()
{
    for (auto item: p_stage_switches)
        delete item;
}

void FODON_Loss_Cal::sweep_all_loss(SimParaClass &sim_para)
{
    double loss_i;
    PathClass path;
    for (int s = 0; s < port; ++s)
    {
        for (int d = 0; d < port; ++d)
        {
            if (s != d)
            {
                loss_i = cal_loss(s, d, path, sim_para);
                // cout << loss_i << endl;
                loss[s][d] = loss_i;
                total_loss += loss_i;
                // update worst-case loss and corresponding path
                if (loss_i > max_loss)
                {
                    max_loss = loss[s][d];
                    worst_loss_paths.clear();
                    append_path(worst_loss_paths, path);
                }
                else if (loss_i == max_loss)
                {
                    append_path(worst_loss_paths, path);
                }
                // update worst-case number of crossings and corresponding path
                if (num_crossing_out[s][d] > max_num_crossing_out)
                {
                    max_num_crossing_out = num_crossing_out[s][d];
                    worst_crossing_paths.clear();
                    append_path(worst_crossing_paths, path);
                }
                else if (num_crossing_out[s][d] == max_num_crossing_out)
                {
                    append_path(worst_crossing_paths, path);
                }

            }
        }
    }
    ave_loss = total_loss / port / (port - 1);
}

double FODON_Loss_Cal::cal_loss(int s, int d, PathClass &path, SimParaClass &sim_para)
{
    // s = 0;
    // d = 672;
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
    R /= radix[stage_num - 1];
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

void FODON_Loss_Cal::write_result()
{
    ofstream result_details, result_final;
    result_details.open("./Data/" + result_filename0 + ".csv");
    if (!result_details)
    {
        std::cerr << "Error:  Can't open output file!!!" << std::endl;
    }
    else
    {
        for (int i = 0; i < port; ++i)
        {
            for (int j = 0; j < port - 1; ++j)
            {
                result_details << loss[i][j] << ',';
            }
            result_details << loss[i][port - 1] << '\n';
        }
        result_details.close();
    }
    result_final.open("./Data/" + result_filename0 + ".txt");
    if (!result_final)
    {
        std::cerr << "Error:  Can't open output file!!!" << std::endl;
    }
    else
    {
        result_final << "The worst-case loss is: " << max_loss << " dB\n";
        result_final << "The average loss is: " << ave_loss << " dB\n";
        result_final << "The number of crossings on the worst-case path is: " << max_num_crossing_out << " \n";
        result_final << "\nThe worst-case path (crossings outside stage switches) is: \n";
        result_final << worst_crossing_paths << '\n';
        result_final << "The worst-case path (loss): \n";
        result_final << worst_loss_paths << '\n';
    }
    result_final.close();
}