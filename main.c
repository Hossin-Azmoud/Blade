//   LIBROW FFT sample
//   Demonstrates how to use the FFT
//
//   The code is property of LIBROW
//   You can use it on your own
//   When utilizing credit LIBROW site

//   Include FFT header
#include "fft.h"
//   Output
#include <stdio.h>

//   Application entry point
int main(int argc, char *argv[])
{
	//   Output counter
	unsigned int i;

	//   Input signal {0, 1, 2, 3, 4, 5, 6, 7}
	TComplex Signal[8] = {{0., 0.}, {1., 0.}, {2., 0.}, {3., 0.}, {4., 0.}, {5., 0.}, {6., 0.}, {7., 0}};

	//   Process the signal with FFT
	FFTForwardInplace(Signal, 8);

	//   Output the result
	//   Real part
	printf("re: ");
	for (i = 0; i < 8; ++i)
		printf("%f ", Signal[i].re);
	//   Imaginary part
	printf("\nim: ");
	for (i = 0; i < 8; ++i)
		printf("%f ", Signal[i].im);
	printf("\n\n");
	//   Done
	return 0;
}
