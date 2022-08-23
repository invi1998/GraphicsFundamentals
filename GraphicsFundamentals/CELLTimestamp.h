#pragma once

#include <Windows.h>

namespace CELL
{
	/*
	 * ��ȡһ�������ִ�������ѵ�ʱ��
	 */
	class CELLTimestamp
	{
	public:
		CELLTimestamp();
		~CELLTimestamp() = default;

		void update();

		/*
		 * ��ȡ��ǰ��
		 */
		double getElapsedSecond();

		/*
		 * ��ȡ����
		 */
		double getElapsedTimeInMilliSec();

		/*
		 * ��ȡ΢��
		 */
		double getElapsedTimeInMicroSec();

	protected:
		LARGE_INTEGER _frequency;
		LARGE_INTEGER _startCount;
	};
}
