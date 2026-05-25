#pragma once

#include "../libs/mpreal.h"
#include "../libs/Interval.h"
#include <functional>
#include <string>

using namespace mpfr;
using namespace interval_arithmetic;

using RealFn = std::function<long double(long double)>;
using IntervalFn = std::function<Interval<long double>(Interval<long double>)>;

typedef long double (*FnPtr)(long double);
typedef void (*IVFnPtr)(long double, long double, long double*, long double*);
