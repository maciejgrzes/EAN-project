#pragma once

#include "../libs/mpreal.h"
#include "../libs/Interval.h"
#include <functional>
#include <string>

using namespace mpfr;
using namespace interval_arithmetic;

using RealFn = std::function<long double(long double)>;
using IntervalFn = std::function<Interval<mpreal>(Interval<mpreal>)>;

typedef long double (*FnPtr)(long double);
typedef void (*IVFnPtr)(long double, long double, long double*, long double*);

const int MAX_ITER = 100;
const int MAX_DEPTH = 50;
