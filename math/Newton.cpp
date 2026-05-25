#include "Newton.h"
#include "IntervalUtils.h"
#include <ios>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>

using namespace std;

namespace {
    struct SubInterval {
        Interval<long double> x;
        int depth;
    };

    string FormatInterval(Interval<long double> x) {
        string lo, hi;
        x.IEndsToStrings(lo, hi);
        return "[" + lo + ", " + hi + "]";
    }

    void PushBisection(vector<SubInterval>& queue, Interval<long double> x, long double mid, int depth) {
        queue.push_back({ Interval<long double>(mid, x.b), depth + 1 });
        queue.push_back({ Interval<long double>(x.a, mid), depth + 1 });
    }

    void BisectWithLog(vector<SubInterval>& queue, Interval<long double> x, long double mid, int depth, const string& reason, OutputBox& out) {
        PushBisection(queue, x, mid, depth);
        out.Add("Bisect (" + reason + ") " + FormatInterval(x) + " depth=" + to_string(depth));
    }

    void StoreOrQueue(Interval<long double> x, vector<Interval<long double>>& roots, vector<SubInterval>& queue, int depth, long double eps) {
        if (x.GetWidth() < eps) roots.push_back(x);
        else queue.push_back({ x, depth + 1 });
    }
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


void runNewtonRaphsonInterval(Interval<long double> x0, 
                              IntervalFn f, 
                              IntervalFn df, 
                              IntervalFn ddf, 
                              OutputBox& out, 
                              const int MAX_ITER, 
                              const int MAX_DEPTH,
                              long double epsilon) 
{
    out.Clear();
    vector<SubInterval> queue = {{ x0, 0 }};
    vector<Interval<long double>> roots;
    vector<Interval<long double>> depthLimited;

    const Interval<long double> two = IntRead<long double>("2");

    while (!queue.empty()) {
        auto [x, depth] = queue.back();
        queue.pop_back();

        Interval<long double> fx = f(x);
        if (fx.a > 0.0L || fx.b < 0.0L) {
            out.Add("Skip " + FormatInterval(x) + ": f doesn't contain 0");
            continue;
        }

        if (depth > MAX_DEPTH) {
            out.Add("Max bisection depth at " + FormatInterval(x));
            depthLimited.push_back(x);
            continue;
        }

        bool done = false;
        for (int i = 0; i < MAX_ITER; i++) {
            long double mid_val = x.Mid();
            Interval<long double> m(mid_val, mid_val);

            Interval<long double> fm   = f(m);
            Interval<long double> dfm  = df(m);
            Interval<long double> ddfx = ddf(x);

            if (ContainsZero(ddfx)) {
                BisectWithLog(queue, x, mid_val, depth, "f''=0", out);
                done = true; break;
            }

            if (ContainsZero(dfm)) {
                BisectWithLog(queue, x, mid_val, depth, "f'=0", out);
                done = true; break;
            }

            Interval<long double> disc = dfm * dfm - two * fm * ddfx;

            if (disc.b < 0.0L) {
                Interval<long double> xn = m - fm / dfm;
                long double na = max(xn.a, x.a), nb = min(xn.b, x.b);
                if (na > nb) {
                    BisectWithLog(queue, x, mid_val, depth, "disc<0", out);
                    done = true; break;
                }
                x.a = na;
                x.b = nb;
                continue;
            }

            int st = 0;
            Interval<long double> disc_clipped(max(disc.a, 0.0L), disc.b);
            Interval<long double> sp = ISqrt(disc_clipped, st);

            Interval<long double> x1 = m - (dfm - sp) / ddfx;
            Interval<long double> x2 = m - (dfm + sp) / ddfx;

            long double a1 = max(x1.a, x.a), b1 = min(x1.b, x.b);
            long double a2 = max(x2.a, x.a), b2 = min(x2.b, x.b);

            bool v1 = a1 <= b1;
            bool v2 = a2 <= b2;

            if (!v1 && !v2) {
                BisectWithLog(queue, x, mid_val, depth, "candidates miss", out);
                done = true; break;
            }

            if (v1 && v2) {
                Interval<long double> c1(a1, b1), c2(a2, b2);
                StoreOrQueue(c1, roots, queue, depth, epsilon);
                StoreOrQueue(c2, roots, queue, depth, epsilon);
                out.Add("Split candidates " + FormatInterval(x) + " depth=" + to_string(depth));
                done = true; break;
            }

            Interval<long double> xi = v1 ? Interval<long double>(a1, b1) : Interval<long double>(a2, b2);

            ostringstream ss;
            ss << "d=" << depth << " Iter " << setw(2) << i + 1
               << ": " << FormatInterval(xi)
               << " w=" << scientific << setprecision(2) << xi.GetWidth();
            out.Add(ss.str());

            if (xi.GetWidth() < epsilon) {
                roots.push_back(xi);
                done = true; break;
            }
            x = xi;
        }
        if (!done) {
            long double mid_val = x.Mid();
            BisectWithLog(queue, x, mid_val, depth, "max iter", out);
        }
    }

    out.Add("--- Roots: " + to_string(roots.size()) + " ---");
    for (auto& r : roots) {
        out.Add("Root: " + FormatInterval(r));
    }

    out.Add("--- Depth limited intervals: " + to_string(depthLimited.size()) + " ---");
    for (auto& r : depthLimited) {
        out.Add("Depth-limited candidate: " + FormatInterval(r));
    }
}


void runNewtonRaphsonFromPoint(long double x0, 
                               RealFn f, 
                               RealFn df, 
                               RealFn ddf, 
                               IntervalFn fi, 
                               IntervalFn dfi, 
                               IntervalFn ddfi, 
                               OutputBox& out, 
                               const int MAX_ITER, 
                               const int MAX_DEPTH,
                               long double epsilon) 
{
    out.Clear();
    long double x = x0;

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
        out.Add("  disc<0, using Newton fallback");
        if (fabsl(dfx) < epsilon) { out.Add("Error: f'(x)=0 too"); return; }
        xn = x - fx / dfx;
    } else {
        long double sp = sqrtl(disc);
        long double x1 = x - (dfx - sp) / ddfx;
        long double x2 = x - (dfx + sp) / ddfx;
        xn = (fabsl(x2 - x) > fabsl(x1 - x)) ? x1 : x2;
    }

    long double step = fabsl(xn - x);

    x = xn;

    ostringstream ss;
    ss << ": x=" << fixed << setprecision(15) << x
       << "  step=" << scientific << setprecision(2) << step;
    out.Add(ss.str());
    out.Add("Approximate root: x^ = " + to_string(x));

    long double xm(x);
    long double delta = epsilon;
    Interval<long double> bracket;
    bool found = false;

    for (int i = 0; i < 60; i++) {
        Interval<long double> cand(xm - delta, xm + delta);
        Interval<long double> fc = fi(cand);
        if (fc.a <= 0.0L && fc.b >= 0.0L) {
            bracket = cand;
            out.Add("Bracket: " + FormatInterval(cand) + "  delta=" + to_string(delta));
            found = true;
            break;
        }
        delta *= 10.0L;
    }

    if (!found) { out.Add("Could not bracket root."); return; }

    runNewtonRaphsonInterval(bracket, fi, dfi, ddfi, out, MAX_ITER, MAX_DEPTH, epsilon);
}
