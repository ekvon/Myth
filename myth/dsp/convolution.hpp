#ifndef __Myth__Dsp__Convolution__Hpp
#define __Myth__Dsp__Convolution__Hpp

namespace myth
{

namespace dsp
{
	template <
		typename _InputIter1,
		typename _InputIter2,
		typename _OutIter,
		bool _IsComplex1,
		bool _IsComplex2
	>
	struct array_multiplier;
	
	//	Multiplier of real arrays
	template <
		typename _InputIter1,
		typename _InputIter2,
		typename _OutIter>
	struct array_multiplier<
		_InputIter1,
		_InputIter2,
		_OutIter,
		false,
		false>
	{
		//	All input and output arrays must have the same specified length
		static void mul(_InputIter1 real1, _InputIter2 real2, _OutIter out, size_t len){
			for(size_t i=0;i<len;i++){
				*(out+i)=*(real1+i)**(real2+i);
			}
		}
	};
	
	//	Multiplier for complex and real data. Both real and imag parts of complex number are multiplied on the same real number.
	//	That's input array for complex numbers must be twice as much as the input array for real numbers.
	//	Output array must have the same length as the input array for complex data.
		//	Multiplier of real arrays
	template <
		typename _InputIter1,
		typename _InputIter2,
		typename _OutIter>
	struct array_multiplier<
		_InputIter1,
		_InputIter2,
		_OutIter,
		true,
		false>
	{
		//	All input and output arrays must have the same specified length
		static void mul(_InputIter1 complex, _InputIter2 real, _OutIter out, size_t len){
			for(size_t i=0;i<len;i++){
				//	real
				*(out++)=*(real+i)**(complex++);
				//	imag
				*(out++)=*(real+i)**(complex++);
			}
		}
	};
}	//	dsp

}	//	myth
#endif
