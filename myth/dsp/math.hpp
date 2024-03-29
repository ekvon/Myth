#ifndef __Utils__Math__Hpp
#define __Utils__Math__Hpp

#include <functional>

#include <math.h>

namespace utils
{
	template <typename T> struct math_func_def;
	
	template <>
	struct math_func_def<float>
	{};
	
	template <>
	struct math_func_def<double>
	{};
	
	template <>
	struct math_func_def<long double>
	{};
}

#endif
