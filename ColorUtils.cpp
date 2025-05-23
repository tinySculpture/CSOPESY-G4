#include "ColorUtils.h"
#include <iostream>
#include <windows.h>

namespace ColorUtils {

    /**
     * @enum Color
     * @brief Defines standard console text colors for Windows consoles.
     *
     * Values are based on Windows console color codes used by SetConsoleTextAttribute.
     * For example, `Color::Red` corresponds to `4`, `Color::White` to `15`, etc.
     * These values are typically defined in the corresponding `ColorUtils.h` enum.
     */

     /**
      * @brief Sets the console text color for future `std::cout` outputs.
      *
      * This is a Windows-specific implementation that uses the Win32 API to modify
      * the text color of the console.
      *
      * @param color The `ColorUtils::Color` enum value representing the desired text color.
      */
    void setTextColor(Color color) {
#ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, static_cast<int>(color));
#else
        // No-op on non-Windows platforms; override as needed for portability
        (void)color;
#endif
    }

    /**
     * @brief Prints the given text to the console in a specific color.
     *
     * Automatically resets the text color back to white after printing to avoid
     * affecting subsequent output.
     *
     * @param color The color to apply to the text.
     * @param text The string to be printed to the console.
     */
    void printColoredText(Color color, const std::string& text) {
        setTextColor(color);
        std::cout << text;
        setTextColor(Color::White); // Reset to default after printing
    }

    /**
     * @brief Prints a stylized CLI prompt to the console.
     *
     * The prompt includes:
     * - "G4@csopesy" in purple
     * - ":" as a separator
     * - "~" in blue
     * - "$ " as the command entry symbol
     *
     * This provides a visually distinct and personalized shell prompt.
     */
    void printPrompt() {
        printColoredText(Color::Purple, "\nG4@csopesy");
        std::cout << ":";
        printColoredText(Color::Blue, "~");
        std::cout << "$ ";
    }

}
