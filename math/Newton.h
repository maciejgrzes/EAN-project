#pragma once

#include "../common/Types.h"
#include "../ui/OutputBox.h"

void runNewtonRaphsonReal(
        long double x0, 
        RealFn f, 
        RealFn df, 
        RealFn ddf, 
        OutputBox& out, 
        int MAX_ITER, 
        long double epsilon);

void runNewtonRaphsonInterval(
        Interval<long double> x0, 
        IntervalFn f, 
        IntervalFn df, 
        IntervalFn ddf, 
        OutputBox& out, 
        int MAX_ITER, 
        int MAX_DEPTH, 
        long double epsilon);

void runNewtonRaphsonFromPoint(
        long double x0, 
        RealFn f, 
        RealFn df, 
        RealFn ddf, 
        IntervalFn fi, 
        IntervalFn dfi, 
        IntervalFn ddfi, 
        OutputBox& out, 
        int MAX_ITER, 
        int MAX_DEPTH,
        long double epsilon);
