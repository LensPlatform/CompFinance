#pragma once

#include <math.h>
#include <vector>
#include <algorithm>
using namespace std;

#define EPS 1.0e-08

//  Classic Gaussian functions

//  Normal density
inline double normalDens(const double x)
{
    return x<-10.0 || 10.0<x ? 0.0 : exp(-0.5*x*x) / 2.506628274631;
}

//	Normal CDF (N in Black-Scholes)
//  Zelen and Severo's approximation (1964)
//  See https://en.wikipedia.org/wiki/Normal_distribution#Numerical_approximations_for_the_normal_CDF
inline double normalCdf(const double x)
{
	if (x < -10.0) return 0.0;
	if (x > 10.0) return 1.0;
    if (x < 0.0) return 1.0 - normalCdf(-x);

	static const double p = 0.2316419;
	static const double b1 = 0.319381530;
	static const double b2 = -0.356563782;
	static const double b3 = 1.781477937;
	static const double b4 = -1.821255978;
	static const double b5 = 1.330274429;

	const auto t = 1.0 / (1.0 + p*x);

    const auto pol = t*(b1 + t*(b2 + t*(b3 + t*(b4 + t*b5))));

    const auto pdf = normalDens(x);

	return 1.0 - pdf * pol;
}

//	Inverse CDF (for generation of Gaussians out of Uniforms)
//  Beasley-Springer-Moro algorithm
//  Moro, The Full Monte, Risk 1995
//  See Glasserman, Monte Carlo Methods in Financial Engineering, p 68
inline double invNormalCdf(const double p)
{
    const bool sup = p > 0.5;
    const double up = sup ? 1.0 - p : p;

	static const double a0 = 2.50662823884;
	static const double a1 = -18.61500062529;
	static const double a2 = 41.39119773534;
	static const double a3 = -25.44106049637;

	static const double b0 = -8.47351093090;
	static const double b1 = 23.08336743743;
	static const double b2 = -21.06224101826;
	static const double b3 = 3.13082909833;

	static const double c0 = 0.3374754822726147;
	static const double c1 = 0.9761690190917186;
	static const double c2 = 0.1607979714918209;
	static const double c3 = 0.0276438810333863;
	static const double c4 = 0.0038405729373609;
	static const double c5 = 0.0003951896511919;
	static const double c6 = 0.0000321767881768;
	static const double c7 = 0.0000002888167364;
	static const double c8 = 0.0000003960315187;

	double x = up - 0.5;
	double r;

	if (fabs(x)<0.42)
	{
		r = x*x;
		r = x*(((a3*r + a2)*r + a1)*r + a0) / ((((b3*r + b2)*r + b1)*r + b0)*r + 1.0);
		return sup ? -r: r;
	}

	r = up;
	r = log(-log(r));
	r = c0 + r*(c1 + r*(c2 + r*(c3 + r*(c4 + r*(c5 + r*(c6 + r*(c7 + r*c8)))))));

	return sup? r: -r;
}

