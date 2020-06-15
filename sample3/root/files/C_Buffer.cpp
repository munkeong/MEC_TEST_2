#include "C_Buffer.h"



C_Buffer::C_Buffer(int id)
{
	m_ProcessorStatus = FREE;

	pin_genin = 0;
	pin_ready = 1;
	pout_procout = 2;
	pout_genout = 3;

	m_id = id;
	m_t = 0;
}


C_Buffer::~C_Buffer()
{

}

void C_Buffer::delta_int()
{
}

void C_Buffer::delta_ext(double e, const adevs::Bag<IO_Type>& xb)
{
	m_t += e;
	adevs::Bag<IO_Type>::const_iterator i;
	for (i = xb.begin(); i != xb.end(); i++)
	{
		S_CLIENT* c = (*i).value;

		if (c == NULL) {

			m_ProcessorStatus = FREE;
		}
		else if (c->_bufNum != m_id) {
			return;
		}
		else {

			c->_queueEnterTime = m_t;
			m_Buffer.push_back(c);
		}

	}
	
}

void C_Buffer::delta_conf(const adevs::Bag<IO_Type>& xb)
{
	delta_int();
	delta_ext(0.0, xb);
}

void C_Buffer::output_func(adevs::Bag<IO_Type>& yb)
{
	if (IsSendingStatus()) {

		IO_Type res;

		res.port = pout_procout;
		res.value = m_Buffer.front();
		res.value->_queueLength = m_Buffer.size() - 1;

		IO_Type res2;

		res2.port = pout_genout;
		res2.value = m_Buffer.front();
		res2.value->_queueLength = m_Buffer.size() - 1;

		m_Buffer.erase(m_Buffer.begin());

		m_ProcessorStatus = BUSY;

		yb.insert(res);
		yb.insert(res2);

	}
}

double C_Buffer::ta()
{
	if (IsSendingStatus())
		return 0;
	else
		return DBL_MAX;
}

void C_Buffer::gc_output(adevs::Bag<IO_Type>& g)
{
}

bool C_Buffer::FindFreeProcessor()
{
	
	if (m_ProcessorStatus == FREE)
	{
		//id = i;
		return true;
	}
	
	return false;
}

bool C_Buffer::IsSendingStatus()
{
	if (!m_Buffer.empty() && FindFreeProcessor())
		return true;
	else
		return false;
}