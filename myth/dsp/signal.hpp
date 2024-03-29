#ifndef __Dsp__Signal__HPP
#define __Dsp__Signal__HPP

#include <vector>

namespace dsp
{
	template <
		typename V,
		typename _TimeContainer,
		typename _FreqType=float>
	class signal
	{
	public:
		using value_type=V;
		using time_container=_TimeContainer;
		using frequency_type=_FreqType;
		using size_type=size_t;
		
	public:
		signal(std::vector<value_type>&& rhs,frequency_type sr,value_type default_val=0):
			value_(std::move(rhs)),default_(default_val)
		{
			if(!sr){
				//	sample rate is not specified
				return;
			}
			time_type delta=1.0/sr;
			size_type N=value_.size();
			for(size_type n=0;n<N;n++){
				time_.push_back(n*delta);
			}
		}
		//	accessors
		value_type value(size_type n)noexcept{
			if(value_.size()<=n)
				return default_;
			return value_[n];
		}
		//	temporary decision
		void get_values(size_type off, size_type len, std::vector<value_type> &values){
			values.clear();
			if(value_.size()<off+len)
				return;
			for(size_type i=0;i<len;i++){
				values.push_back(value_[off+i]);
			}
		}
		
	protected:
		std::vector<value_type> value_;
		time_container tc_;
		
		value_type default_;
	};
}

#endif//__Dsp__Signal__HPP
