// f(x) = x^3 - x - 2
#include "../libs/mpreal.h"
#include "../libs/Interval.h"
using namespace interval_arithmetic;

extern "C" {
    long double f  (long double x) { return x*x*x - x - 2.0L; }
    long double df (long double x) { return 3.0L*x*x - 1.0L; }
    long double ddf(long double x) { return 6.0L*x; }

    void f_iv  (long double a, long double b, long double* ra, long double* rb) { 
        Interval<mpreal> x = Interval<mpreal>(mpreal(a),mpreal(b));
        Interval<mpreal> r = x*x*x - x - IntRead<mpreal>("2");
        *ra=(long double)r.a;
        *rb=(long double)r.b; 
    }

    void df_iv (long double a, long double b, long double* ra, long double* rb) { 
        Interval<mpreal> x = Interval<mpreal>(mpreal(a),mpreal(b));
        Interval<mpreal> r = IntRead<mpreal>("3")*x*x - IntRead<mpreal>("1");
        *ra=(long double)r.a; 
        *rb=(long double)r.b; 
    }

    void ddf_iv(long double a, long double b, long double* ra, long double* rb) { 
        Interval<mpreal> x = Interval<mpreal>(mpreal(a),mpreal(b));
        Interval<mpreal> r = IntRead<mpreal>("6")*x;
        *ra=(long double)r.a; 
        *rb=(long double)r.b; 
    }
}
