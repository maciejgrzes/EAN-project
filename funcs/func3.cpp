// f(x) = sin(x)^2 + 0.5sin(x) - 0.5
#include "../libs/mpreal.h"
#include "../libs/Interval.h"
using namespace interval_arithmetic;
extern "C" {
    long double f  (long double x) { return sinl(x) * sinl(x) + 0.5L * sinl(x) - 0.5L ; }
    long double df (long double x) { return sinl(2.0L*x) + 0.5L * cosl(x); }
    long double ddf(long double x) { return 2.0L * cosl(2.0L*x) - 0.5L * sinl(x); }

    void f_iv  (long double a, long double b, long double* ra, long double* rb) {
        Interval<long double> x = Interval<long double>(a, b);
        Interval<long double> r = ISin(x) * ISin(x) + IntRead<long double>("0.5") * ISin(x) - IntRead<long double>("0.5");
        *ra = (long double)r.a; *rb = (long double)r.b;
    }

    void df_iv (long double a, long double b, long double* ra, long double* rb) {
        Interval<long double> x = Interval<long double>(a, b);
        Interval<long double> r = ISin(2.0L*x) + IntRead<long double>("0.5") * ICos(x);
        *ra = (long double)r.a; *rb = (long double)r.b;
    }

    void ddf_iv(long double a, long double b, long double* ra, long double* rb) {
        Interval<long double> x = Interval<long double>(a, b);
        Interval<long double> r = IntRead<long double>("2") * ICos(2.0L*x) - IntRead<long double>("0.5") * ISin(x);
        *ra = (long double)r.a; *rb = (long double)r.b;
    }
}
