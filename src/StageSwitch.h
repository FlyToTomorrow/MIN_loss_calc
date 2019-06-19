#pragma once

#include <string>
#include "BasicElements.h"

class BasicStageSwitch
{
protected:
    /* data */


public:
    string type;
    int radix;
    BasicStageSwitch(std::string type_arg, int radix_arg) : type(type_arg),
                                                            radix(radix_arg) {};

    virtual ~BasicStageSwitch() {};

    virtual double cal_loss(int s, int d, SimParaClass sim_para) { return 0; };
};

class LoopedCrossbar : public BasicStageSwitch
{
private:
    /* data */
public:
    LoopedCrossbar(std::string type_arg, int radix_arg) :
            BasicStageSwitch(type_arg, radix_arg
            ) {};

    ~LoopedCrossbar() {};
    double cal_loss(int s, int d, SimParaClass sim_para);
};

class PILOSS : public BasicStageSwitch
{
private:
    /* data */
public:
    PILOSS(std::string type_arg, int radix_arg) : BasicStageSwitch(type_arg, radix_arg) {};

    ~PILOSS() {};
    double cal_loss(int s, int d, SimParaClass sim_para);
};
