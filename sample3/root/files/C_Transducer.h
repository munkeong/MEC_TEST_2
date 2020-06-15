#pragma once

#include "adevs.h"
#include <fstream>

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

class C_Transducer : public adevs::Atomic<IO_Type>
{
public:

	C_Transducer(double maxCust);
	C_Transducer(double maxCust, std::string fileName);

	~C_Transducer();
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

	enum Status { WAIT, STOP, SEND };

	int pin_procin;
	int pout_stop;

private:
	double m_t;

	int m_numCustomers;

	double m_averageWaitingTime;
	double m_queueLength;
	double m_totalServiceTime;
	double m_waitTime;
	double m_procBusyTime;

	double m_etcsQueueLength, m_tcsQueueLength;
	double m_etcsBusyTime, m_tcsBusyTime;
	double m_etcsWaitTime, m_tcsWaitTime;
	int m_numETCSCustomers, m_numTCSCustomers;
	

	std::string m_fileName;

	std::ofstream output_strm;

	double m_maxCust;

protected:
	int m_Status;
};

