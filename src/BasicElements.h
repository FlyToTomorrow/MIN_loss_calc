#pragma once
#include <string>
#include <queue>
#include <random>
#include <iostream>
#include <fstream>
#include <iomanip> 
#include <algorithm>    // std::find
#include <map>
#include <limits>

typedef unsigned long long Time;
//typedef long long TimeDiff;
const Time MAX_TIME = std::numeric_limits<Time>::max();
//const Time MAX_LONG = std::numeric_limits<long long>::max();


using namespace std;

// ************************	Declaration ************************	
class SimParaClass;
class EnergyParaClass;
class RequestElement;
class PerformanceResultElement;
class EnergyResultElement;
class HardwareResultElement;
class ResultClass;

// -----------------------	for parameter reading -----------------------	
bool ReadParameterFile(const string FileName, map<string, string> &parameter_list);
inline string PopValue(const char *variable_name, map<string, string> &parameter_list,
	bool is_allow_Null = false);
// -----------------------	Some assistant Functions -----------------------	
inline double ratio2dB(double x);
inline double dB2ratio(double x);
inline int ipow(int base, int exp);
inline bool to_bool(std::string const& s);
void WriteSweepResultToFile(
	const SimParaClass sim_para, const ResultClass* p_sim_result);
void WriteRealTrafficResultToFile(
	const SimParaClass sim_para, const ResultClass* p_sim_result);



// =======================	EnergyParaClass =======================	
class EnergyParaClass
{
public:
	EnergyParaClass() :
		MR_num_per_group(1),
		sensitivity(0),
		slope_efficiency(0),
		laser_voltage(0),
		laser_current_threshold(0),
		laser_tuning_power(0),
		switch_on_power(0),
		serdes_dynamic_power(0),
		serdes_static_power(0),
		modulator_static_power(0),
		modulator_dynamic_power(0),
		receiver_static_power(0),
		receiver_dynamic_power(0),
		tuning_power(0),
		control_dynamic_power(0),
		control_static_power(0),
		Q_dynamic_power(0),
		Q_static_power(0),
		laser_plugin_efficiency(0.1),
		Control_delay_cycle(4),
		transmission_time_o(0),
		transmission_time_e(0) {};
	~EnergyParaClass() {};

	int MR_num_per_group;
	double sensitivity;
	double slope_efficiency, laser_voltage, laser_current_threshold, laser_tuning_power;
	double switch_on_power;
	double serdes_dynamic_power;
	double serdes_static_power;
	double modulator_static_power, modulator_dynamic_power;
	double receiver_static_power, receiver_dynamic_power;
	double tuning_power;
	double control_dynamic_power;
	double control_static_power;
	double Q_dynamic_power;
	double Q_static_power;

	double laser_plugin_efficiency;
	Time Control_delay_cycle;

	double transmission_time_o, transmission_time_e;
};



// =======================	SimParaClass =======================	
class SimParaClass
{
public:
	inline SimParaClass();
	~SimParaClass() {};
	void GetParameterValues(const string FileName);
	void GetParameterValues_FromInput(int argc, char* argv[]);
	void CheckParameterValues();
	void ConfirmParameterValues_FromInput();
	void PreProcessing();	// calculate the set up time, BR and offered load


	// -----------------------	parameters -----------------------	
	//switch basic
	int SwitchPort;
	int StageSwitch_Radix;		// only for the MIN
	int sim_type;	// 1. Xtalk, 2. Performance, 3. Performance sweep
	int sweeping_type;	// 1. uniform sweeping, 2. sweep 2 times, form 0 to middle,
						// 3. sweep 2 times, from middle1 to middle 2
	int laser_power_calculation_type;	// 1. voltage*current; 2. optical power/efficiency
	bool is_serdes_idle_power_flag;		// true: consider; false: not consider
	bool is_APC_flag;					// whether adaptive power control (APC) is used
	bool is_static_tuning;					// true: static tuning, false: optimistic tuning

	string OutputFileName_fromInput;
	string OutputFileName0;
	string Path;
	string ParameterFileName;
	string switch_type;
	string traffic_pattern_type;
	double hot_prob;
	Time rearrange_time_in_cycle;
	string extra_setting_file;
	string trace_filename;
	size_t trace_length;

	string stage_switch_type;

	// pkt len type
	string pkt_len_type;
	int AveLen;
	int pkt_max_size;

	//loss related
	double Li_dB;
	double Lp_dB;
	double Lc_dB;
	double Lgc_dB;
	double Lwg_dB;
	double Lm_dB;
	double Xd_dB;
	double Xt_dB;
	double Xc_dB;
	double Ps_dB;
	double Li;
	double Lp;
	double Lc;
	double Lgc;
	double Lwg;
	double Lm;
	double Xd;
	double Xt;
	double Xc;
	double Ps;

	int Loop;

	//performance related
	double speed_of_light = 299792458;
	double n_SiO2 = 1.444;
	double BR0;
	int WDM_num;
	double BR;
	double ClkFrequency;
	Time SetupTime;
	unsigned int MaxBufferLen;
	unsigned int VIQ_depth;
	unsigned int VOQ_num;
	unsigned int VIQ_num;
	int FlitSize;
	double fiber_len;
	double EOE_delay;
	double propagation_delay;
	double Switch_on_off_delay;
	Time Control_delay_cycle;
	double Laser_tuning_delay;
	double Synchronization_delay;

	Time sim_time;
	Time warmup_time;
	int job_num;
	double injection_rate;
	double injection_rate_start;
	double injection_rate_end;
	int injection_rate_num;

	EnergyParaClass energy_para;

};

inline SimParaClass::SimParaClass() :
	//switch basic
	SwitchPort(16),
	StageSwitch_Radix(2),
	sim_type(1),
	sweeping_type(1),
	laser_power_calculation_type(1),
	is_serdes_idle_power_flag(false),
	is_APC_flag(false),
    is_static_tuning(false),
	OutputFileName_fromInput(""),
	switch_type("Baseline"),
	traffic_pattern_type("Uniform"),
	hot_prob(1),
	rearrange_time_in_cycle(0),
	extra_setting_file(""),
	trace_filename(""),
	trace_length(1000),
	// pkt size related
	pkt_len_type("Fixed"),
	AveLen(1024 * 8),			// bits
	pkt_max_size(4096 * 8),
	//loss related
	Li_dB(-3.62),
	Lp_dB(-0.08),
	Lc_dB(-0.05),
	Lgc_dB(-1.5),
	Lwg_dB(-1),
	Lm_dB(-1),
	Xd_dB(-46),
	Xt_dB(-12),
	Xc_dB(-37),
	Ps_dB(-24),
	Li(dB2ratio(Li_dB)),
	Lp(dB2ratio(Lp_dB)),
	Lc(dB2ratio(Lc_dB)),
	Lgc(dB2ratio(Lgc_dB)),
	Lwg(dB2ratio(Lwg_dB)),
	Lm(dB2ratio(Lm_dB)),
	Xd(dB2ratio(Xd_dB)),
	Xt(dB2ratio(Xt_dB)),
	Xc(dB2ratio(Xc_dB)),
	Ps(dB2ratio(Ps_dB)),
	Loop(100),
	//performance related
	BR0(10e9),			// bps
	WDM_num(1),
	BR(BR0*WDM_num),		// bps
	ClkFrequency(2.5e9),	// Hz
	MaxBufferLen(1024),		// cells
	VIQ_depth(4),	// cells
	VOQ_num(1),
	VIQ_num(1),
	FlitSize(128),				// bits
	fiber_len(0),	// m
	EOE_delay(0),
	propagation_delay(fiber_len / (speed_of_light / n_SiO2) + EOE_delay), // ns
	Switch_on_off_delay(5e-9),		// s
	Control_delay_cycle(4),		// cycles
	Laser_tuning_delay(100e-9),	// s
	Synchronization_delay(0),
	sim_time((Time)5e4),
	warmup_time(sim_time / 5),
	job_num(1),
	injection_rate(20e9),
	injection_rate_start(0.01e9),
	injection_rate_end(10e9),
	injection_rate_num(10)
{

	//switch basic --input and output
	Path = "./Data/";
	ParameterFileName = "parameter_setting_" + switch_type;
	if (traffic_pattern_type == "HotSpot" || traffic_pattern_type == "HotPair")
		OutputFileName0 = to_string(SwitchPort) + 'x' + to_string(SwitchPort)
		+ '_' + switch_type
		+ '_' + to_string(WDM_num)
		+ '_' + to_string(AveLen / 8)
		+ '_' + to_string(SetupTime)
		+ '_' + to_string((int)(hot_prob * 100) / 100.0)
		+ traffic_pattern_type
		+ '_' + pkt_len_type;
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

	// /*---- determine whether this is real traffic trace ----*/	
	// if (!trace_filename.empty())
	// {
	// 	OutputFileName0 = to_string(SwitchPort) + 'x' + to_string(SwitchPort)
	// 		+ '_' + switch_type
	// 		+ '_' + to_string(WDM_num)
	// 		+ '_' + to_string(SetupTime)
	// 		+ '_' + trace_filename;
	// }
}


// ************************	Performance simulation related ************************	

// =======================	Request Element =======================	
class RequestElement
{
public:
	RequestElement() {};
	//RequestElement(int s, int d,   int len, size_t generate_time_para, 
	//	int service_time_by_cycle) :
	//	source(s),
	//	destination(d),
	//	pkt_len(len),
	//	generate_time(generate_time_para),
	//	transmission_time(service_time_by_cycle) {};
	RequestElement(int s, int d, size_t len, Time generate_time_para,
		double BR, double clk_frequency) :
		source(s),
		destination(d),
		pkt_len(len),
		generate_time(generate_time_para),
		transmission_time((Time)(ceil(pkt_len / BR*clk_frequency))),
		HOL_time(MAX_TIME),
		last_rearrange_time(-1) {};
	void Record_HOL_Time(Time current_time) { HOL_time = current_time; };
	void Record_LR_Time(Time current_time) { last_rearrange_time = current_time; };
	//void Set_IsTxBusy() { is_Tx_busy = true; };
	~RequestElement() {};

	int source;
	int destination;
	size_t pkt_len;	// Unit: bit
	Time generate_time;
	Time transmission_time;
	Time HOL_time; // the time of the request arrives the head of the buffer
	Time last_rearrange_time;	// time of last rearrangement for rearrange switches (-1U by default)
};


// =======================	Performance Result Element =======================	
class PerformanceResultElement
{
public:
	PerformanceResultElement() {};
	PerformanceResultElement(double injection_rate, double BR) :
		injection_rate(injection_rate),
		offered_load(injection_rate / BR),
		ave_latency(0),
		ave_throughput(0),
		ave_Qsize(0),
		total_latency(0),
		max_latency(0),
		total_pkt(0),
		total_data(0),
		execution_time(0), 
		rearrange_num(0){};
	inline ~PerformanceResultElement();

	inline void initialization(double injection_rate, double BR);
	void Update(Time current_time, Time generate_time, size_t length);

	//void WriteToFile(string filename);

	double injection_rate, offered_load, ave_latency, ave_throughput, ave_Qsize;
	Time total_latency, max_latency;
	unsigned long long total_pkt, total_data;
	Time execution_time;

	Time rearrange_num;
};

inline void PerformanceResultElement::initialization(double injection_rate_para, double BR)
{
	injection_rate = injection_rate_para;
	offered_load = injection_rate / BR;
	total_latency = 0;
	ave_latency = 0;
	ave_throughput = 0;
	max_latency = 0;
	total_pkt = 0;
	total_data = 0;
	ave_Qsize = 0;
	execution_time = 0;
	rearrange_num = 0;
}

inline PerformanceResultElement::~PerformanceResultElement()
{
}

// =======================	Energy Result Element =======================
class EnergyResultElement
{
public:
	inline EnergyResultElement();
	inline ~EnergyResultElement();
	inline void initialization();
	inline void Cal_Efficiency(unsigned long long total_data, double real_sim_time,
		bool is_serdes_idle_power_flag = true, int laser_power_calculation_type = 1);

	long double total, laser_dynamic, laser_static, laser_tuning, serdes_dynamic, serdes_static, modulator_static, modulator_dynamic,
		receiver_static, receiver_dynamic, switch_static, tuning, control_dynamic,
		control_static, Q_dynamic, Q_static;
	double laser_plugin;
	double ave_loss, max_loss;
	double ave_power, efficiency;


};

inline EnergyResultElement::EnergyResultElement() :
	total(0),
	laser_dynamic(0),
	laser_static(0),
	laser_tuning(0),
	serdes_dynamic(0),
	serdes_static(0),
	modulator_static(0),
	modulator_dynamic(0),
	receiver_static(0),
	receiver_dynamic(0),
	switch_static(0),
	tuning(0),
	control_dynamic(0),
	control_static(0),
	Q_dynamic(0),
	Q_static(0),
	laser_plugin(0),
	ave_loss(0),
	max_loss(0),
	ave_power(0),
	efficiency(0)
{}

inline void EnergyResultElement::initialization()
{
	total = 0;
	laser_dynamic = 0;
	laser_static = 0;
	laser_tuning = 0;
	serdes_dynamic = 0;
	serdes_static = 0;
	modulator_dynamic = 0;
	receiver_dynamic = 0;
	modulator_static = 0;
	receiver_static = 0;
	switch_static = 0;
	tuning = 0;
	control_dynamic = 0;
	control_static = 0;
	Q_dynamic = 0;
	Q_static = 0;

	laser_plugin = 0;
	ave_loss = 0;
	max_loss = 0;

	ave_power = 0;
	efficiency = 0;
}

inline void EnergyResultElement::Cal_Efficiency(unsigned long long total_data,
	double real_sim_time, bool is_serdes_idle_power_flag,
	int laser_power_calculation_type)
{
	long double serdes_total =
		is_serdes_idle_power_flag ? serdes_dynamic + serdes_static : serdes_dynamic;
	switch (laser_power_calculation_type)
	{
	case 1: total = laser_dynamic + laser_static + laser_tuning + serdes_total + modulator_static
		+ modulator_dynamic + receiver_static + receiver_dynamic
		+ switch_static + tuning + control_dynamic + control_static
		+ Q_dynamic + Q_static; // mJ
		break;
	case 2: total = laser_plugin + laser_tuning + serdes_total + modulator_static + modulator_dynamic
		+ receiver_static + receiver_dynamic + switch_static
		+ tuning + control_dynamic + control_static
		+ Q_dynamic + Q_static; // mJ
		break;
	default:
		cout << "Error: Please input valid laser_power_calculation_type!!!" << endl;
		exit(43);
		break;
	}

	ave_power = total / real_sim_time / 1e3;	// W
	efficiency = total / total_data * 1e9;	// pJ/bit
}

inline EnergyResultElement::~EnergyResultElement()
{
}

class HardwareResultElement
{
public:
	inline HardwareResultElement();
	~HardwareResultElement() {};

	int laser, serdes, modulator, receiver, switch_element, crossing, MUX, OPin;

private:

};

inline HardwareResultElement::HardwareResultElement() :
	laser(0),
	serdes(0),
	modulator(0),
	receiver(0),
	switch_element(0),
	crossing(0),
	MUX(0),
	OPin(0)
{}


class ResultClass
{
public:
	inline ResultClass();
	inline ~ResultClass();

	PerformanceResultElement performance_result;
	EnergyResultElement energy_result;
	HardwareResultElement hardware_result;

private:

};

inline ResultClass::ResultClass()
{
}

inline ResultClass::~ResultClass()
{
}

// ************************	Fundamental ************************
inline double ratio2dB(double x)
{
	return 10 * log10(x);
}

inline double dB2ratio(double x)
{
	return pow(10, x / 10);
}

// ======================= inline functions =======================	
inline string PopValue(const char *variable_name, map<string, string> &parameter_list,
	bool is_allow_Null)
{
	map<string, string>::iterator it;
	string parameter_result;
	it = parameter_list.find(variable_name);
	if (it != parameter_list.end())
	{
		parameter_result = it->second;
		parameter_list.erase(it);
		return parameter_result;
	}
	else
	{
		cout << "Warning: No \"" << variable_name <<
			"\" variable in the configure files!!!" << endl;
		/*exit(-5);*/
		if (is_allow_Null)
		{
			return "";
		}
		else
		{
			return "0";
		}
	}
}

inline bool to_bool(std::string const& s) {
	if (s == "0" || s == "false" || s == "False")
		return false;
	else
		return true;
}


inline int ipow(int base, int exp)
{
	int result = 1;
	while (exp)
	{
		if (exp & 1)
			result *= base;
		exp >>= 1;
		base *= base;
	}

	return result;
}