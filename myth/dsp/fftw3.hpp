#ifndef __Myth__Dsp__Fftw3__Hpp
#define __Myth__Dsp__Fftw3__Hpp

#include <fftw3.h>

#include <cstring>
#include <type_traits>
#include <vector>

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
		enum {n0=_N};
		using real_type=float;
		using complex_type=fftwf_complex;
		using plan_type=fftwf_plan;
		
		fftw3(){
			in_=fftwf_alloc_complex(n0);
			out_=fftwf_alloc_complex(n0);
			plan_=fftwf_plan_dft_1d(n0, in_, out_, FFTW_BACKWARD, FFTW_MEASURE|FFTW_PRESERVE_INPUT);
		}
		
		void put(complex_type * in)noexcept{
			memcpy(in_, in, n0*sizeof(complex_type));
		}
		
		void execute()noexcept{
			fftwf_execute(plan_);
		}
		
		void get(complex_type * buf)const{
			memcpy(buf, out_, n0*sizeof(complex_type));
		}
		
	private:
		complex_type * in_;
		complex_type * out_;
		plan_type plan_;
	};
	
	//	C++ wrappers for libfftw3
	template <size_t _N>
	class fftw3<float, r2c_transform_tag, 1, _N>
	{
	public:
		enum { n0= _N };
		
		using real_type = float;
		using complex_type = fftwf_complex;
		using plan_type = fftwf_plan;
		using size_type = size_t;
		
		fftw3(unsigned flags = FFTW_MEASURE|FFTW_PRESERVE_INPUT){
			//	allocate memory
			in_ = fftwf_alloc_real(n0);
			out_ = fftwf_alloc_complex(n0);
			//	create plan
			plan_ = fftwf_plan_dft_r2c_1d(n0, in_, out_, flags);
		}
		
		template <typename T>
		void put(T * in) noexcept{
			for(size_type i = 0;i < n0;i++){
				in_[i]=static_cast<real_type>(*in++);
			}
		}
		
		//	put input data
		void put(real_type * in) noexcept{
			memcpy(in_, in, n0 * sizeof(real_type));
		}
		
		void execute() noexcept{
			fftwf_execute(plan_);
		}
		
		//	copy all available output data to the buffer
		void get(complex_type * buf){
			memcpy(buf, out_, n0 * sizeof(complex_type));
		}
		
		private:
			real_type * in_;
			complex_type * out_;
			fftwf_plan plan_;
	};
}

}

#endif
