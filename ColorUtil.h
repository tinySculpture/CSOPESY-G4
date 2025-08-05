#pragma once

#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

/**
 * @file ColorUtil.h
 * @brief Utility functions and types for colored console output.
 */
namespace ColorUtil {

    /**
     * @enum Color
     * @brief Represents available console text colors.
     *
     * Enumerates standard and light variants of common console colors.
     */
    enum class Color {
        Black = 0, Blue = 1, Green = 2, Aqua = 3, Red = 4,
        Purple = 5, Yellow = 6, White = 7, Gray = 8, LightBlue = 9,
        LightGreen = 10, LightAqua = 11, LightRed = 12,
        LightPurple = 13, LightYellow = 14, LightWhite = 15
    };

    /**
     * @brief Sets the console text color.
     * 
     * Changes the color of subsequent text output to the specified color.
     * 
     * @param color The color to set for console text.
     */
    void setTextColor(Color color);

    /**
     * @brief Prints text in the specified color.
     * 
     * Sets the console text color, prints the given text, and then resets the color.
     * 
     * @param color The color to use for the text.
     * @param text The text string to print.
     */
    void printColoredText(Color color, const std::string& text);

    /**
     * @brief Prints a colored prompt to the console.
     * 
     * Typically used to display a prompt for user input with a distinct color.
     */
    void printPrompt();

}