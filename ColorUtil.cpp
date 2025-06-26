#include <iostream>
#include <windows.h>

#include "ColorUtil.h"

namespace ColorUtil {

    void setTextColor(Color color) {
        #ifdef _WIN32
                HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
                SetConsoleTextAttribute(hConsole, static_cast<int>(color));
        #else
                (void)color;
        #endif
    }

    void printColoredText(Color color, const std::string& text) {
        setTextColor(color);
        std::cout << text;
        setTextColor(Color::White);
    }

    void printPrompt() {
        printColoredText(Color::Purple, "\nG4@csopesy");
        std::cout << ":";
        printColoredText(Color::Blue, "~");
        std::cout << "$ ";
    }
}
