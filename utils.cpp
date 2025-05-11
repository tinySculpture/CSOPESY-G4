#include <iostream>
#include <string>
#include <windows.h>

#include "utils.h"

void setConsoleTextColor(int colorCode)
{
	// Set the console text color
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, colorCode);
}

/*
	Change the text color of the console
	Text colors:
	1 = Blue		|	2 = Green	|	3 = Aqua		|	4 = Red
	5 = Purple		|	6 = Yellow	|	7 = White		|	8 = Gray
	9 = LBlue		|	0 = Black	|	10 = LGreen		|	11 = LAqua
	12 = LPurple	|	13 = LRed	|	14 = LYellow	|	15 = LWhite
*/
void printColoredText(int colorCode, std::string text)
{
	setConsoleTextColor(colorCode);
	std::cout << text;
	setConsoleTextColor(DEFAULT_TEXT_COLOR); // Reset to default color
}