#pragma once

enum INTERPOLATION_TYPE {
	InterpolationTypeBilinear = 1,
	InterpolationTypeBicubicPolynomial_050 = 2,		// a = -0.5 (default)
	InterpolationTypeBicubicPolynomial_075 = 3,		// a = -0.75
	InterpolationTypeBicubicPolynomial_100 = 4,		// a = -1.0 
	InterpolationTypeBicubicPolynomial_000 = 5,		// a = -0.0 
	InterpolationTypeBicubicPolynomial_300 = 6,		// a = -3.0 
	InterpolationTypeBicubicBSpline = 7,
	InterpolationTypeBicubicMichell = 8,			// B = 1/3, C = 1/3
	InterpolationTypeBicubicLanczos = 9,
	InterpolationTypeBicubicCatmullRom = 10,
};
