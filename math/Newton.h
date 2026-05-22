#pragma once

#include "../common/Types.h"
#include "../ui/OutputBox.h"

void runNewtonRaphsonReal(long double x0, RealFn f, RealFn df, RealFn ddf, OutputBox& out, const int MAX_ITER);

void runNewtonRaphsonInterval(Interval<mpreal> x0, IntervalFn f, IntervalFn df, IntervalFn ddf, OutputBox& out, const int MAX_ITER, const int MAX_DEPTH);

void runNewtonRaphsonFromPoint(long double x0, RealFn f, RealFn df, RealFn ddf, IntervalFn fi, IntervalFn dfi, IntervalFn ddfi, OutputBox& out, const int MAX_ITER, const int MAX_DEPTH);
