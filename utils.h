#pragma once

#include <iostream>
#include <string>

void setConsoleTextColor(int colorCode);
void printColoredText(int colorCode, std::string text);

const int DEFAULT_TEXT_COLOR = 7;