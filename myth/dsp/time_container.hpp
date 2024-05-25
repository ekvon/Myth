#ifndef __Myth__Dsp__Time_Container__Hpp
#define __Myth__Dsp__Time_Container__Hpp

#include <chrono>
#include <stdexcept>

using namespace std;
using namespace std::chrono;

namespace myth
{

namespace dsp
{
	struct uniform_time_tag
	{};
	struct random_time_tag
	{};
	
	//	Template parameters:
	//	@_DurationBase - duration between two adjacent samples of signal, i.e. sample rate (impl. of std::chrono::duration)
	//	@_Duration - time that's used in time interface (impl. of std::chrono::duration) 
	template <typename _DurationBase, typename _Duration, typename _TimeTag> class time_container;
	
	template <typename _SampleRate, typename _Duration>
	class time_container<_SampleRate, _Duration, uniform_time_tag>
	{
		public:
			using sr_type=_SampleRate;
			using duration_type=_Duration;
			using tick_type=typename sr_type::rep;
			using period_type=typename sr_type::period;
			using size_type=size_t;
			
		public:
			time_container(size_type size = 0, tick_type start = 0):start_(start), size_(size){
			}
			
			//	tick-interface (_SampleRate is used as duration type)
			tick_type start_tick()const{
				return start_;
			}
			
			tick_type tick(tick_type n)const{
				n%=size_;
				return start_+n;
			}
			
			tick_type end_tick()const{
				return start_+size_;
			}
			
			//	duration (time) interface
			typename duration_type::rep start_time()const{
				auto sr_time=sr_type(start_);
				auto time=duration_cast<duration_type>(sr_time);
				return time.count();
			}
			
			typename duration_type::rep time(size_type n)const{
				tick_type t=this->tick(n);
				auto sr_time=sr_type(t);
				auto time=duration_cast<duration_type>(sr_time);
				return time.count();
			}
			
			//	duration of k-th time interval (the same for all intervals)
			typename duration_type::rep end_time()const{
				tick_type end_tick=start_+size_;
				auto sr_time=sr_type(end_tick);
				auto time=duration_cast<duration_type>(sr_time);
				return time.count();
			}
			
			//	return tick of sample rate duration that corresponds the tick specified for time duration
			size_type tick_index_by_time(tick_type time_tick) const {
				if(time_tick < start_time() || end_time() <= time_tick)
					throw std::invalid_argument("invalid time_tick value\n");
					
				duration_type d(time_tick);
				sr_type sr_time=duration_cast<sr_type>(d);
				tick_type tick=sr_time.count();
				assert(start_ <= tick && tick < start_ + size_);
				return tick;
			}
			
			typename duration_type::rep operator[](size_type n)const{
				return this->time(n);
			}
			
			size_type operator()(tick_type tick)const{
				return this->tick_index_by_time(tick);
			}
			
			//	total number of duration ticks
			size_type duration_size() const {
				return this->start_time() - this->end_time();
			}
			
			//	total number of sample rate ticks
			size_type size()const{
				return size_;
			}
			
			bool is_inited()const{
				return size_!=0;
			}
			
		private:
			tick_type start_;
			tick_type size_;
	};
}

}

#endif
