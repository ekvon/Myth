#ifndef __Myth__Dsp__Complex_Filter_Hpp
#define __Myth__Dsp__Complex_Filter_Hpp

#include <functional>

namespace myth
{

namespace dsp
{
	template <typename _BaseType, size_t _ComplexSize>
	class ComplexFilter
	{
	public:
		typedef _BaseType base_type;
		typedef _BaseType value_type[2];
		typedef size_t size_type;
		//	pointer to function which multiply to arrays of complex numbers and store result in output array
		typedef std::function<void(value_type*,value_type*,value_type*)> multiplier_type;
		
		struct Plan
		{
			bool complexData;
			//	the length of input data
			size_type dataSize;
			//	stride between two adjacent windows
			size_type stride;
			//	number of filter applications
			size_type outSize;
			//	offset from the begining of the data at which filter application is stopped
			size_type finalOffset;
		};
		typedef Plan plan_type;
		
	public:
		ComplexFilter():inited_(false), planned_(false){
		}
		~ComplexFilter(){
		}
		
	public:
		//	Params:
		//	@coefs - pointer to continuous section of memory containing real coefficients
		void loadRealCoefs(base_type * coefs){
			for(size_type i=0;i<_ComplexSize;i++){
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
			for(size_type i=0;i<_ComplexSize;i++){
				//	real
				coefs_[i][0]=coefs[i][0];
				//	imag
				coefs_[i][1]=coefs[i][1];
			}
			inited_=true;
		}
		
		int makePlan(bool complex, size_type dataSize, size_type stride, size_type outSize)noexcept{
			plan_.complexData=complex;
			if(dataSize<_ComplexSize)
				return -1;
			plan_.dataSize=dataSize;
			if(!stride||_ComplexSize<stride)
				return -1;
			plan_.stride=stride;
			plan_.finalOffset=(outSize-1)*stride+_ComplexSize;
			if(dataSize<plan_.finalOffset)
				return -1;
			if(out_!=nullptr)
				delete[]out_;
			//	allocate output memory
			out_=new value_type(outSize);
			planned_=true;
			return 0;
		}
		
		void destroyPlan()noexcept{
			memset(plan_, 0, sizeof(plan_type));
			if(out_!=nullptr)
				delete[]out_;
		}
		
		//	Process real or complex input data and store the result in output buffer.
		//	The size of the input data and output buffer MUST be the same and corresponds to template parameter _ComplexSize.
		//	If output buffer is not pointed then internal buffer is used.
		//	Params:
		//	@data - pointer to the base type of complex data stored in the following format [re0, im0, re1, im1, ...]
		//	@out - output buffer
		template <typename T>
		void processComplex(T * data, value_type * out=nullptr){
			if(!inited_||!planned_)
				return;
			if(!plan_.complexData)
				return;
			
			if(out==nullptr){
				//	store output in internal buffer
				clear();
				out=out_;
			}
			//	the start position of current filter application
			T * start=data;
			//	iterator over input data
			T * it;
			base_type real, imag;
			for(size_type n=0;n<plan_.outSize;n++){
				//	Multiplication of two complex array. The following formula is used:
				//		(a+j*b)*(c+j*d)=(ac-bd)+j*(ad+bc)
				for(size_type i=0;i<_ComplexSize;i++){
					//	real
					real=static_cast<base_type>(coefs_[i][0]**(start+2*i)-coefs_[i][1]**(start+2*i+1));
					out[n][0]=real;
					//	imag
					imag=static_cast<base_type>coefs_[i][1]**(start+2*i+1)+coefs_[i][1]**(start+2*i);
					out[n][1]=imag;
				}
				//	move start position on plan_.stride value
				start+=plan_.stride*2;
			}
		}
		
		template <typename T>
		void processReal(T * data, value_type * out=nullptr){
			if(!inited_||!planned_)
				return;
			if(plan_.complexData)
				return;
				
			if(out==nullptr)
				//	store output in internal buffer
				clear();
				out=out_;
			}
			
			//	the start position of current filter application
			T * start=data;
			//	iterator over input data
			T * it;
			base_type real, imag;
			for(size_type n=0;n<plan_.outSize;n++){
				for(size_type i=0;i<_ComplexSize;i++){
					//	real
					out[i][0]=coefs_[i][0]**(start+i);
					//	imag
					out[i][1]=coefs_[i][1]**(start+i);
				}
				//	move start position on plan_.stride value
				start+=plan_.stride;
			}
		}
		
		//	Copy output data from specified offset to the buffer. All parameters are expressed in complex items.
		//	Params:
		//	@len - length of required data in complex items
		//	@off - offset from the begining of output array in complex items
		//	Return number of copied complex items;
		size_type getData(void * buf, size_type len, size_type off){
			if(!len||_ComplexSize<=off)
				return 0;
				
			if(_ComplexSize<off+len){
				//	reduction of requested length
				len=_ComplexSize-off;
			}
			size_type len_in_bytes=len*sizeof(value_type);
			memcpy(buf, out_+off, len_in_bytes);
			return len;
		}
		
		//	Copy real part of output from specified offset to the buffer.
		//	Params:
		//	@len - length of required data in COMPLEX items
		//	@off - offset from the begining of output array in COMPLEX items
		//	Return number of copied items
		size_type getReal(void * buf, size_type len, size_type off){
			if(!len||_ComplexSize<=off)
				return 0;
				
			if(_ComplexSize<off+len){
				//	reduction of requested length
				len=_ComplexSize-off;
			}
			BaseType * p=(BaseType*)(buf);
			for(size_t i=0;i<len){
				*p++=out_[i][0];
			}
		}
		
		//	Copy imag part of output from specified offset to the buffer.
		//	Params:
		//	@len - length of required data in COMPLEX items
		//	@off - offset from the begining of output array in COMPLEX items
		//	Return number of copied items
		size_type getImag(void * buf, size_type len, size_type off){
			if(!len||_ComplexSize<=off)
				return 0;
				
			if(_ComplexSize<off+len){
				//	reduction of requested length
				len=_ComplexSize-off;
			}
			BaseType * p=(BaseType*)(buf);
			for(size_t i=0;i<len){
				*p++=out_[i][1];
			}
		}
		
		//	return number of complex coefs
		size_type size()const{
			return _ComplexSize;
		}
		
		//	clear output array
		void clear(){
			memset(out_, 0, plan_.outSize*sizeof(value_type));
		}
		
	private:
		bool inited_;
		bool planned_;
		//	allocate static memory for real and complex parts
		value_type coefs_[_ComplexSize];
		value_type out_[_ComplexSize];
		//	
		plan_type plan_;
	};
}

}

#endif
