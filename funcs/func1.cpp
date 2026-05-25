// f(x) = x^3 - x - 2
#include "../libs/mpreal.h"
#include "../libs/Interval.h"
using namespace interval_arithmetic;

extern "C" {
    long double f  (long double x) { return x*x*x - x - 2.0L; }
    long double df (long double x) { return 3.0L*x*x - 1.0L; }
    long double ddf(long double x) { return 6.0L*x; }

    void f_iv  (long double a, long double b, long double* ra, long double* rb) { 
        Interval<long double> x = Interval<long double>(a, b);
        Interval<long double> r = x*x*x - x - IntRead<long double>("2");
        *ra=(long double)r.a;
        *rb=(long double)r.b; 
    }

    void df_iv (long double a, long double b, long double* ra, long double* rb) { 
        Interval<long double> x = Interval<long double>(a, b);
        Interval<long double> r = IntRead<long double>("3")*x*x - IntRead<long double>("1");
        *ra=(long double)r.a; 
        *rb=(long double)r.b; 
    }

    void ddf_iv(long double a, long double b, long double* ra, long double* rb) { 
        Interval<long double> x = Interval<long double>(a, b);
        Interval<long double> r = IntRead<long double>("6")*x;
        *ra=(long double)r.a; 
        *rb=(long double)r.b; 
    }
}
