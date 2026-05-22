#pragma once

#include <raylib.h>
#include <string>

struct InputBox {
    Rectangle bounds;
    std::string value;
    bool focused = false;
    int maxLength;
    std::string hint;

    float backspaceTimer = 0.0f;
    const float backspaceDelay  = 0.5f;
    const float backspaceRepeat = 0.05f;

    std::string PushValue();
    void Update();
    void Draw() const;
};
