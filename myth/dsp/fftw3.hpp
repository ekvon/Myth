#ifndef __Myth__Dsp__Fftw3__Hpp
#define __Myth__Dsp__Fftw3__Hpp

#include <fftw3.h>

#include <type_traits>

namespace myth
{

namespace dsp
{
	struct c2c_transform_tag
	{};
	struct r2c_transform_tag
	{};
	struct r2r_transform_tag
	{};
	
	template <typename T, typename _TransformType, uint8_t _Rank, size_t _N> class fftw3;
	
	//	C++ wrappers for libfftw3
	template <size_t _N>
	class fftw3<float, c2c_transform_tag, 1, _N>
	{
	public:
		enum {Size=_N};
		using complex_type=fftwf_complex;
		using plan_type=fftwf_plan;
		
		fftw3(){
			in_=fftwf_alloc_complex(Size);
			out_=fftwf_alloc_complex(Size);
			plan_=fftwf_plan_dft_1d(Size,in_,out_,FFTW_BACKWARD,FFTW_MEASURE|FFTW_PRESERVE_INPUT);
		}
		
		void load_complex_data(complex_type * in){
			memcpy(in_, in, Size*sizeof(complex_type));
		}
		void load_float_data(float * in){
			memcpy(in_, in, Size*sizeof(float)*2);
		}
		
		void execute(){
			fftwf_execute(plan_);
		}
		
		//	return magnitudes of complex output in csv-format
		std::string magnitude(){
			std::string value;
			float mag;
			char buf[0x10];
			for(size_t i=0;i<Size;i++){
				mag=sqrtf(out_[i][0]*out_[i][0]+out_[i][1]*out_[i][1]);
				if(!i)
					//	the first value
					sprintf(buf, "%.2f", mag);
				else if(i<Size-1)
					sprintf(buf, ",%.2f", mag);
				else
					//	the last value (with eol)
					sprintf(buf, ",%.2f\n", mag);
				value+=std::string(buf);
			}
			return value;
		}
		
	private:
		complex_type * in_;
		complex_type * out_;
		plan_type plan_;
	};
}

}

#endif
