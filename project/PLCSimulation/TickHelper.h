#pragma once

#include <windows.h>

class CTickHelper{

protected:
	LARGE_INTEGER m_nFreq;
	LARGE_INTEGER m_nBeginTime;
	LARGE_INTEGER m_nEndTime;
	double m_time;

public:
	CTickHelper(){}
	~CTickHelper(){}

	void Start(){
		QueryPerformanceFrequency(&m_nFreq);
		QueryPerformanceCounter(&m_nBeginTime); 
	}
	void End(){
		QueryPerformanceCounter(&m_nEndTime);
		m_time=(double)(m_nEndTime.QuadPart-m_nBeginTime.QuadPart)/(double)m_nFreq.QuadPart;
	}
	double GetSpeed(){
		return m_time;
	}
};