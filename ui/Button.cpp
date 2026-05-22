#include "Button.h"

bool Button::IsClicked() const {
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, bounds);
    return hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void Button::Draw() const {
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, bounds);

    Color color = isOn ? LIGHTGRAY : GRAY;
    DrawRectangleRec(bounds, color);
    DrawRectangleLinesEx(bounds, 2, WHITE);

    int fontSize = 20;
    DrawText(label.c_str(),
        bounds.x,
        bounds.y - fontSize - 5,
        fontSize, WHITE);
}
