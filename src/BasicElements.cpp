#include "BasicElements.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
//#include <algorithm>
using namespace std;

// ************************	Fundamental ************************	
bool ReadParameterFile(const string FileName, map<string, string> &parameter_list)
{
    ifstream ParameterFile(FileName);
    string line;
    string id, eq, val;
    if (ParameterFile.is_open())
    {
        while (getline(ParameterFile, line))
        {
            if (!line.compare(0, 2, "//") || !line.compare(0, 2, "\r\n") || line[0] == '\r' || line[0] == '\n' ||
                line[0] == '\0' || line[0] == ' ' || line[0] == '#' || line[0] == ';')
                continue;
            else
            {
                istringstream is_line(line);
                string key;
                if (getline(is_line, key, '='))
                {
                    string value;
                    if (getline(is_line, value, ';'))
                    {
                        key.erase(remove_if(key.begin(), key.end(), ::isspace), key.end());
                        value.erase(remove_if(value.begin(), value.end(), ::isspace), value.end());
                        //cout << key.compare("Li_dB") << endl;
                        parameter_list.insert(pair<string, string>(key, value));
                    }
                }
            }
        }
        ParameterFile.close();
        return true;
    }
    else
    {
        cerr << "Unable to open file " << FileName << " !!!" << endl;
        return false;
    }

}

// ======================= SimParaClass related functions =======================
void SimParaClass::GetParameterValues_FromInput(int argc, char *argv[])
{
    // ************************	handle the input parameters ************************
    switch (argc)
    {
        case 4:
            extra_setting_file = argv[3];
        case 3:
            ParameterFileName = argv[2];
        case 2:
            stage_switch_type = argv[1];
        case 1:
            break;
        default:
            std::cerr << "Usage: " << argv[0] << "SOURCE DESTINATION" << std::endl;
            exit(1000);
    }
}

void SimParaClass::ConfirmParameterValues_FromInput()
{
    if (sim_type == 3)    // sweep performance with the injection rate
        cout << "\n************** The Given parameters are **************\n" <<
             "Sweep injection rates:" <<
             "\nswitch type = " << switch_type <<
             "\ntraffic pattern type = " << traffic_pattern_type <<
             "\npacket length type = " << pkt_len_type <<
             "\nPort = " << SwitchPort <<
             "\nAverage pkt length = " << AveLen / 8 << " Bytes" <<
             "\nSetup time = " << SetupTime << " Cycles" <<
             "\nInjection rate start = " << injection_rate_start / 1e9 << " Gbps" <<
             "\nInjection rate stop = " << injection_rate_end / 1e9 << " Gbps" <<
             "\n# of sweeping points = " << injection_rate_num <<
             "\n# of parallel jobs= " << job_num <<
             "\nSimulation time = " << sim_time << " cycles" <<
             "\nParameterFileName = " << ParameterFileName <<
             "\nOutputFileName0 = " << OutputFileName0 <<
             "\nlaser_power_calculation_type = " << laser_power_calculation_type <<
             "\nis_serdes_idle_power_flag = " << is_serdes_idle_power_flag <<
             "\nis_APC_flag = " << is_APC_flag <<
             "\nis_static_tuning = " << is_static_tuning <<
             "\n******************************************************\n" << endl;
    else if (sim_type == 2) // obtain the performance with a specific injection rate
        cout << "\n************** The Given parameters are **************\n" <<
             "Simulate performance under specific injection rate:" <<
             "\nswitch_type = " << switch_type <<
             "\ntraffic pattern type = " << traffic_pattern_type <<
             "\npacket length type = " << pkt_len_type <<
             "\nPort = " << SwitchPort <<
             "\nSetup time = " << SetupTime << " Cycles" <<
             "\nAverage pkt length = " << AveLen / 8 << " Bytes" <<
             "\nInjection rate = " << injection_rate / 1e9 << " Gbps" <<
             "\nSimulation time = " << sim_time << " cycles" <<
             "\nParameterFileName = " << ParameterFileName <<
             "\nOutputFileName0 = " << OutputFileName0 <<
             "\nlaser_power_calculation_type = " << laser_power_calculation_type <<
             "\nis_serdes_idle_power_flag = " << is_serdes_idle_power_flag <<
             "\nis_APC_flag = " << is_APC_flag <<
             "\nis_static_tuning = " << is_static_tuning <<
             "\n******************************************************\n" << endl;
    else if (sim_type == 1) // obtain the crosstalk based on the Ment Caro simulation
        cout << "\n************** The Given parameters are **************\n" <<
             "Simulate Crosstalk:" <<
             "\nswitch_type = " << switch_type <<
             "\nPort = " << SwitchPort <<
             "\nLoop =" << Loop <<
             "\nParameterFileName = " << ParameterFileName <<
             "\nOutputFileName0 = " << OutputFileName0 <<
             "\nlaser_power_calculation_type = " << laser_power_calculation_type <<
             "\nis_serdes_idle_power_flag = " << is_serdes_idle_power_flag <<
             "\nis_APC_flag = " << is_APC_flag <<
             "\nis_static_tuning = " << is_static_tuning <<
             "\n******************************************************\n" << endl;
}

void SimParaClass::GetParameterValues(const string FileName)
{
    map<string, string> parameter_list;

    if (ReadParameterFile(FileName + ".cfg", parameter_list))
    {
        // traffic pattern and pkt_len type
        traffic_pattern_type =
                PopValue("traffic_pattern_type", parameter_list);
        hot_prob = stof(PopValue("hot_prob", parameter_list));
        // switch basic configuration
        StageSwitch_Radix =
                stoi(PopValue("stage_switch_radix", parameter_list));
        sweeping_type = stoi(PopValue("sweeping_type", parameter_list));
        laser_power_calculation_type = stoi(PopValue("laser_power_calculation_type", parameter_list));
        is_serdes_idle_power_flag = to_bool(PopValue("is_serdes_idle_power_flag", parameter_list));
        is_APC_flag = to_bool(PopValue("is_APC_flag", parameter_list));
        is_static_tuning = to_bool(PopValue("is_static_tuning", parameter_list));
        if (!extra_setting_file.size())
            extra_setting_file = PopValue("extra_setting_file", parameter_list, true);
        //trace_length = stoul(PopValue("trace_length", parameter_list, true));

        // pkt size related
        pkt_len_type = PopValue("pkt_len_type", parameter_list);
        AveLen = stoi(PopValue("AveLen", parameter_list)) * 8;
        pkt_max_size = stoi(PopValue("pkt_max_size", parameter_list)) * 8;

        // crosstalk related
        Li_dB = stof(PopValue("Li_dB", parameter_list));
        Lp_dB = stof(PopValue("Lp_dB", parameter_list));
        Lc_dB = stof(PopValue("Lc_dB", parameter_list));
        Lgc_dB = stof(PopValue("Lgc_dB", parameter_list));
        Lwg_dB = stof(PopValue("Lwg_dB", parameter_list));
        Lm_dB = stof(PopValue("Lm_dB", parameter_list));
        Xd_dB = stof(PopValue("Xd_dB", parameter_list));
        Xt_dB = stof(PopValue("Xt_dB", parameter_list));
        Xc_dB = stof(PopValue("Xc_dB", parameter_list));
        Ps_dB = stof(PopValue("Ps_dB", parameter_list));
        Li = dB2ratio(Li_dB);
        Lp = dB2ratio(Lp_dB);
        Lc = dB2ratio(Lc_dB);
        Lgc = dB2ratio(Lgc_dB);
        Lwg = dB2ratio(Lwg_dB);
        Lm = dB2ratio(Lm_dB);
        Xd = dB2ratio(Xd_dB);
        Xt = dB2ratio(Xt_dB);
        Xc = dB2ratio(Xc_dB);
        Ps = dB2ratio(Ps_dB);
        //performance related
        BR0 = stof(PopValue("BR0", parameter_list));
        WDM_num = stoi(PopValue("WDM_num", parameter_list));
        ClkFrequency = stof(PopValue("ClkFrequency", parameter_list));
        MaxBufferLen = stoi(PopValue("MaxBufferLen", parameter_list));
        VIQ_depth = stoi(PopValue("VIQ_depth", parameter_list));
        VOQ_num = stoi(PopValue("VOQ_number", parameter_list));
        VIQ_num = stoi(PopValue("VIQ_number", parameter_list));
        FlitSize = stoi(PopValue("FlitSize", parameter_list));
        fiber_len = stof(PopValue("fiber_len", parameter_list));
        EOE_delay = stof(PopValue("EOE_delay", parameter_list));
        Control_delay_cycle =
                stoi(PopValue("Control_delay_cycle", parameter_list));
        Switch_on_off_delay =
                stof(PopValue("Switch_on_off_delay", parameter_list));
        propagation_delay = fiber_len / (speed_of_light / n_SiO2);
        Laser_tuning_delay =
                stof(PopValue("Laser_tuning_delay", parameter_list));
        rearrange_time_in_cycle =
                stoi(PopValue("rearrange_time_in_cycle", parameter_list));
        Synchronization_delay =
                stof(PopValue("synchronization_delay", parameter_list));

        injection_rate_start =
                stof(PopValue("injection_rate_start", parameter_list));
        injection_rate_end =
                stof(PopValue("injection_rate_end", parameter_list));

        // energy related
        energy_para.MR_num_per_group =
                stoi(PopValue("MR_num_per_group", parameter_list));
        energy_para.sensitivity = Ps;
        energy_para.slope_efficiency =
                stof(PopValue("slope_efficiency", parameter_list));
        energy_para.laser_current_threshold =
                stof(PopValue("laser_current_threshold", parameter_list));
        energy_para.laser_voltage =
                stof(PopValue("laser_voltage", parameter_list));
        energy_para.laser_tuning_power =
                stof(PopValue("laser_tuning_power", parameter_list));
        energy_para.switch_on_power =
                stof(PopValue("switch_on_power", parameter_list));
        energy_para.serdes_dynamic_power =
                stof(PopValue("serdes_dynamic_power", parameter_list));
        energy_para.serdes_static_power =
                stof(PopValue("serdes_static_power", parameter_list));
        energy_para.modulator_static_power =
                stof(PopValue("modulator_static_power", parameter_list));
        energy_para.receiver_static_power =
                stof(PopValue("receiver_static_power", parameter_list));
        energy_para.modulator_dynamic_power =
                stof(PopValue("modulator_dynamic_power", parameter_list));
        energy_para.receiver_dynamic_power =
                stof(PopValue("receiver_dynamic_power", parameter_list));
        energy_para.tuning_power =
                stof(PopValue("tuning_power", parameter_list));
        energy_para.control_dynamic_power =
                stof(PopValue("control_dynamic_power", parameter_list));
        energy_para.control_static_power =
                stof(PopValue("control_static_power", parameter_list));
        energy_para.Q_dynamic_power =
                stof(PopValue("Q_dynamic_power", parameter_list));
        energy_para.Q_static_power =
                stof(PopValue("Q_static_power", parameter_list));
        energy_para.transmission_time_o = (ceil(AveLen / BR * ClkFrequency)) / ClkFrequency;    // ns
        energy_para.transmission_time_e = (ceil(AveLen / FlitSize)) / ClkFrequency;    // ns

        energy_para.Control_delay_cycle = Control_delay_cycle;
        energy_para.laser_plugin_efficiency =
                stof(PopValue("laser_plugin_efficiency", parameter_list));

    }
    else
        exit(-4);
}

void SimParaClass::PreProcessing()
/*-----------------------
Calculate the setup time
generate the OutputFileName0 (root of the outpufile name)
-----------------------	*/
{

    if (switch_type == "AWG_single" || switch_type == "AWGmm")
    {
        SetupTime = max((Time) ceil((2 * (fiber_len / (speed_of_light / n_SiO2) +
                                          EOE_delay)) * ClkFrequency) + Control_delay_cycle,
                        (Time) ceil(Laser_tuning_delay * ClkFrequency))
                    + (Time) ceil(Synchronization_delay * ClkFrequency);
        BR = BR0;
    }
    else
    {
        SetupTime = (Time) ceil((Synchronization_delay + fiber_len / (speed_of_light / n_SiO2)
                                 + EOE_delay + max(fiber_len / (speed_of_light / n_SiO2)
                                                   + EOE_delay, Switch_on_off_delay)) * ClkFrequency)
                    + Control_delay_cycle;
        BR = BR0 * WDM_num;
    }
    // ************************ output file setting ************************
    if (OutputFileName_fromInput!="")
    {
        OutputFileName0 = OutputFileName_fromInput;
    }
    else
    {
        if (sim_type == 1)    // obtain crosstalk
            OutputFileName0 = to_string(SwitchPort) + 'x'
                              + to_string(SwitchPort) + '_' + switch_type
                              + '_' + to_string(AveLen / 8) + '_' + to_string(SetupTime)
                              + "_Xtalk";
        else if (sim_type == 2 || sim_type == 3) // obtain performance
        {
            if (switch_type == "Baseline" || switch_type == "FODON"
                || switch_type == "Benes")
            {
                if (traffic_pattern_type == "HotSpot" || traffic_pattern_type == "HotPair")
                {
                    OutputFileName0 = to_string(SwitchPort) + 'x' + to_string(SwitchPort)
                                      + '_' + to_string(StageSwitch_Radix) + "radix_" + switch_type
                                      + '_' + to_string(WDM_num)
                                      + '_' + to_string(AveLen / 8)
                                      + '_' + to_string(SetupTime)
                                      + '_' + to_string(hot_prob).substr(0, 4) + traffic_pattern_type
                                      + '_' + pkt_len_type;
                }
                else
                {
                    OutputFileName0 = to_string(SwitchPort) + 'x' + to_string(SwitchPort)
                                      + '_' + to_string(StageSwitch_Radix) + "radix_" + switch_type
                                      + '_' + to_string(WDM_num)
                                      + '_' + to_string(AveLen / 8)
                                      + '_' + to_string(SetupTime)
                                      + '_' + traffic_pattern_type
                                      + '_' + pkt_len_type;
                }
            }
            else
            {
                if (traffic_pattern_type == "HotSpot" || traffic_pattern_type == "HotPair")
                {
                    OutputFileName0 = to_string(SwitchPort) + 'x' + to_string(SwitchPort)
                                      + '_' + switch_type
                                      + '_' + to_string(WDM_num)
                                      + '_' + to_string(AveLen / 8)
                                      + '_' + to_string(SetupTime)
                                      + '_' + to_string(hot_prob).substr(0, 4) + traffic_pattern_type
                                      + '_' + pkt_len_type;
                }
                else
                {
                    OutputFileName0 = to_string(SwitchPort) + 'x' + to_string(SwitchPort)
                                      + '_' + switch_type
                                      + '_' + to_string(WDM_num)
                                      + '_' + to_string(AveLen / 8)
                                      + '_' + to_string(SetupTime)
                                      + '_' + traffic_pattern_type
                                      + '_' + pkt_len_type;
                }
            }
        }
        else if (sim_type == 4) // obtain performance under real traffics
        {
            if (switch_type == "Baseline" || switch_type == "FODON"
                || switch_type == "Benes")
            {
                OutputFileName0 = to_string(SwitchPort) + 'x' + to_string(SwitchPort)
                                  + '_' + to_string(StageSwitch_Radix) + "radix_" + switch_type
                                  + '_' + to_string(WDM_num)
                                  + '_' + to_string(SetupTime)
                                  + '_' + trace_filename;
            }
            else
            {
                OutputFileName0 = to_string(SwitchPort) + 'x' + to_string(SwitchPort)
                                  + '_' + switch_type
                                  + '_' + to_string(WDM_num)
                                  + '_' + to_string(SetupTime)
                                  + '_' + trace_filename;
            }

        }
        else
        {
            cerr << "Error: Please give valid simulation type!!!" << endl;
            exit(358);
        }
    }

}

void SimParaClass::CheckParameterValues()
{
    ofstream output("Parameter_Checking_" + ParameterFileName + ".txt");
    output << "************ Basic seeting ***************" << endl;
    output << "traffic type = " << traffic_pattern_type << ";" << endl;
    output << "hotspot probability = " << hot_prob << ";" << endl;
    output << "packet length type = " << pkt_len_type << ";" << endl;
    output << "sweeping_type = " << sweeping_type << ";" << endl;
    output << "laser_power_calculation_type = " << laser_power_calculation_type << ";" << endl;
    output << "is_serdes_idle_power_flag = " << is_serdes_idle_power_flag << ";" << endl;
    output << "is_APC_flag = " << is_APC_flag << ";" << endl;
    output << "rearrange_time_in_cycle = " << rearrange_time_in_cycle << ";" << endl;

    output << "************ Loss and crosstalk related ***************" << endl;
    output << "Li_dB = " << Li_dB << " dB;" << endl;
    output << "Lp_dB = " << Lp_dB << " dB;" << endl;
    output << "Lc_dB = " << Lc_dB << " dB;" << endl;
    output << "Lm_dB = " << Lm_dB << " dB;" << endl;
    output << "Xd_dB = " << Xd_dB << " dB;" << endl;
    output << "Xt_dB = " << Xt_dB << " dB;" << endl;
    output << "Xc_dB = " << Xc_dB << " dB;" << endl;
    output << "Ps_dB = " << Ps_dB << " dB;" << endl << endl;
    output << "Li = " << Li << ";" << endl;
    output << "Lp = " << Lp << ";" << endl;
    output << "Lc = " << Lc << ";" << endl;
    output << "Lm = " << Lm << ";" << endl;
    output << "Xd = " << Xd << ";" << endl;
    output << "Xt = " << Xt << ";" << endl;
    output << "Xc = " << Xc << ";" << endl;
    output << "Ps = " << Ps << ";" << endl;

    output << "********* Performance related ************" << endl;
    output << "sweeping_type = " << sweeping_type << ";" << endl;
    output << "WDM_num = " << WDM_num << ";" << endl;
    output << "BR0 = " << BR0 / 1e9 << " Gbps;" << endl;
    output << "BR = " << BR / 1e9 << " Gbps;" << endl;
    output << "ClkFrequency = " << ClkFrequency / 1e9 << " GHz;" << endl;
    output << "SetupTime = " << SetupTime << " Cycles;" << endl;
    output << "AveLen = " << AveLen / 8 << " Bytes;" << endl;
    output << "MaxBufferLen = " << MaxBufferLen << " packets;" << endl;
    output << "propagation_delay = " << propagation_delay * 1e9 << " ns;" << endl;

    output << "********* Energy related ************" << endl;
    output << "MR_num_per_group = " << energy_para.MR_num_per_group << ";" << endl;
    switch (laser_power_calculation_type)
    {
        case 1:
            output << "slope_efficiency = " << energy_para.slope_efficiency << " W/A;" << endl;
            output << "laser_current_threshold = " << energy_para.laser_current_threshold << " mA;" << endl;
            output << "laser_voltage = " << energy_para.laser_voltage << " V;" << endl;
            break;
        case 2:
            output << "laser plugin efficiency = " << energy_para.laser_plugin_efficiency << " W/A;" << endl;
            break;
        default:
            break;
    }
    output << "switch_on_power = " << energy_para.switch_on_power << " mW;" << endl;
    output << "serdes_dynamic_power = " << energy_para.serdes_dynamic_power << " mW;" << endl;
    output << "serdes_static_power = " << energy_para.serdes_static_power << " mW;" << endl;
    output << "modulator_static_power = " << energy_para.modulator_static_power << " mW;" << endl;
    output << "modulator_dynamic_power = " << energy_para.modulator_dynamic_power << " mW;" << endl;
    output << "receiver_static_power = " << energy_para.receiver_static_power << " mW;" << endl;
    output << "receiver_dynamic_power = " << energy_para.receiver_dynamic_power << " mW;" << endl;
    output << "tuning_power = " << energy_para.tuning_power << " mW;" << endl;
    output << "control_dynamic_power = " << energy_para.control_dynamic_power << " mW;" << endl;
    output << "control_static_power = " << energy_para.control_static_power << " mW;" << endl;
    output << "Q_dynamic_power = " << energy_para.Q_dynamic_power << " mW;" << endl;
    output << "Q_static_power = " << energy_para.Q_static_power << " mW;" << endl;
    output << "***************** END ********************" << endl;
}

// ************************	PerformanceResultElement related functions ************************
void PerformanceResultElement::Update(Time current_time, Time generate_time, size_t length)
{
    Time time_difference = current_time - generate_time;
    total_latency += time_difference;
    if (time_difference > max_latency)
        max_latency = time_difference;
    total_pkt += 1;
    total_data += length;
    //cout << time_difference << endl;
}

void WriteSweepResultToFile(const SimParaClass sim_para, const ResultClass *p_sim_result)
{
    ofstream SweepDataFile_csv;
    //ofstream SweepDataFile_txt;
    SweepDataFile_csv.open(sim_para.Path + sim_para.OutputFileName0 + "_Sweep.csv");
    if (!SweepDataFile_csv)
    {
        std::cerr << "Error:  Can't open output file!!!" << std::endl;
    }
    else
    {
        SweepDataFile_csv << "injection_rate," <<
                          "offered_load," <<
                          "latency," <<
                          "throughput," <<
                          "max_latency," <<
                          "total_pkt," <<
                          "total_data," <<
                          "ave_Qsize," <<

                          "max_loss," <<
                          "ave_loss," <<

                          "laser_dynamic," <<
                          "laser_static," <<
                          "laser_tuning," <<
                          "laser_plugin," <<
                          "serdes_dynamic," <<
                          "serdes_static," <<
                          "modulator_dynamic," <<
                          "modulator_static," <<
                          "receiver_dynamic," <<
                          "receiver_static," <<
                          "switch_static," <<
                          "tuning," <<
                          "control_dynamic," <<
                          "control_static," <<
                          "Q_dynamic," <<
                          "Q_static," <<
                          "ave_power," <<
                          "efficiency";

        SweepDataFile_csv << '\n';
        for (int i = 0; i < sim_para.injection_rate_num; i++)
        {
            SweepDataFile_csv << p_sim_result[i].performance_result.injection_rate << ",";
            SweepDataFile_csv << p_sim_result[i].performance_result.offered_load << ",";
            SweepDataFile_csv << p_sim_result[i].performance_result.ave_latency << ",";
            SweepDataFile_csv << p_sim_result[i].performance_result.ave_throughput << ",";
            SweepDataFile_csv << p_sim_result[i].performance_result.max_latency << ",";
            SweepDataFile_csv << p_sim_result[i].performance_result.total_pkt << ",";
            SweepDataFile_csv << p_sim_result[i].performance_result.total_data << ",";
            SweepDataFile_csv << p_sim_result[i].performance_result.ave_Qsize << ",";

            SweepDataFile_csv << p_sim_result[i].energy_result.max_loss << ",";
            SweepDataFile_csv << p_sim_result[i].energy_result.ave_loss << ",";

            SweepDataFile_csv << p_sim_result[i].energy_result.laser_dynamic << ",";
            SweepDataFile_csv << p_sim_result[i].energy_result.laser_static << ",";
            SweepDataFile_csv << p_sim_result[i].energy_result.laser_tuning << ",";
            SweepDataFile_csv << p_sim_result[i].energy_result.laser_plugin << ",";
            SweepDataFile_csv << p_sim_result[i].energy_result.serdes_dynamic << ",";
            SweepDataFile_csv << p_sim_result[i].energy_result.serdes_static << ",";
            SweepDataFile_csv << p_sim_result[i].energy_result.modulator_dynamic << ",";
            SweepDataFile_csv << p_sim_result[i].energy_result.modulator_static << ",";
            SweepDataFile_csv << p_sim_result[i].energy_result.receiver_dynamic << ",";
            SweepDataFile_csv << p_sim_result[i].energy_result.receiver_static << ",";
            SweepDataFile_csv << p_sim_result[i].energy_result.switch_static << ",";
            SweepDataFile_csv << p_sim_result[i].energy_result.tuning << ",";
            SweepDataFile_csv << p_sim_result[i].energy_result.control_dynamic << ",";
            SweepDataFile_csv << p_sim_result[i].energy_result.control_static << ",";
            SweepDataFile_csv << p_sim_result[i].energy_result.Q_dynamic << ",";
            SweepDataFile_csv << p_sim_result[i].energy_result.Q_static << ",";
            SweepDataFile_csv << p_sim_result[i].energy_result.ave_power << ",";
            SweepDataFile_csv << p_sim_result[i].energy_result.efficiency;

            SweepDataFile_csv << "\n";
        }
        SweepDataFile_csv.close();
    }
}

void WriteRealTrafficResultToFile(const SimParaClass sim_para, const ResultClass *p_sim_result)
{
    ofstream result_filename;
    //ofstream SweepDataFile_txt;
    result_filename.open(sim_para.Path + sim_para.OutputFileName0 + "_RealTraffic.csv");
    if (!result_filename)
    {
        std::cerr << "Error:  Can't open output file!!!" << std::endl;
    }
    else
    {
        result_filename << "trace_filename," <<
                        "latency," <<
                        "throughput," <<
                        "max_latency," <<
                        "total_pkt," <<
                        "total_data," <<
                        "ave_Qsize," <<

                        "max_loss," <<
                        "ave_loss," <<

                        "laser_dynamic," <<
                        "laser_static," <<
                        "laser_tuning," <<
                        "laser_plugin," <<
                        "serdes_dynamic," <<
                        "serdes_static," <<
                        "modulator_dynamic," <<
                        "modulator_static," <<
                        "receiver_dynamic," <<
                        "receiver_static," <<
                        "switch_static," <<
                        "tuning," <<
                        "control_dynamic," <<
                        "control_static," <<
                        "Q_dynamic," <<
                        "Q_static," <<
                        "ave_power," <<
                        "efficiency";

        result_filename << '\n';

        result_filename << sim_para.trace_filename << ",";

        result_filename << p_sim_result->performance_result.ave_latency << ",";
        result_filename << p_sim_result->performance_result.ave_throughput << ",";
        result_filename << p_sim_result->performance_result.max_latency << ",";
        result_filename << p_sim_result->performance_result.total_pkt << ",";
        result_filename << p_sim_result->performance_result.total_data << ",";
        result_filename << p_sim_result->performance_result.ave_Qsize << ",";

        result_filename << p_sim_result->energy_result.max_loss << ",";
        result_filename << p_sim_result->energy_result.ave_loss << ",";

        result_filename << p_sim_result->energy_result.laser_dynamic << ",";
        result_filename << p_sim_result->energy_result.laser_static << ",";
        result_filename << p_sim_result->energy_result.laser_tuning << ",";
        result_filename << p_sim_result->energy_result.laser_plugin << ",";
        result_filename << p_sim_result->energy_result.serdes_dynamic << ",";
        result_filename << p_sim_result->energy_result.serdes_static << ",";
        result_filename << p_sim_result->energy_result.modulator_dynamic << ",";
        result_filename << p_sim_result->energy_result.modulator_static << ",";
        result_filename << p_sim_result->energy_result.receiver_dynamic << ",";
        result_filename << p_sim_result->energy_result.receiver_static << ",";
        result_filename << p_sim_result->energy_result.switch_static << ",";
        result_filename << p_sim_result->energy_result.tuning << ",";
        result_filename << p_sim_result->energy_result.control_dynamic << ",";
        result_filename << p_sim_result->energy_result.control_static << ",";
        result_filename << p_sim_result->energy_result.Q_dynamic << ",";
        result_filename << p_sim_result->energy_result.Q_static << ",";
        result_filename << p_sim_result->energy_result.ave_power << ",";
        result_filename << p_sim_result->energy_result.efficiency;

        result_filename.close();
    }
}