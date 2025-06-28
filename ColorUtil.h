#pragma once

#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

namespace ColorUtil {

    enum class Color {
        Black = 0, Blue = 1, Green = 2, Aqua = 3, Red = 4,
        Purple = 5, Yellow = 6, White = 7, Gray = 8, LightBlue = 9,
        LightGreen = 10, LightAqua = 11, LightRed = 12,
        LightPurple = 13, LightYellow = 14, LightWhite = 15
    };

    void setTextColor(Color color);
    void printColoredText(Color color, const std::string& text);
    void printPrompt();

}