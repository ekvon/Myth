#ifndef __Dsp__Signal__HPP
#define __Dsp__Signal__HPP

#include <cassert>
#include <cstring>

namespace myth
{

namespace dsp
{
	template <typename V,typename _TimeContainer>
	class real_signal
	{
	public:
		using value_type=V;
		using tc_type=_TimeContainer;
		using duration_type=typename tc_type::duration_type;
		using tick_type=typename tc_type::tick_type;
		using size_type=size_t;
		
	public:
		real_signal(value_type * data, size_type size, tc_type tc):size_(size), tc_(tc){
			if(!size_)
				//	ToDo: exception strategy
				assert(0);
			data_=new value_type[size_];
			memcpy(data_, data, size_*sizeof(value_type));
		}
		
		~real_signal(){
			if(data_!=nullptr)
				delete[]data_;
		}
		
		//	return the value of specified sample
		//	Params:
		//	@n - the number of sample
		value_type value(size_type n)const{
			return data_[n%size_];
		}
		
		//	return values of samples in specified range
		//	Params:
		//	@off - offset from the begining of signal (in samples)
		//	@len - the length of requested data (in samples)
		//	@it - output iterator
		template <typename _Iter>
		size_type get_values(size_type off, size_type len, _Iter it){
			if(size_<=off)
				return 0;
			if(size_<off+len)
				//	reduce requested length
				len=size_-off;
			for(size_type i=0;i<len;i++){
				*it++=data_[off+i];
			}
		}

		//	return number of ticks of the duration defined in time interface of the 'time_container'
		//	for the secified sample
		typename duration_type::rep time(size_type n)const{
			return tc_[n];
		}
		
		value_type operator[](size_type n)const{
			return this->value(n);
		}
		
		size_type operator()(tick_type tick)const{
			return tc_(tick);
		}
		
		size_type size()const{
			return size_;
		}
		
		//	WARMING: access to raw data
		value_type * at(size_type n){
			n%=size_;
			return data_+n;
		}
		
	protected:
		size_type size_;
		value_type * data_;
		//	contains time of signal samples
		tc_type tc_;
	};
}

}

#endif//__Dsp__Signal__HPP
