#pragma once

#include "../common/Types.h"

template<typename T>
bool ContainsZero(const Interval<T>& x) {
    return x.a <= T(0) && x.b >= T(0);
}

Interval<mpreal> ParseInterval(const string& input);
bool loadFunctions(const string& path, RealFn& f, RealFn& df, RealFn& ddf, IntervalFn& f_iv, IntervalFn& df_iv, IntervalFn& ddf_iv);
