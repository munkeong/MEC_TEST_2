#pragma once

#include "adevs.h"
#include <random>
#include <time.h>

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


class C_Processor : public adevs::Atomic<IO_Type>
{

	enum PROC_TYPE { HIPASS, NON_HIPASS };
	enum Status { INIT, BUSY, WAIT };

public:
	
	C_Processor(int id, int procType = 0);
	~C_Processor();

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



	int pin_bufin;
	int pout_transout, pout_done, pout_init;

private:
	double m_t;

	std::default_random_engine m_generator;

	double m_distParam;

	double m_resDist;
	S_CLIENT *m_pCustomer;
	PROC_TYPE m_procType;

	int m_id;

protected:
	int m_Status;
};

