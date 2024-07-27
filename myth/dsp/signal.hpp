#ifndef __Dsp__Signal__HPP
#define __Dsp__Signal__HPP

#include <cassert>
#include <cstring>

#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <type_traits>

namespace myth
{

namespace dsp
{
	template <typename V,typename _TimeContainer>
	class signal_re
	{
	public:
		using value_type=V;
		using tc_type=_TimeContainer;
		using duration_type=typename tc_type::duration_type;
		using tick_type=typename tc_type::tick_type;
		using size_type=size_t;
		
	public:
		signal_re(value_type * data, size_type size, tc_type tc):size_(size), tc_(tc){
			if(!size_)
				//	ToDo: exception strategy
				assert(0);
			data_=new value_type[size_];
			memcpy(data_, data, size_*sizeof(value_type));
		}
		
		//	read raw data from the file
		signal_re(std::string path) : data_(nullptr){
			std::filebuf fbuf;
			
			fbuf.open(path, std::ios::in | std::ios::binary);
			if(!fbuf.is_open())
				throw std::invalid_argument("signal_re: invalid path to the file\n");
			auto size = fbuf.pubseekoff(0, std::ios::end);
			if(size % sizeof(value_type))
				throw std::logic_error("signal_re: the file size must multiply of value type\n");
			fbuf.pubseekoff(0, std::ios::beg);
			
			data_ = new value_type[size / sizeof(value_type)];
			std::istream is(&fbuf);
			is.read((char*)data_, size);
			if(is.tellg() != size)
				throw std::length_error("signal_re: unable to read data\n");
			tc_.set_size(size / sizeof(value_type));
			fbuf.close();
		}
		
		~signal_re(){
			if(data_!=nullptr)
				delete[]data_;
		}
		
		//	return the value of specified sample
		//	Params:
		//	@n - the number of sample
		value_type value(size_type n)const{
			return data_[n%size_];
		}
		
		//	Return values of samples in specified range
		//	Params:
		//	@off - offset from the begining of signal (in samples)
		//	@len - the length of requested data (in samples)
		//	@it - output iterator
		//	Value type of iterator must be the same as the type od data
		template <typename _Iter>
		size_type get_data(size_type off, size_type len, _Iter it){
			using it_value_type=typename std::iterator_traits<_Iter>::value_type;
			static_assert(std::is_same<value_type, it_value_type>::value);
			
			if(size_<=off)
				return 0;
			if(size_<off+len)
				//	reduce requested length
				len=size_-off;
			for(size_type i=0;i<len;i++){
				*it++=data_[off+i];
			}
			return len;
		}
		
		//	The same as the previuos function but the data type must be convertible to iterator value type.
		template <typename _Iter>
		size_type get_data_as(size_type off, size_type len, _Iter it){
			using it_value_type=typename std::iterator_traits<_Iter>::value_type;
			static_assert(std::is_convertible<value_type, it_value_type>::value);
			
			if(size_<=off)
				return 0;
			if(size_<off+len)
				//	reduce requested length
				len=size_-off;
			for(size_type i=0;i<len;i++){
				*it++=static_cast<it_value_type>(data_[off+i]);
			}
			return len;
		}

		//	return number of ticks of the duration defined in time interface of the 'time_container'
		//	for the secified sample
		typename duration_type::rep time(size_type n)const{
			return tc_[n];
		}
		
		value_type operator[](size_type n)const{
			return this->value(n);
		}

		//	return sr_type::rep value that 		
		tick_type operator()(typename duration_type::rep time_tick)const{
			return tc_(time_tick);
		}
		
		size_type size()const{
			return size_;
		}
		
		//	WARNING: access to raw data
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
