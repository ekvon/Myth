#ifndef __Myth__Complex__Complex__Hpp
#define __Myth__Complex__Complex__Hpp

#include <complex>
#include <string>

namespace myth
{

namespace complex
{
	template <typename T, size_t _N>
	class fixed_array
	{
	public:
		enum {N=_N};
		enum format_type {csv};
		
		using real_type=T;
		using value_type=std::complex<T>;
		using size_type=size_t;
		
		fixed_array(value_type * data=nullptr){
			if(data==nullptr)
				this->clear();
			else
				this->load(data);
		}
		
		//	load data of specified length to the specified offset
		//	Params:
		//	@data (in) - data to load
		size_type load(value_type * data, size_type off=0, size_type len=N){
			if(N<=off)
				return 0;
			if(N<(off+len))
				//	reduce the length
				len=N-off;
			//	copy data
			memcpy(data_+off, data, len*sizeof(value_type));
			return len;
		}
		
		//	return magnitudes of complex samples in specified format
		std::string mag_to_str(format_type format)const{
			std::string str;
			real_type mag;
			char buf[0x10];
			
			switch(format){
			case(format_type::csv):{
				mag=std::abs(data_[0]);
				sprintf(buf, "%.6f", mag);
				str+=std::string(buf);
				for(size_type i=1;i<N;i++){
					mag=std::abs(data_[i]);
					sprintf(buf, ", %.6f", mag);
					str+=std::string(buf);
				}
				break;
			}
			default:return str;
			}
			str+='\n';
			return str;
		}
		
		size_type size()const{
			return N;
		}
		
		void clear(){
			memset(data_, 0, N*sizeof(value_type));
		}
		
	private:
		value_type data_[N];
	};
}

}

#endif
