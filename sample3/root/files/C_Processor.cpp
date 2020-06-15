#include "C_Processor.h"


C_Processor::C_Processor(int id, int procType)
{
	m_Status = INIT;

	m_t = 0;

	pin_bufin = 0;
	pout_transout = 1;
	pout_done = 2;
	pout_init = 3;

	srand(time(NULL));
	m_generator.seed(rand());

	m_procType = (PROC_TYPE)procType;
	m_id = id;

	if (m_procType == HIPASS)
		m_distParam = 0.3;
	else
		m_distParam = 0.07;

}


C_Processor::~C_Processor()
{
}

void C_Processor::delta_int()
{
	m_t += ta();

	if (m_Status == BUSY)
	{
		m_Status = WAIT;
	}

	else if (m_Status == INIT)
	{
		m_Status = WAIT;
	}
}

void C_Processor::delta_ext(double e, const adevs::Bag<IO_Type>& xb)
{
	m_t += e;

	m_Status = BUSY;

	std::exponential_distribution<double> dist(m_distParam);
	m_resDist = dist(m_generator);

	adevs::Bag<IO_Type>::const_iterator i;
	for (i = xb.begin(); i != xb.end(); i++)
	{
		m_pCustomer = (*i).value;

		m_pCustomer->_serviceTime += m_resDist;

		m_pCustomer->_procEnterTime = m_t;

		m_pCustomer->_queueWaitingTime += (m_t - m_pCustomer->_queueEnterTime);

		m_pCustomer->_procType = m_procType;
	}

}

void C_Processor::delta_conf(const adevs::Bag<IO_Type>& xb)
{
	delta_int();
	delta_ext(0.0, xb);
}

void C_Processor::output_func(adevs::Bag<IO_Type>& yb)
{
	if (m_Status == BUSY)
	{
		IO_Type outres;
		outres.port = pout_transout;
		outres.value = m_pCustomer;
		yb.insert(outres);

		IO_Type doneres;
		doneres.port = pout_done;
		yb.insert(doneres);
	}

	else if (m_Status == INIT)
	{
		IO_Type initres;
		initres.port = pout_init;
		initres.value = new S_CLIENT;
		initres.value->_procNum = m_id;
		initres.value->_procType = m_procType;
		initres.value->_isProcInit = 1;

		yb.insert(initres);
	}
}

double C_Processor::ta()
{
	if (m_Status == BUSY)
		return m_resDist;
	else if (m_Status == INIT)
		return 0;
	else
		return DBL_MAX;
}

void C_Processor::gc_output(adevs::Bag<IO_Type>& g)
{
}
