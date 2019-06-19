#include "StageSwitch.h"

using namespace std;

double LoopedCrossbar::cal_loss(int s, int d, SimParaClass sim_para)
{
    if (s == d)
    {
        if (s == 0)
            return 2 * (radix - 1) * sim_para.Lp_dB + 2 * radix * sim_para.Lc_dB;
        else
            return 2 * (radix - 1) * (sim_para.Lp_dB + sim_para.Lc_dB);
    }
    else
    {
        return sim_para.Li_dB + (2 * radix - 3 - s - d) * sim_para.Lp_dB + (2 * radix - 2 - s - d) * sim_para.Lc_dB;
    }
}

double PILOSS::cal_loss(int s, int d, SimParaClass sim_para)
{
    return sim_para.Li_dB + (radix - 1) * sim_para.Lp_dB + 2 * (radix - 1) * sim_para.Lc_dB;
}