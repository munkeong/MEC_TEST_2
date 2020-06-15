#pragma once

#include "adevs.h"
#include <random>
#include <list>
#include <fstream>
#include <time.h>
#include <algorithm>

#ifndef MSG_STRUCT
#define MSG_STRUCT

typedef enum E_DIRECTION { D_LEFT, D_STRAIGHT, D_RIGHT };
typedef enum E_CAR_TYPE { AUTONOMOUS, MANUAL };

typedef struct {

	int _id;

	E_CAR_TYPE _carType;
	E_CAR_TYPE _payType;

	E_DIRECTION _targetDirection;

	double _genTime, _queueEnterTime, _procEnterTime;

	double _waitingTime;
	double _serviceTime;

	int _queueLength;
	double _queueWaitingTime;

	// for proc
	int _procNum;
	int _procType;
	int _isProcInit;

	// for buf
	int _bufNum;


} S_CLIENT;

typedef adevs::PortValue<S_CLIENT*> IO_Type;
#endif



class C_Generator : public adevs::Atomic<IO_Type>
{


	typedef struct proc_buf_info{
		int _procNum;
		int _procType;
		int _bufSize;
	}PROC_BUF_INFO;
	
	struct cmp {
		bool operator()(PROC_BUF_INFO *a, PROC_BUF_INFO *b) {
			return a->_procNum < b->_procNum;
		}
	}cmp;
	
public:

	C_Generator(double distParam = 1, int AV_ratio = 5, int EP_ratio = 5, int testMode = 1);

	/// Internal transition function.
	void delta_int();
	/// External transition function.
	void delta_ext(double e, const adevs::Bag<IO_Type>& xb);
	/// Confluent transition function.
	void delta_conf(const adevs::Bag<IO_Type>& xb);
	/// Output function.
	void output_func(adevs::Bag<IO_Type>& yb);
	/// Time advance function.
	double ta();
	/// Output value garbage collection.
	void gc_output(adevs::Bag<IO_Type>& g);
	/// Destructor.
	~C_Generator();

	enum Status { STOP, ACTIVE };

	int pin_stop, pin_init, pin_bufin;
	//std::vector<int> p_out;
	int pout_bufout;

private:
	std::list<S_CLIENT*> arrivals;
	double m_distParam;
	std::default_random_engine m_generator;

	int m_carNum;

	double m_t;

	double m_resDist;

	std::vector<PROC_BUF_INFO*> m_procInfo;
//	std::vector<BUF_INFO*> m_bufInfo;

	int calcTargetBuffer(IO_Type &param);
	//int findETCS(int start, int end);
	int findTCS(int type, int start, int end);
	int findTCS2(int type, int _start, int _end);

	int m_isInitCompleted;

	int m_bufNum;

	int m_bufCount;

	int m_testMode;

	int m_AV_Ratio;
	
	int m_EP_Ratio;

protected:
	int m_Status;
};

