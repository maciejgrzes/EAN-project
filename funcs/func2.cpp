// f(x) = cos(x) - x,  root ≈ 0.7391 (Dottie number)
#include "../libs/mpreal.h"
#include "../libs/Interval.h"
using namespace interval_arithmetic;
extern "C" {
    long double f  (long double x) { return cosl(x) - x; }
    long double df (long double x) { return -sinl(x) - 1.0L; }
    long double ddf(long double x) { return -cosl(x); }

    void f_iv  (long double a, long double b, long double* ra, long double* rb) {
        Interval<mpreal> x = Interval<mpreal>(mpreal(a), mpreal(b));
        Interval<mpreal> r = ICos(x) - x;
        *ra = (long double)r.a; *rb = (long double)r.b;
    }

    void df_iv (long double a, long double b, long double* ra, long double* rb) {
        Interval<mpreal> x = Interval<mpreal>(mpreal(a), mpreal(b));
        Interval<mpreal> r = -1 * ISin(x) - IntRead<mpreal>("1");
        *ra = (long double)r.a; *rb = (long double)r.b;
    }

    void ddf_iv(long double a, long double b, long double* ra, long double* rb) {
        Interval<mpreal> x = Interval<mpreal>(mpreal(a), mpreal(b));
        Interval<mpreal> r = -1 * ICos(x);
        *ra = (long double)r.a; *rb = (long double)r.b;
    }
}
