#ifndef __Dsp__Window__Hpp
#define __Dsp__Window__Hpp

#include <complex>
#include <functional>
#include <vector>

#include <cmath>
#include <cstring>

namespace dsp
{
	template <typename T, size_t _N>
	struct real_window
	{
		using value_type=T;
		using size_type=size_t;
		
		real_window(){
			memset(h_, 0, _N*sizeof(value_type));
		}
		
		real_window(value_type * h){
			memcpy(h_, h, _N*sizeof(value_type));
		}
		
		//	WindowConcept
		size_type size()const{
			return _N;
		}
		value_type& operator[](size_type n){
			return h_[n%_N];
		}
		value_type operator[](size_type n)const{
			return h_[n%_N];
		}
		//	impulse characteristic
		value_type h_[_N];
	};
	
	template <typename T, size_t _N>
	struct rect_window
	{
	public:
		using value_type=T;
		using size_type=size_t;
		
	public:
		size_type size()const{
			return _N;
		}
		value_type operator[](size_type n)const{
			return 1.0;
		}
	};

	//	the general window function for this family is
	//		w[n]=sum(k, 0, _K){(-1)^k*a_[k]*cos(2*M_PI*n/(_N-1))},
	//	where _K, _N are positive integer parameters. In particular,
	//	Hann window parametrs are _K==1 ,a_[0], a_[1]=1-a_[0]=0.5,
	//	Hamming window parameters are _K==1, a_[0]=0.54, a_[1]=1-a_[0]=0.46.
	template <typename T, size_t _K, size_t _N>
	class cosine_sum_window
	{
	public:
		using value_type=T;
		using size_type=size_t;
		using complex_type=std::complex<value_type>;
		
	public:
		cosine_sum_window(value_type * a){
			for(size_type k=0;k<_K;k++){
				a_[k]=*a++;
			}
			for(size_type n=0;n<_N;n++){
				h_[n]=0.0;
				//	initial value of the sign: -1.0^0
				value_type sign=1.0;
				for(size_type k=0;k<_K;k++){
					h_[n]+=static_cast<value_type>(sign*a_[k]*cos(2*M_PI*n/(_N-1)));
					//	change the sign of the next item
					sign*=-1.0;
				}
			}
		}
		
		size_type size()const{
			return _N;
		}
			
		value_type operator[](size_type n)const{
			return h_[n%_N];
		}
		
	private:
		value_type a_[_K];
		value_type h_[_N];
	};
	
	//	complex frequency response
	template <typename T=float>
	struct cfr
	{
		using value_type=T;
		using size_type=size_t;
		using complex_type=std::complex<value_type>;
		
		cfr(value_type f, size_type N):f_(f), N_(N){
		}
		
		void set_params(value_type f, size_type N){
			f_=f;
			N_=N;
		}
		
		template <typename _WinType>
		void get(_WinType win){
			value_type w=2*M_PI*f_;
			complex_type value;
			mag_.clear();
			ph_.clear();
			//	for each frequency
			for(size_type n=0;n<N_;n++){
				value=std::complex<value_type>(0, 0);
				for(size_type k=0;k<win.size();k++){
					value+=win[k]*std::exp(-w*n*k)*1000;
				}
				mag_.push_back(std::abs(value));
			}
		}
		//	number of frequencies
		size_type N_;
		//	frequency stride
		value_type f_;
		//	magnitude values
		std::vector<value_type> mag_;
		//	phase values
		std::vector<value_type> ph_;
	};
}

#endif//__Dsp__Window__Hpp
