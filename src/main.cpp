#include <SFML/Graphics.hpp>
#include "headers/Emulator.h"
#include <iostream>


int main(int argc, char** argv)
{

	std::string fileName = argv[1];
	Emulator emu(fileName, 25, sf::Color::Black, sf::Color::Green);

	emu.mainLoop();

	return 0;

}
