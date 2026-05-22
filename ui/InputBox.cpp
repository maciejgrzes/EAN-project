#include "InputBox.h"

std::string InputBox::PushValue() {
    if (IsKeyPressed(KEY_ENTER) && !value.empty()) {
        std::string result = value;
        value = "";
        return result;
    }
    return "";
}

void InputBox::Update() {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        focused = CheckCollisionPointRec(GetMousePosition(), bounds);

    if (!focused) return;

    int ch;
    while ((ch = GetCharPressed()) != 0) {
        if ((int)value.size() < maxLength)
            value += (char)ch;
    }

    if (IsKeyDown(KEY_BACKSPACE) && !value.empty()) {
        backspaceTimer -= GetFrameTime();
        if (IsKeyPressed(KEY_BACKSPACE) || backspaceTimer <= 0.0f) {
            value.pop_back();
            backspaceTimer = IsKeyPressed(KEY_BACKSPACE) ? backspaceDelay : backspaceRepeat;
        }
    } else {
        backspaceTimer = 0.0f;
    }
}

void InputBox::Draw(std::string name) const {
    DrawText(name.c_str(), bounds.x, bounds.y - 30, 20, WHITE);
    DrawRectangleRec(bounds, RAYWHITE);
    DrawRectangleLinesEx(bounds, 2, focused ? BLUE : GRAY);

    int fontSize = 20;
    DrawText(value.c_str(),
        bounds.x + 8,
        bounds.y + (bounds.height - fontSize) / 2,
        fontSize, BLACK);

    if (focused && ((int)(GetTime() * 2) % 2 == 0)) {
        int textWidth = MeasureText(value.c_str(), fontSize);
        DrawText("|",
            bounds.x + 8 + textWidth,
            bounds.y + (bounds.height - fontSize) / 2,
            fontSize, BLACK);
    }
}
