#include "Newton.h"
#include "IntervalUtils.h"
#include <ios>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <string>
using namespace std;

long double DistanceToInterval(long double p, const Interval<long double>& x) {
    if (p < x.a) return x.a - p;
    if (p > x.b) return p - x.b;
    return 0.0L;
}

Interval<long double> InflateInterval(const Interval<long double>& x, long double r)
{
    ostringstream ss;
    ss << setprecision(numeric_limits<long double>::max_digits10) << r;

    string rs = ss.str();

    Interval<long double> rad;
    rad.a = LeftRead<long double>("-" + rs);
    rad.b = RightRead<long double>(rs);

    return IAdd(x, rad);
}

string FormatInterval(Interval<long double> x) {
    string lo, hi;
    x.IEndsToStrings(lo, hi);
    return "[" + lo + ", " + hi + "]";
}

Interval<long double> PointInterval(long double x) {
    ostringstream ss;
    ss << setprecision(numeric_limits<long double>::max_digits10) << x;

    string s = ss.str();

    Interval<long double> r;
    r.a = LeftRead<long double>(s);
    r.b = RightRead<long double>(s);

    return r;
}

void runNewtonRaphsonReal(long double x0, 
                          RealFn f,
                          RealFn df, 
                          RealFn ddf, 
                          OutputBox& out, 
                          const int MAX_ITER,
                          long double epsilon) 
{
    out.Clear();
    long double x = x0;

    for (int i = 0; i < MAX_ITER; i++) {
        long double fx   = f(x);
        long double dfx  = df(x);
        long double ddfx = ddf(x);

        if (fabsl(ddfx) < epsilon) {
            out.Add("Error: f''(x) = 0, cannot apply NR2");
            return;
        }

        long double disc = dfx*dfx - 2.0L*fx*ddfx;

        long double xn;
        if (disc < 0.0L) {
            out.Add("  disc<0 at iter " + to_string(i+1) + ", using Newton fallback");
            if (fabsl(dfx) < epsilon) { out.Add("Error: f'(x)=0 too"); return; }
            xn = x - fx / dfx;
        } else {
            long double sp = sqrtl(disc);
            long double x1 = x - (dfx - sp) / ddfx;
            long double x2 = x - (dfx + sp) / ddfx;
            xn = (fabsl(x2 - x) > fabsl(x1 - x)) ? x1 : x2;
        }

        long double step = fabsl(xn - x);

        ostringstream ss;
        ss << "Iter " << setw(2) << i + 1
           << ": x=" << fixed << setprecision(10) << xn
           << "  step=" << scientific << setprecision(2) << step;
        out.Add(ss.str());

        x = xn;

        if (step < 1e-15L || step / max(fabsl(x), 1.0L) < epsilon) {
            ostringstream root;
            root << scientific << setprecision(20) << x;
            out.Add("--- Roots: ---");
            out.Add("Root: " + root.str());
            return;
        }
    }
    out.Add("Max iterations reached.");
}


void runNewtonRaphsonInterval(
     Interval<long double> x0,
     IntervalFn f,
     IntervalFn df,
     IntervalFn ddf,
     OutputBox& out,
     const int MAX_ITER,
     long double epsilon) 
{
    out.Clear();

    const Interval<long double> two = IntRead<long double>("2");

    Interval<long double> x = x0;

    for (int i = 0; i < MAX_ITER; i++) {
        Interval<long double> old = x;

        long double mid = old.Mid();
        Interval<long double> m = PointInterval(mid);

        Interval<long double> fm   = f(m);
        Interval<long double> dfm  = df(m);
        Interval<long double> ddfx = ddf(old);

        if (ContainsZero(ddfx)) {
            out.Add("Error: f''(x) contains 0, cannot apply NR2.");
            out.Add("Current interval: " + FormatInterval(old));
            return;
        }

        Interval<long double> disc =
            ISub(
                IMul(dfm, dfm),
                IMul(IMul(two, fm), ddfx)
            );

        if (disc.b < 0.0L) {
            out.Add("Error: discriminant is strictly negative, cannot apply NR2.");
            out.Add("disc = " + FormatInterval(disc));
            out.Add("Current interval: " + FormatInterval(old));
            return;
        }

        Interval<long double> discClipped;
        discClipped.a = max(0.0L, disc.a);
        discClipped.b = disc.b;

        int st = 0;
        Interval<long double> sp = ISqrt(discClipped, st);

        if (st != 0) {
            out.Add("Error: could not compute sqrt(discriminant).");
            out.Add("disc = " + FormatInterval(disc));
            return;
        }

        Interval<long double> x1 =
            ISub(
                m,
                IDiv(ISub(dfm, sp), ddfx)
            );

        Interval<long double> x2 =
            ISub(
                m,
                IDiv(IAdd(dfm, sp), ddfx)
            );

        long double d1 = DistanceToInterval(mid, x1);
        long double d2 = DistanceToInterval(mid, x2);

        Interval<long double> xn = (d1 <= d2) ? x1 : x2;

        x = xn;

        long double step = max(
            fabsl(x.a - old.a),
            fabsl(x.b - old.b)
        );

        long double scale = max(fabsl(x.Mid()), 1.0L);
        long double tol = epsilon * scale;

        ostringstream ss;
        ss << "Iter " << setw(2) << i + 1
           << ": " << FormatInterval(x)
           << " w=" << scientific << setprecision(2) << IntWidth(x)
           << " step=" << step;

        out.Add(ss.str());

        if (step < tol) {
            Interval<long double> root = InflateInterval(x, tol);

            ostringstream ss;
            out.Add("--- Interval root: ---");
            ss << FormatInterval(root)
               << " w=" << scientific << setprecision(2) << root.GetWidth();
            out.Add(ss.str());
            return;
        }

        // If the interval becomes a point interval, inflate it a little
        // so the next interval operation still produces an interval enclosure.
        if (IntWidth(x) == 0.0L) {
            x = InflateInterval(x, tol);
        }
    }

    out.Add("Max iterations reached.");
    out.Add("Current interval: " + FormatInterval(x));
}


void runNewtonRaphsonFromPoint(
     long double x0,
     IntervalFn f,
     IntervalFn df,
     IntervalFn ddf,
     OutputBox& out,
     const int MAX_ITER,
     long double epsilon) 
{
    out.Clear();

    Interval<long double> x = PointInterval(x0);

    const Interval<long double> two = IntRead<long double>("2");

    for (int i = 0; i < MAX_ITER; i++) {
        Interval<long double> old = x;

        Interval<long double> fx   = f(x);
        Interval<long double> dfx  = df(x);
        Interval<long double> ddfx = ddf(x);

        if (ContainsZero(ddfx)) {
            out.Add("Error: f''(x) contains 0, cannot apply NR2.");
            out.Add("Current interval: " + FormatInterval(x));
            return;
        }

        Interval<long double> disc = dfx * dfx - two * fx * ddfx;

        if (disc.b < 0.0L) {
            out.Add("Error: discriminant is strictly negative, cannot apply NR2.");
            out.Add("disc = " + FormatInterval(disc));
            out.Add("Current interval: " + FormatInterval(x));
            return;
        }

        Interval<long double> disc_clipped(
            max(disc.a, 0.0L),
            disc.b
        );

        int st = 0;
        Interval<long double> sp = ISqrt(disc_clipped, st);

        Interval<long double> x1 = x - (dfx - sp) / ddfx;
        Interval<long double> x2 = x - (dfx + sp) / ddfx;

        long double oldMid = old.Mid();

        long double d1 = DistanceToInterval(oldMid, x1);
        long double d2 = DistanceToInterval(oldMid, x2);

        Interval<long double> xn = (d1 <= d2) ? x1 : x2;

        x = xn;

        long double step = max(
            fabsl(x.a - old.a),
            fabsl(x.b - old.b)
        );

        ostringstream ss;
        ss << "Iter " << setw(2) << i + 1
           << ": " << FormatInterval(x)
           << " w=" << scientific << setprecision(2) << x.GetWidth()
           << " step=" << step;

        out.Add(ss.str());

        if (x.GetWidth() < epsilon && step < epsilon) {
            ostringstream ss;
            out.Add("--- Interval root: ---");
            ss << FormatInterval(x)
               << " w=" << scientific << setprecision(2) << x.GetWidth();
            out.Add(ss.str());
            return;
        }

        if (x.GetWidth() == 0.0L) {
            long double r = epsilon * max(fabsl(x.Mid()), 1.0L);
            x = InflateInterval(x, r);
        }
    }

    out.Add("Max iterations reached.");
    out.Add("Current interval: " + FormatInterval(x));
}
