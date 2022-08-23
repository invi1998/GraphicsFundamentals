#include "CELLTimestamp.h"

namespace CELL
{
	CELLTimestamp::CELLTimestamp()
	{
		QueryPerformanceFrequency(&_frequency);
		QueryPerformanceCounter(&_startCount);
	}

	void CELLTimestamp::update()
	{
		QueryPerformanceCounter(&_startCount);
	}

	/*
	 * 获取当前秒
	 *
	 */
	double CELLTimestamp::getElapsedSecond()
	{
		return getElapsedTimeInMicroSec() * 0.000001;
	}

	/*
	 * 获取毫秒
	 */
	double CELLTimestamp::getElapsedTimeInMilliSec()
	{
		return getElapsedTimeInMicroSec() * 0.001;
	}

	/*
	 * 获取微秒
	 */
	double CELLTimestamp::getElapsedTimeInMicroSec()
	{
		LARGE_INTEGER endCount;
		QueryPerformanceCounter(&endCount);

		double startTimeInMicroSec = _startCount.QuadPart * (1000000.0 / _frequency.QuadPart);
		double endTimeInMicroSec = endCount.QuadPart * (1000000.0 / _frequency.QuadPart);

		return endTimeInMicroSec - startTimeInMicroSec;
	}
}