#ifndef __Myth__Dsp__Time_Container__Hpp
#define __Myth__Dsp__Time_Container__Hpp

namespace dsp
{
	template <bool v>
	struct uniform_time_tag
	{
		enum {value=v};
	};
	
	template <typename V=float>
	class tc_samplerate
	{
		public:
			using value_type=V;
			using size_type=size_t;
			using uniform_type=uniform_time_tag<true>;
			
		public:
			tc_samplerate(value_type sr, value_type start, size_type size):sr_(sr), start_(start), size_(size){
			}
			
			value_type start_time()const{
				return start_;
			}
			
			value_type end_time()const{
				return start_+(1.0/sr_)*size_;
			}
			
			//	duration of k-th time interval (the same for all intervals)
			value_type duration(size_type k)const{
				return 1/sr_;
			}
			
			//	total number of time samples
			size_type size()const{
				return size_;
			}
			
		private:
			value_type sr_;
			value_type start_;
			size_type size_;
	};
}

#endif
