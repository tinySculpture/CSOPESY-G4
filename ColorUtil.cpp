#include <iostream>
#include <windows.h>

#include "ColorUtil.h"

/**
 * @namespace ColorUtil
 * @brief Utility functions for handling colored console output.
 *
 * This namespace provides functions to set the text color in the console,
 * print colored text, and display a colored command prompt.
 */

    /**
     * @brief Sets the text color for console output.
     * 
     * On Windows systems, this function changes the console text color using
     * the Windows API. On other platforms, this function does nothing.
     *
     * @param color The color to set for the console text.
     */
    
    /**
     * @brief Prints text in the specified color to the console.
     *
     * This function sets the console text color, prints the given text,
     * and then resets the color to white.
     *
     * @param color The color to use for the text.
     * @param text The string to print in the specified color.
     */
    
    /**
     * @brief Prints a colored command prompt to the console.
     *
     * The prompt consists of a purple "G4@csopesy", a colon, a blue "~", and a dollar sign,
     * each with appropriate coloring.
     */
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
