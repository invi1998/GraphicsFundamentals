#pragma once

#include <Windows.h>

namespace CELL
{
	/*
	 * 获取一个代码段执行所花费的时间
	 */
	class CELLTimestamp
	{
	public:
		CELLTimestamp();
		~CELLTimestamp() = default;

		void update();

		/*
		 * 获取当前秒
		 */
		double getElapsedSecond();

		/*
		 * 获取毫秒
		 */
		double getElapsedTimeInMilliSec();

		/*
		 * 获取微秒
		 */
		double getElapsedTimeInMicroSec();

	protected:
		LARGE_INTEGER _frequency;
		LARGE_INTEGER _startCount;
	};
}
