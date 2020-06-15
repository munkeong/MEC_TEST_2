#include "C_Generator.h"


C_Generator::C_Generator(double distParam, int AV_ratio, int EP_ratio, int testMode)
{
	m_testMode = testMode;
	m_bufNum = 0;

	m_t = 0;
	m_carNum = 0;
	m_distParam = distParam;

	srand(time(NULL));
	m_generator.seed(rand());

	m_isInitCompleted = 0;

	//std::poisson_distribution<int> dist(m_distParam);
	std::exponential_distribution<double> dist(m_distParam);
	m_resDist = dist(m_generator) + 1;

	m_Status = ACTIVE;

	pin_stop = 100;
	pin_init = 101;
	pin_bufin = 102;

	pout_bufout = 0;

	m_bufCount = 0;

	m_AV_Ratio = AV_ratio;

	m_EP_Ratio = EP_ratio;

}

C_Generator::~C_Generator()
{
	for (int i = 0; i < m_procInfo.size(); i++)
		delete m_procInfo[i];
}


void C_Generator::delta_int()
{
	m_t += ta();
}

void C_Generator::delta_ext(double e, const adevs::Bag<IO_Type>& xb)
{
	m_t += e;

	adevs::Bag<IO_Type>::const_iterator i;
	for (i = xb.begin(); i != xb.end(); i++)
	{
		S_CLIENT* c = (*i).value;

		if (c == NULL) {
			m_Status = STOP;
		}

		else if (c->_isProcInit == 1) {
			PROC_BUF_INFO *tmp = new PROC_BUF_INFO;
			tmp->_procNum = c->_procNum;
			tmp->_procType = c->_procType;
			tmp->_bufSize = c->_queueLength;

			m_procInfo.push_back(tmp);

			c->_isProcInit = 0;
		}

		else {
			m_procInfo[c->_bufNum]->_bufSize = c->_queueLength;
		}
	}
}

void C_Generator::delta_conf(const adevs::Bag<IO_Type>& xb)
{
	delta_int();
	delta_ext(0.0, xb);
}

void C_Generator::output_func(adevs::Bag<IO_Type>& yb)
{

	if (!m_isInitCompleted)
	{
		std::sort(m_procInfo.begin(), m_procInfo.end(), cmp);
		m_isInitCompleted = 1;
		m_bufNum = m_procInfo.size();
	}

	if (m_Status == ACTIVE)
	{

		if (m_isInitCompleted) 
		{
			std::exponential_distribution<double> dist(m_distParam);
			m_resDist = dist(m_generator);
		}


		int targetBufNum = (m_bufCount++)%m_bufNum;
		
		IO_Type output;
		output.value = new S_CLIENT;
		output.value->_id = m_carNum++;

		
		if (rand() % 10 < m_AV_Ratio)
		{
			output.value->_carType = AUTONOMOUS;
		}
		else
		{
			output.value->_carType = MANUAL;
		}
		
		if (output.value->_carType == AUTONOMOUS)
		{
			output.value->_payType = AUTONOMOUS;
		}	
		else 
		{
			if (rand() % 10 < m_EP_Ratio)
			{
				output.value->_payType = AUTONOMOUS;
			}
				
			else
			{
				output.value->_payType = MANUAL;
			}
				
		}
		output.value->_targetDirection = (E_DIRECTION)(rand() % 3);

		
		targetBufNum = calcTargetBuffer(output);
		output.value->_bufNum = targetBufNum;

		output.port = pout_bufout;

		output.value->_genTime = m_t + ta();
		output.value->_serviceTime = 0;
		output.value->_waitingTime = 0;
		output.value->_queueWaitingTime = 0;
		output.value->_isProcInit = 0;

		yb.insert(output);
	}
}

int C_Generator::calcTargetBuffer(IO_Type &param)
{
	int start, end;
	int res = 0;

	if (param.value->_targetDirection == D_LEFT) 
	{

		start = 0;
		end = m_bufNum / 3 - 1;

		if (end < 0)
			end = 0;

	}
	else if (param.value->_targetDirection == D_STRAIGHT) 
	{
		start = m_bufNum / 3;
		end = start + m_bufNum / 3 -1 + m_bufNum % 3;
	}
	else 
	{
		end = m_bufNum - 1;
		start = end + 1 - m_bufNum / 3;
		if (m_bufNum / 3 == 0)
			start -= 1;
	}

	res = findTCS(param.value->_payType, start, end);

	if (m_testMode && param.value->_carType == MANUAL) 
	{

		int perc = rand() % 10;

		if (perc < 2) 
		{

			int originRes = res;
			
			res = findTCS2(param.value->_payType, start, end);
			
		}
	
	}

	return res;
}

int C_Generator::findTCS(int type, int _start, int _end)
{
	int res = -1;
	int min = 20000000;
	int start = _start, end = _end;

	do {

		for (int i = start; i <= end; i++) 
		{

			if (m_procInfo[i]->_procType == type) 
			{

				if (m_procInfo[i]->_bufSize < min) 
				{
					res = i;
					min = m_procInfo[i]->_bufSize;
				}
			}
		}

		start -= 2;
		if (start < 0)
			start = 0;
		end += 2;
		if (end > m_bufNum -1)
			end = m_bufNum -1;

	} while (res == -1);

	return res;
}

int C_Generator::findTCS2(int type, int _start, int _end)
{
	int res = -1;
	int min = 20000000;
	int start = _start, end = _end;

	do {

		for (int i = start; i <= end; i++) 
		{

			if (type == AUTONOMOUS) {

				if (m_procInfo[i]->_bufSize < min) 
				{
					res = i;
					min = m_procInfo[i]->_bufSize;
				}
			}
			else {

				if (m_procInfo[i]->_procType == type) 
				{

					if (m_procInfo[i]->_bufSize < min) 
					{
						res = i;
						min = m_procInfo[i]->_bufSize;
					}
				}
			}
			
		}

		start -= 2;
		if (start < 0)
			start = 0;
		end += 2;
		if (end > m_bufNum - 1)
			end = m_bufNum - 1;

	} while (res == -1);

	return res;
}

double C_Generator::ta()
{
	if (m_Status == ACTIVE) 
	{
		
		return m_resDist;
	}

	else 
	{
		return DBL_MAX;
	}
}

void C_Generator::gc_output(adevs::Bag<IO_Type>& g)
{
}
