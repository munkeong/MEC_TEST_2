#include "C_Transducer.h"


C_Transducer::C_Transducer(double maxCust)
{
	m_t = 0;

	m_Status = WAIT;
	m_numCustomers = 0;

	m_waitTime = 0;
	m_procBusyTime = 0;
	m_totalServiceTime = 0;

	m_queueLength = 0;

	m_etcsQueueLength = m_tcsQueueLength = 0;
	m_etcsBusyTime = m_tcsBusyTime = 0;
	m_etcsWaitTime = m_tcsWaitTime = 0;

	m_numETCSCustomers = m_numTCSCustomers = 0;

	pin_procin = 0;
	pout_stop = 1;
	m_maxCust = maxCust;
}

C_Transducer::C_Transducer(double maxCust, std::string fileName)
{
	m_t = 0;

	m_Status = WAIT;
	m_numCustomers = 0;

	m_waitTime = 0;
	m_procBusyTime = 0;
	m_totalServiceTime = 0;

	m_queueLength = 0;

	m_etcsQueueLength = m_tcsQueueLength = 0;
	m_etcsBusyTime = m_tcsBusyTime = 0;
	m_etcsWaitTime = m_tcsWaitTime = 0;

	m_numETCSCustomers = m_numTCSCustomers = 0;

	pin_procin = 0;
	pout_stop = 1;
	m_maxCust = maxCust;

	m_fileName = fileName;
}


C_Transducer::~C_Transducer()
{
}

void C_Transducer::delta_int()
{
	if (m_Status == SEND)
	{
		m_Status = STOP;

		std::ofstream fp;

		if (!m_fileName.empty()) {
			fp.open(m_fileName, std::ofstream::out | std::ofstream::app);
			fp << m_totalServiceTime / m_numCustomers << "\t"
				<< m_procBusyTime / m_numCustomers << "\t"
				<< m_etcsBusyTime / m_numETCSCustomers << "\t"
				<< m_tcsBusyTime / m_numTCSCustomers << "\t"
				<< m_waitTime / m_numCustomers << "\t"
				<< m_etcsWaitTime / m_numETCSCustomers << "\t"
				<< m_tcsWaitTime / m_numTCSCustomers << "\t"
				<< m_queueLength / (double)m_numCustomers << "\t"
				<< m_etcsQueueLength / (double)m_numETCSCustomers << "\t"
				<< m_tcsQueueLength / (double)m_numTCSCustomers << "\n";
		}

		std::cout << m_totalServiceTime / m_numCustomers << "\t"
			<< m_procBusyTime / m_numCustomers << "\t"
			<< m_etcsBusyTime / m_numETCSCustomers << "\t"
			<< m_tcsBusyTime / m_numTCSCustomers << "\t"
			<< m_waitTime / m_numCustomers << "\t"
			<< m_etcsWaitTime / m_numETCSCustomers << "\t"
			<< m_tcsWaitTime / m_numTCSCustomers << "\t"
			<< m_queueLength / (double)m_numCustomers << "\t"
			<< m_etcsQueueLength / (double)m_numETCSCustomers << "\t"
			<< m_tcsQueueLength / (double)m_numTCSCustomers << "\n";

	}
}

void C_Transducer::delta_ext(double e, const adevs::Bag<IO_Type>& xb)
{
	if (m_Status == WAIT) {
		m_t += e;
		adevs::Bag<IO_Type>::const_iterator i;
		for (i = xb.begin(); i != xb.end(); i++)
		{
			m_numCustomers++;
			const S_CLIENT* c = (*i).value;

			m_waitTime += c->_queueWaitingTime;
			m_procBusyTime += c->_serviceTime;	// Busy time : service Time
			m_totalServiceTime += (c->_queueWaitingTime + c->_serviceTime);
			m_queueLength += c->_queueLength;

			if (c->_procType == 0) {
				m_etcsQueueLength += c->_queueLength;
				m_etcsBusyTime += c->_serviceTime;
				m_etcsWaitTime += c->_queueWaitingTime;
				m_numETCSCustomers++;
			}
			else {
				m_tcsQueueLength += c->_queueLength;
				m_tcsBusyTime += c->_serviceTime;
				m_tcsWaitTime += c->_queueWaitingTime;
				m_numTCSCustomers++;
			}

			delete c;

		}


		if (m_numCustomers >= m_maxCust)
		{
			m_Status = SEND;
		}

	}
}

void C_Transducer::delta_conf(const adevs::Bag<IO_Type>& xb)
{
}

void C_Transducer::output_func(adevs::Bag<IO_Type>& yb)
{
	if (m_Status == SEND) {
		IO_Type res;
		res.port = pout_stop;
		yb.insert(res);
	}
}

double C_Transducer::ta()
{
	if (m_Status == SEND)
		return 0;
	else
		return DBL_MAX;
}

void C_Transducer::gc_output(adevs::Bag<IO_Type>& g)
{
}
