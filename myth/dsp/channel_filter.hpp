#ifndef __Myth__Dsp__Channel_Filter__Hpp
#define __Myth__Dsp__Channel_Filter__Hpp

#include <chrono>
#include <iostream>
#include <iterator>
#include <stdexcept>

#include <cassert>
#include <cmath>

namespace myth
{

namespace dsp
{
	struct sequential_ordering_tag
	{};
	struct interleaved_ordering_tag
	{};
	
	//	Data of type _InputType (real or complex) are encoded in raw bytes with specified ordering (sequential or interleaved)
	//	and number of channels.
	template <typename _InputType, typename _OrderingTag, uint8_t _NumChannels,bool _IsComplex> struct channel_filter_base;
	
	//	channel_filter for complex data
	template <typename _InputType, typename _OrderingTag, uint8_t _NumChannels>
	struct channel_filter_base<_InputType,_OrderingTag,_NumChannels,true>
	{
		using char_type=char;
		using in_type=_InputType;
		using order_type=_OrderingTag;
		
		template <typename _InputIter, typename _OutputIter>
		static void select(
			_OutputIter dst_begin,
			_OutputIter dst_end,
			_InputIter src_begin,
			_InputIter src_end,
			uint8_t channel){
			
			using out_type=typename std::iterator_traits<_OutputIter>::value_type;
			//	check _Inputiter::value_type (pointer to raw data)
			static_assert(std::is_same<
					typename std::iterator_traits<_InputIter>::value_type,char_type
				>::value);
			static_assert(0<_NumChannels);
			
			//	test input and output range
			if(src_end<=src_begin||dst_end<=dst_begin)
				throw std::logic_error("channel_filter_base::make (FATAL): invalid iterator values\n");
			
			//	number of complex items in input data
			size_t in_complex_size=(src_end-src_begin)/(2*sizeof(in_type)*_NumChannels);
			//	input data must contain integer number of complex items
			assert(!((src_end-src_begin)%(2*sizeof(in_type)*_NumChannels)));
			//	test channel number
			channel%=_NumChannels;
			
			//	check number of output complex items
			size_t out_complex_size=(dst_end-dst_begin)/2;
			if(out_complex_size<in_complex_size)
				throw std::logic_error("channel_filter_base::make (FATAL): output range is too short\n");

			//	set input iterator to the first value of specified channel
			in_type * _p=(in_type*)(src_begin)+channel*2;
			in_type * _end=(in_type*)(src_end);
			//	set output iterator
			out_type * out_it=static_cast<out_type*>(dst_begin);
			
			if(std::is_same<order_type,interleaved_ordering_tag>::value){
				//	interleaved storage
				for(size_t i=0;_p<_end;i++){
					//	real part
					*out_it++=static_cast<out_type>(*_p++);
					//	imag part must be included (see above)
					assert(_p<_end);
					//	imag part
					*out_it++=static_cast<out_type>(*_p);
					//	ignore all other channels
					_p+=(_NumChannels-1)*2+1;
				}
			}	//	interleaved storage
			else{
				//	not realized yet
				return;
			}
		}	//	make
		//	static std::chrono::time_point<std::chrono::steady_clock> time_/*	{std::chrono::steady_clock::now()}	*/;
	};
	
	//	channel filter for real type
	template <typename _DataType, typename _OrderingTag, uint8_t _NumChannels>
	struct channel_filter_base<_DataType, _OrderingTag, _NumChannels, false>
	{
		using char_type=char;
		using data_type=_DataType;
		using ordering_type=_OrderingTag;
		
		template <typename _InputIter, typename _OutputIter>
		static void select(
			_OutputIter dst_begin,
			_OutputIter dst_end,
			_InputIter src_begin,
			_InputIter src_end,
			uint8_t channel){
			
			using out_type=typename std::iterator_traits<_OutputIter>::value_type;
			//	check _Inputiter::value_type (pointer to raw data)
			static_assert(std::is_same<
				typename std::iterator_traits<_InputIter>::value_type,
				char_type>::value);
			static_assert(0<_NumChannels);
			
			//	test input and output range
			if(src_end<=src_begin||dst_end<=dst_begin)
				throw std::logic_error("channel_filter_base::make (FATAL): invalid iterator values\n");
			
			//	number of items in input data
			size_t in_size=(src_end-src_begin)/(sizeof(data_type)*_NumChannels);
			//	input data must contain integer number of items
			assert(!((src_end-src_begin)%(sizeof(data_type)*_NumChannels)));
			//	test channel number
			channel%=_NumChannels;
			
			//	check number of output items
			size_t out_size=dst_end-dst_begin;
			if(out_size<in_size)
				throw std::logic_error("channel_filter_base::make (FATAL): output range is too short\n");

			//	set input iterator to the first value of specified channel
			data_type * it=(data_type*)(src_begin)+channel;
			data_type * end=(data_type*)(src_end);
			//	set output iterator
			_OutputIter out_it=(dst_begin);

			if(std::is_same<ordering_type,interleaved_ordering_tag>::value){
				//	interleaved storage
				for(;it<end;){
					*out_it++=(out_type)(*it);
					//	ignore all other channels
					it+=_NumChannels;
				}	
			}
			else{
				//	sequential storage (not realized yet)
				return;
			}
		}	//	select
	};
}

}

#endif
