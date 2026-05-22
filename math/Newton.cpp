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
        Interval<mpreal> x;
        int depth;
    };

    string FormatInterval(Interval<mpreal> x) {
        string lo, hi;
        x.IEndsToStrings(lo, hi);
        return "[" + lo + ", " + hi + "]";
    }

    void PushBisection(vector<SubInterval>& queue, Interval<mpreal> x, mpreal mid, int depth) {
        queue.push_back({ Interval<mpreal>(mid, x.b), depth + 1 });
        queue.push_back({ Interval<mpreal>(x.a, mid), depth + 1 });
    }

    void BisectWithLog(vector<SubInterval>& queue, Interval<mpreal> x, mpreal mid, int depth, const string& reason, OutputBox& out) {
        PushBisection(queue, x, mid, depth);
        out.Add("Bisect (" + reason + ") " + FormatInterval(x) + " depth=" + to_string(depth));
    }

    void StoreOrQueue(Interval<mpreal> x, vector<Interval<mpreal>>& roots, vector<SubInterval>& queue, int depth) {
        if (x.GetWidth() < mpreal("1e-15")) roots.push_back(x);
        else queue.push_back({ x, depth + 1 });
    }
}

void runNewtonRaphsonReal(long double x0, RealFn f, RealFn df, RealFn ddf, OutputBox& out, const int MAX_ITER) {
    out.Clear();
    long double x = x0;

    for (int i = 0; i < MAX_ITER; i++) {
        long double fx   = f(x);
        long double dfx  = df(x);
        long double ddfx = ddf(x);

        if (fabsl(ddfx) < 1e-20L) {
            out.Add("Error: f''(x) = 0, cannot apply NR2");
            return;
        }

        long double disc = dfx*dfx - 2.0L*fx*ddfx;

        long double xn;
        if (disc < 0.0L) {
            // Fall back to plain Newton instead of stopping
            out.Add("  disc<0 at iter " + to_string(i+1) + ", using Newton fallback");
            if (fabsl(dfx) < 1e-20L) { out.Add("Error: f'(x)=0 too"); return; }
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
           << ": x=" << fixed << setprecision(10) << (double)xn
           << "  step=" << scientific << setprecision(2) << (double)step;
        out.Add(ss.str());

        x = xn;

        if (step < 1e-15L || step / max(fabsl(x), 1.0L) < 1e-15L) {
            ostringstream root;
            root << fixed << setprecision(20) << (double)x;
            out.Add("--- Roots: ---");
            out.Add("Root: " + root.str());
            return;
        }
    }
    out.Add("Max iterations reached.");
}


void runNewtonRaphsonInterval(Interval<mpreal> x0, IntervalFn f, IntervalFn df, IntervalFn ddf, OutputBox& out, const int MAX_ITER, const int MAX_DEPTH) {
    out.Clear();
    vector<SubInterval> queue = {{ x0, 0 }};
    vector<Interval<mpreal>> roots;
    vector<Interval<mpreal>> depthLimited;

    const Interval<mpreal> two(mpreal(2), mpreal(2));

    while (!queue.empty()) {
        auto [x, depth] = queue.back();
        queue.pop_back();

        Interval<mpreal> fx = f(x);
        if (fx.a > mpreal(0) || fx.b < mpreal(0)) {
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
            mpreal mid_val = x.Mid();
            Interval<mpreal> m(mid_val, mid_val);

            Interval<mpreal> fm   = f(m);
            Interval<mpreal> dfx  = df(m);
            Interval<mpreal> ddfx = ddf(x);

            if (ContainsZero(ddfx)) {
                BisectWithLog(queue, x, mid_val, depth, "f''=0", out);
                done = true; break;
            }

            if (ContainsZero(dfx)) {
                BisectWithLog(queue, x, mid_val, depth, "f'=0", out);
                done = true; break;
            }

            Interval<mpreal> disc = dfx * dfx - two * fm * ddfx;

            if (disc.b < mpreal(0)) {
                // Fall back to plain interval Newton instead of discarding
                Interval<mpreal> xn = m - fm / dfx;
                mpreal na = max(xn.a, x.a), nb = min(xn.b, x.b);
                if (na > nb) {
                    // bisect as last resort
                    BisectWithLog(queue, x, mid_val, depth, "disc<0", out);
                    done = true; break;
                }
                x = Interval<mpreal>(na, nb);
                continue;
            }

            int st = 0;
            Interval<mpreal> disc_clipped(max(disc.a, mpreal(0)), disc.b);
            Interval<mpreal> sp = ISqrt(disc_clipped, st);

            Interval<mpreal> x1 = m - (dfx - sp) / ddfx;
            Interval<mpreal> x2 = m - (dfx + sp) / ddfx;

            mpreal a1 = max(x1.a, x.a), b1 = min(x1.b, x.b);
            mpreal a2 = max(x2.a, x.a), b2 = min(x2.b, x.b);

            bool v1 = a1 <= b1;
            bool v2 = a2 <= b2;

            if (!v1 && !v2) {
                BisectWithLog(queue, x, mid_val, depth, "candidates miss", out);
                done = true; break;
            }

            if (v1 && v2) {
                Interval<mpreal> c1(a1, b1), c2(a2, b2);
                StoreOrQueue(c1, roots, queue, depth);
                StoreOrQueue(c2, roots, queue, depth);
                out.Add("Split candidates " + FormatInterval(x) + " depth=" + to_string(depth));
                done = true; break;
            }

            Interval<mpreal> xi = v1 ? Interval<mpreal>(a1, b1) : Interval<mpreal>(a2, b2);

            ostringstream ss;
            ss << "d=" << depth << " Iter " << setw(2) << i + 1
               << ": " << FormatInterval(xi)
               << " w=" << scientific << setprecision(2) << xi.GetWidth();
            out.Add(ss.str());

            if (xi.GetWidth() < mpreal("1e-15")) {
                roots.push_back(xi);
                done = true; break;
            }
            x = xi;
        }
        if (!done) {
            mpreal mid_val = x.Mid();
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


void runNewtonRaphsonFromPoint(long double x0, RealFn f, RealFn df, RealFn ddf, IntervalFn fi, IntervalFn dfi, IntervalFn ddfi, OutputBox& out, const int MAX_ITER, const int MAX_DEPTH) {
    out.Clear();
    long double x = x0;

    for (int i = 0; i < MAX_ITER; i++) {
        long double fx   = f(x);
        long double dfx  = df(x);
        long double ddfx = ddf(x);

        if (fabsl(ddfx) < 1e-20L) {
            out.Add("f''(x) = 0, stopping phase 1");
            break;
        }

        long double disc = dfx*dfx - 2.0L*fx*ddfx;

        long double xn;
        if (disc < 0.0L) {
            // Newton fallback instead of stopping
            out.Add("  disc<0 at iter " + to_string(i+1) + ", Newton fallback");
            if (fabsl(dfx) < 1e-20L) { out.Add("f'=0 too, stopping"); break; }
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
        ss << "Iter " << setw(2) << i+1
           << ": x=" << fixed << setprecision(15) << (double)x
           << "  step=" << scientific << setprecision(2) << (double)step;
        out.Add(ss.str());

        if (step < 1e-15L || step / max(fabsl(x), 1.0L) < 1e-15L) break;
    }

    out.Add("Approximate root: x^ = " + to_string(x));

    mpreal xm(x);
    mpreal delta("1e-10");
    Interval<mpreal> bracket;
    bool found = false;

    for (int i = 0; i < 60; i++) {
        Interval<mpreal> cand(xm - delta, xm + delta);
        Interval<mpreal> fc = fi(cand);
        if (fc.a <= mpreal(0) && fc.b >= mpreal(0)) {
            bracket = cand;
            out.Add("Bracket: " + FormatInterval(cand) + "  delta=" + delta.toString(4));
            found = true;
            break;
        }
        delta *= mpreal(10);
    }

    if (!found) { out.Add("Could not bracket root."); return; }

    runNewtonRaphsonInterval(bracket, fi, dfi, ddfi, out, MAX_ITER, MAX_DEPTH);
}
