#include "IntervalUtils.h"
#include <dlfcn.h>
#include <cstdio>
#include <algorithm>
#include <stdexcept>
#include <string>

Interval<mpreal> ParseInterval(const string& input) {
    Interval<mpreal> I;
    string s = input;
    s.erase(remove(s.begin(), s.end(), ' '), s.end());
    if (s.empty()) throw runtime_error("no input given");

    if (s.front() == '[' && s.back() == ']') {
        s = s.substr(1, s.size() - 2);
        size_t comma = s.find(',');
        if (comma == string::npos)
            throw runtime_error("missing comma in interval");

        string left  = s.substr(0, comma);
        string right = s.substr(comma + 1);


        I.a = LeftRead<mpreal>(left);
        I.b = RightRead<mpreal>(right);

        if (I.a > I.b) throw runtime_error("lower bound is greater than upper bound");

        return I;
    } else {
        return I;
    }
}

bool loadFunctions(const string& path, RealFn& f, RealFn& df, RealFn& ddf, IntervalFn& f_iv, IntervalFn& df_iv, IntervalFn& ddf_iv) {
    void* lib = dlopen(path.c_str(), RTLD_LAZY);
    if (!lib) { printf("Error: %s\n", dlerror()); return false; }

    FnPtr pf   = (FnPtr)dlsym(lib, "f");
    FnPtr pdf  = (FnPtr)dlsym(lib, "df");
    FnPtr pddf = (FnPtr)dlsym(lib, "ddf");

    IVFnPtr pf_iv   = (IVFnPtr)dlsym(lib, "f_iv");
    IVFnPtr pdf_iv  = (IVFnPtr)dlsym(lib, "df_iv");
    IVFnPtr pddf_iv = (IVFnPtr)dlsym(lib, "ddf_iv");

    if (!pf || !pdf || !pddf || !pf_iv || !pdf_iv || !pddf_iv) {
        printf("Missing exports: %s\n", dlerror());
        dlclose(lib);
        return false;
    }

    f   = pf;
    df  = pdf;
    ddf = pddf;

    auto wrap = [](IVFnPtr fn) {
        return [fn](Interval<mpreal> x) -> Interval<mpreal> {
            long double ra, rb;
            fn((long double)x.a, (long double)x.b, &ra, &rb);
            return Interval<mpreal>(mpreal(ra), mpreal(rb));
        };
    };

    f_iv   = wrap(pf_iv);
    df_iv  = wrap(pdf_iv);
    ddf_iv = wrap(pddf_iv);

    return true;
}
