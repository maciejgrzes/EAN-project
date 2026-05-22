#pragma once

#include <raylib.h>
#include <string>

struct Button {
    Rectangle bounds;
    std::string label;
    bool isOn;

    bool IsClicked() const;
    void Draw() const;
};
