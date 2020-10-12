#include "zoom_window.hpp"
#include <algorithm>
#include <cmath>

void ZoomWindow::LoadFile(float length) {
    x_max = std::max(x_max, length);
    y_max += 1.f;
    x_left = 0.f;
    x_right = x_max;
    y_top = 0.f;
    y_bottom = y_max;
}

void ZoomWindow::Zoom(float x, float factor) {
    float zoom_level = x_right - x_left;
    float focus = x_left + zoom_level * x;
    x_left = focus - zoom_level * factor * x;
    x_right = focus + zoom_level * factor * (1.f - x);

    x_left = std::max(x_left, 0.f);
    x_right = std::min(x_right, x_max);
}

void ZoomWindow::ToggleSingleTrack(std::optional<int> track) {
    if (y_top != 0.f || y_bottom != y_max) {
        y_top = 0.f;
        y_bottom = y_max;
    } else if (track) {
        y_top = *track;
        y_bottom = *track + 1;
    }
}

void ZoomWindow::PanLeft() {
    const float zoom_level = x_right - x_left;
    x_left = std::max(x_left - 0.1f * zoom_level, 0.f);
    x_right = x_left + zoom_level;
}

void ZoomWindow::PanRight() {
    const float zoom_level = x_right - x_left;
    x_right = std::min(x_right + 0.1f * zoom_level, x_max);
    x_left = x_right - zoom_level;
}

void ZoomWindow::PanTo(float time) {
    const float zoom_level = x_right - x_left;
    x_left = time;
    x_right = time + zoom_level;
}

float ZoomWindow::GetTime(float x) const {
    return x_left + x * (x_right - x_left);
}

float ZoomWindow::GetX(float time) const {
    if (x_left == x_right)
        return 0.f;
    return (time - x_left) / (x_right - x_left);
}

float ZoomWindow::GetY(float logic_y) const {
    if (y_top == y_bottom)
        return 0.f;
    return (logic_y - y_top) / (y_bottom - y_top);
}

int ZoomWindow::GetTrack(float y) const {
    if (y_top == y_bottom)
        return 0;
    return std::floor(y_top + y * (y_bottom - y_top));
}
