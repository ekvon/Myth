#ifndef __Myth__Dsp__Complex_Filter_Hpp
#define __Myth__Dsp__Complex_Filter_Hpp

#include <functional>

namespace myth
{

namespace dsp
{
	template <typename _BaseType, size_t _Size>
	class ComplexFilter
	{
	public:
		typedef _BaseType base_type;
		typedef _BaseType value_type[2];
		typedef size_t size_type;
		//	pointer to function which multiply to arrays of complex numbers and store result in output array
		typedef std::function<void(value_type*,value_type*,value_type*)> multiplier_type;
		
	public:
		ComplexFilter():inited_(false){
		}
		~ComplexFilter(){
		}
		
	public:
		//	Params:
		//	@coefs - pointer to continuous section of memory containing real coefficients
		void loadRealCoefs(base_type * coefs){
			for(size_type i=0;i<_Size;i++){
				//	real
				coefs_[i][0]=coefs[i];
				//	imag
				coefs_[i][1]=0;
			}
			inited_=true;
		}
		//	Params:
		//	@coefs - pointer to continuous section of memory containing complex coefficients
		//		Number of coefs. must correspond to template parameter _CoefsNum (controlled by the user).
		void loadComplexCoefs(value_type * coefs){
			for(size_type i=0;i<_Size;i++){
				//	real
				coefs_[i][0]=coefs[i][0];
				//	imag
				coefs_[i][1]=coefs[i][1];
			}
			inited_=true;
		}
		
		//	Process real or complex input data and store the result in output buffer.
		//	The size of the input data and output buffer MUST be the same and corresponds to template parameter _Size.
		//	If output buffer is not pointed then internal buffer is used.
		//	Params:
		//	@data - complex data in the following format [re0, im0, re1, im1, ...]
		//	@out - output buffer
		template <typename T>
		void processComplex(T * data, value_type * out=nullptr){
			if(!inited_)
				return;
				
			clear();
			if(out==nullptr)
				//	store output in internal buffer
				out=out_;
			//	Multiplication of two complex array. The following formula is used:
			//		(a+j*b)*(c+j*d)=(ac-bd)+j*(ad+bc)
			for(size_type i=0;i<_Size;i++){
				//	real
				out[i][0]=coefs_[i][0]*data[2*i]-coefs_[i][1]*data[2*i+1];
				//	imag
				out[i][1]=coefs_[i][1]*data[2*i+1]+coefs_[i][1]*data[2*i];
			}
		}
		
		template <typename T>
		void processReal(T * data, value_type * out=nullptr){
			if(!inited_)
				return;
				
			clear();
			if(out==nullptr)
				//	store output in internal buffer
				out=out_;
			for(size_type i=0;i<_Size;i++){
				//	real
				out[i][0]=coefs_[i][0]*data[i];
				//	imag
				out[i][1]=coefs_[i][1]*data[i];
			}
		}
		
		//	Copy output data from specified offset to the buffer. All parameters are expressed in complex items.
		//	Params:
		//	@len - length of required data in complex items
		//	@off - offset from the begining of output array in complex items
		//	Return number of copied complex items;
		size_type getData(void * buf, size_type len, size_type off){
			if(!len||_Size<=off)
				return 0;
				
			if(_Size<off+len){
				//	reduction of requested length
				len=_Size-off;
			}
			size_type len_in_bytes=len*sizeof(value_type);
			memcpy(buf, out_+off, len_in_bytes);
			return len;
		}
		
		//	return number of complex coefs
		size_type size()const{
			return _Size;
		}
		
		//	clear output array
		void clear(){
			memset(out_,0,_Size*sizeof(value_type));
		}
		
	private:
		bool inited_;
		//	allocate static memory for real and complex parts
		value_type coefs_[_Size];
		value_type out_[_Size];
	};
}

}

#endif
