// f(x) = x^2 - 2
#include "../libs/mpreal.h"
#include "../libs/Interval.h"
using namespace interval_arithmetic;

extern "C" {
    long double f  (long double x) { return x*x - 2.0L; }
    long double df (long double x) { return 2.0L*x; }
    long double ddf(long double x) { return 2.0L; }

    void f_iv  (long double a, long double b, long double* ra, long double* rb) { 
        Interval<mpreal> r = Interval<mpreal>(mpreal(a),mpreal(b)) * Interval<mpreal>(mpreal(a),mpreal(b)) - IntRead<mpreal>("2");
        *ra=(long double)r.a;
        *rb=(long double)r.b; 
    }

    void df_iv (long double a, long double b, long double* ra, long double* rb) { 
        Interval<mpreal> r = IntRead<mpreal>("2") * Interval<mpreal>(mpreal(a),mpreal(b));
        *ra=(long double)r.a; 
        *rb=(long double)r.b; 
    }

    void ddf_iv(long double a, long double b, long double* ra, long double* rb) { 
        *ra = 2.0L; 
        *rb = 2.0L; 
    }
}
