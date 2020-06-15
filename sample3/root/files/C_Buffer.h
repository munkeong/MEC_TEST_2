#pragma once

#include "adevs.h"

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

class C_Buffer : public adevs::Atomic<IO_Type>
{
public:
	C_Buffer(int id);
	~C_Buffer();

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

	bool FindFreeProcessor();
	bool IsSendingStatus();
	//void UpdateStatus(std::string & str);

	int m_ProcessorStatus;

	std::vector<S_CLIENT*> m_Buffer;

	enum Status { INIT, NORMAL, STOP };
	enum ProcessorStatus { FREE, BUSY };

	int pin_genin, pin_ready;
	int pout_procout, pout_genout;

private:
	double m_t;

	int m_id;

protected:
	int m_Status;
};

