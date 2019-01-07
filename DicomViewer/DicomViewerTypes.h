#pragma once

enum INTERPOLATION_TYPE {
	eBilinear = 1,
	eBicubicPolynomial_050 = 2,		// a = -0.5 (default)
	eBicubicPolynomial_075 = 3,		// a = -0.75
	eBicubicPolynomial_100 = 4,		// a = -1.0 
	eBicubicPolynomial_000 = 5,		// a = -0.0 
	eBicubicPolynomial_300 = 6,		// a = -3.0 
	eBicubicBSpline = 7,
	eBicubicMichell = 8,			// B = 1/3, C = 1/3
	eBicubicLanczos = 9,
};
