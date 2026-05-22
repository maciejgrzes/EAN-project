#include "OutputBox.h"
#include <algorithm>

void OutputBox::Add(const std::string& line) {
    lines.push_back(line);
    WrapLine(line);

    int visibleLines = (int)(bounds.height - 16) / fontSize;
    int maxScroll = std::max(0, (int)wrappedLines.size() - visibleLines);
    scroll = maxScroll;
}

void OutputBox::Clear() {
    lines.clear();
    wrappedLines.clear();
    scroll = 0;
}

void OutputBox::WrapLine(const std::string& line) {
    int maxWidth = (int)(bounds.width - 16);

    if (MeasureText(line.c_str(), fontSize) <= maxWidth) {
        wrappedLines.push_back(line);
        return;
    }

    std::string current, word;
    for (int i = 0; i <= (int)line.size(); i++) {
        if (i == (int)line.size() || line[i] == ' ') {
            std::string test = current.empty() ? word : current + " " + word;
            if (MeasureText(test.c_str(), fontSize) > maxWidth && !current.empty()) {
                wrappedLines.push_back(current);
                current = word;
            } else {
                current = test;
            }
            word.clear();
        } else {
            word += line[i];
            if (MeasureText(word.c_str(), fontSize) > maxWidth) {
                if (!current.empty()) {
                    wrappedLines.push_back(current);
                    current.clear();
                }
                wrappedLines.push_back(word);
                word.clear();
            }
        }
    }
    if (!current.empty()) wrappedLines.push_back(current);
}

void OutputBox::Rewrap() {
    wrappedLines.clear();
    for (const std::string& line : lines)
        WrapLine(line);
}

void OutputBox::Update() {
    if (!CheckCollisionPointRec(GetMousePosition(), bounds)) return;
    scroll -= (int)GetMouseWheelMove();
    scroll = std::max(0, scroll);
    int visibleLines = (int)(bounds.height - 16) / fontSize;
    int maxScroll = std::max(0, (int)wrappedLines.size() - visibleLines);
    scroll = std::min(scroll, maxScroll);
}

void OutputBox::Draw() {
    DrawText("Output:", (int)bounds.x, (int)(bounds.y - 30), 20, WHITE);
    DrawRectangleRec(bounds, WHITE);
    DrawRectangleLinesEx(bounds, 2, GRAY);

    BeginScissorMode((int)bounds.x, (int)bounds.y,
                     (int)bounds.width, (int)bounds.height);

    for (int i = scroll; i < (int)wrappedLines.size(); i++) {
        int y = (int)(bounds.y + 8 + (i - scroll) * fontSize);
        if (y + fontSize > bounds.y + bounds.height) break;
        DrawText(wrappedLines[i].c_str(), (int)(bounds.x + 8), y, fontSize, BLACK);
    }

    EndScissorMode();
}
