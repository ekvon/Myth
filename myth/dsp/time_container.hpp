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
	//	@_Duration - impl. of std::chrono::duration
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
			time_container():start_(0), delta_(0), size_(0){
			}
			
			time_container(size_type size, tick_type start=0, tick_type delta=1):delta_(delta), start_(start), size_(size){
			}
			
			//	tick-interface
			tick_type start_tick()const{
				return start_;
			}
			
			tick_type tick(size_type n)const{
				n%=size_;
				return start_+static_cast<tick_type>(delta_*n);
			}
			
			tick_type end_tick()const{
				return start_+static_cast<tick_type>(delta_*size_);
			}
			
			//	duration interface
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
				tick_type end_tick=start_+static_cast<tick_type>(delta_*size_);
				auto sr_time=sr_type(end_tick);
				auto time=duration_cast<duration_type>(end_tick);
				return time.count();
			}
			
			size_type tick_index_by_time(tick_type tick)const{
				if(tick<start_)
					throw std::invalid_argument("time value is less than start time\n");
				tick_type end=this->end_tick();
				if(end<tick)
					throw std::invalid_argument("time value is greater than end time\n");
				return static_cast<size_type>((tick-start_)/delta_);
			}
			
			typename duration_type::rep operator[](size_type n)const{
				return this->time(n);
			}
			
			size_type operator()(tick_type tick)const{
				return this->tick_index_by_time(tick);
			}
			
			//	total number of ticks
			size_type size()const{
				return size_;
			}
			
			bool is_inited()const{
				return size_!=0;
			}
			
		private:
			tick_type start_;
			tick_type delta_;
			size_type size_;
	};
}

}

#endif
