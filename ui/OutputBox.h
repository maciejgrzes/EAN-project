#pragma once

#include <raylib.h>
#include <string>
#include <vector>

struct OutputBox {
    std::vector<std::string> lines;
    std::vector<std::string> wrappedLines;
    Rectangle bounds;
    int scroll = 0;
    int fontSize = 20;

    void Add(const std::string& line);
    void Clear();
    void Rewrap();
    void Update();
    void Draw();

private:
    void WrapLine(const std::string& line);
};
