#include "common/Types.h"
#include "math/IntervalUtils.h"
#include "math/Newton.h"
#include "ui/Button.h"
#include "ui/InputBox.h"
#include "ui/OutputBox.h"

#include <iostream>
#include <raylib.h>
#include <string>
using namespace std;

const int SCREEN_W = 1300;
const int SCREEN_H = 700;

int main(int argc, char* argv[]) {
    InitWindow(SCREEN_W, SCREEN_H, "Solving nonlinear equations");
    SetTargetFPS(60);

    Button normal     = {{ 10, 100, 100, 50 }, "Real numbers", true};
    Button IVreal     = {{ 10, 200, 100, 50 }, "Interval for real numbers",false};
    Button IVinterval = {{ 10, 300, 100, 50 }, "Interval for interval numbers",false};

    InputBox  input  = {{ 400, 100, 300, 40 }, "", false, 32 };
    OutputBox output = {{ }, { }, { 400, 200, 800, 400 }};

    enum class Mode { Real, IntervalFromReal, IntervalFromInterval };
    Mode currentMode = Mode::Real;

    RealFn f, df, ddf;
    IntervalFn f_iv, df_iv, ddf_iv;

    string soPath = "./build/funcs/func.so";
    if (argc > 1) soPath = argv[1];

    if (!loadFunctions(soPath, f, df, ddf, f_iv, df_iv, ddf_iv)) {
        output.Add("ERROR: cannot load: " + soPath);
        soPath = "./build/funcs/func.so";
        output.Add("Falling back to: " + soPath);
        if (!loadFunctions(soPath, f, df, ddf, f_iv, df_iv, ddf_iv)) {
            cerr << "ERROR: cannot load: " << soPath << endl;
            cerr << "Make sure " << soPath << " is built before executing the program" << endl;
            return -1;
        }
    }
    else
        output.Add("Loaded: " + soPath);

    while (!WindowShouldClose()) {
        ///////////
        // Logic //
        ///////////
        input.Update();
        output.Update();

        if (normal.IsClicked()) {
            normal.isOn = true; IVreal.isOn = false; IVinterval.isOn = false;
            currentMode = Mode::Real;
            output.Clear();
        }
        if (IVreal.IsClicked()) {
            normal.isOn = false; IVreal.isOn = true; IVinterval.isOn = false;
            currentMode = Mode::IntervalFromReal;
            output.Clear();
        }
        if (IVinterval.IsClicked()) {
            normal.isOn = false; IVreal.isOn = false; IVinterval.isOn = true;
            currentMode = Mode::IntervalFromInterval;
            output.Clear();
        }

        string pushed = input.PushValue();
        if (!pushed.empty()) {
            try {
                if (currentMode == Mode::Real) {
                    long double x0 = stold(pushed);
                    runNewtonRaphsonReal(x0, f, df, ddf, output, MAX_ITER);
                }
                else if (currentMode == Mode::IntervalFromReal) {
                    long double x0 = stold(pushed);
                    runNewtonRaphsonFromPoint(x0, f, df, ddf, f_iv, df_iv, ddf_iv, output, MAX_ITER, MAX_DEPTH);
                }
                else {
                    Interval<mpreal> x0 = ParseInterval(pushed);
                    runNewtonRaphsonInterval(x0, f_iv, df_iv, ddf_iv, output, MAX_ITER, MAX_DEPTH);
                }
            } catch (...) {
                output.Clear();
                output.Add("Error: wrong input format");
            }
        }

        /////////////
        // Drawing //
        /////////////
        BeginDrawing();
        ClearBackground(BLACK);

        normal.Draw();
        IVreal.Draw();
        IVinterval.Draw();

        switch (currentMode) {
            case Mode::Real:                 input.hint = "1.5";    break;
            case Mode::IntervalFromReal:     input.hint = "1.5";    break;
            case Mode::IntervalFromInterval: input.hint = "[1, 2]"; break;
        }
        input.Draw();
        output.Draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
