#include <iostream>
#include <cstring>
#include <SFML/Graphics.hpp>
#include "headers/Emulator.h"



int main(const int argc, const char** argv)
{
	sf::Color bgColor, fgColor;
	unsigned int scale = 25, cpuhz = 700, frequency = 523;
	bool border = false, step = false, debug = false;


	std::cout << argc-2 << std::endl;
	for (int i = 1; i < argc-1; i+=2) {
		std::string flag = argv[i];
		std::string input = argv[i+1];
		for (char& c : flag) { c = tolower(c); }
		for (char& c : input) { c = tolower(c); }

		if (flag == "--border" || flag == "-b") {
			i--;
			border = true;
		}

		else if (flag == "--step" || flag == "-i") {
			i--;
			step = true;
		}

		else if (flag == "--debug" || flag == "-d") {
			i--;
			debug = true;
		}

		else if (flag == "--cpuhz" || flag == "-h"){
			try {
				const int _hz = std::stoi(input);
				if (_hz < 1) { throw std::exception(); }
				cpuhz = _hz;
			} catch (std::exception&) {
				std::cout << "\033[31mError processing CPU speed argument: \033[33m"<< input << "\n" <<
							 "\033[31mcpuhz argument must be positive integer.\n" << std::endl;
				return -1;
			}
		}

		else if (flag == "--beep" || flag == "-f"){
			try {
				const int freq = std::stoi(input);
				if (freq < 1) { throw std::exception(); }
				frequency = freq;
			} catch (std::exception) {
				std::cout << "\033[31mError processing beep frequency argument: \033[33m"<< input << "\n" <<
							 "\033[31mBeep frequency argument must be positive integer.\n" << std::endl;
				return -1;
			}
		}

		else if (flag == "--scale" || flag == "-s"){
			try {
				const int sc = std::stoi(input);
				if (sc < 1) { throw std::exception(); }
				scale = sc;
			} catch (std::exception) {
				std::cout << "\033[31mError processing scale argument: \033[33m"<< input << "\n" <<
							 "\033[31mScale argument must be positive integer.\n" << std::endl;
				return -1;
			}
		}

		else if (flag == "--theme" || flag == "-t"){
			if (input == "red") {
				fgColor = sf::Color::Red;
				bgColor = sf::Color::Black;
			} else if (input == "green") {
				fgColor = sf::Color::Green;
				bgColor = sf::Color::Black;
			} else if (input == "blue") {
				fgColor = sf::Color::Blue;
				bgColor = sf::Color::Black;
			} else if (input == "pink") {
				fgColor = sf::Color{255, 175, 255};
				bgColor = sf::Color::Black;
			} else if (input == "octo") {
				fgColor = sf::Color{255, 196, 0};
				bgColor = sf::Color{176, 94, 0};
			} else if (input == "pastel") {
				fgColor = sf::Color{66, 159, 196};
				bgColor = sf::Color{196, 167, 231};
			} else if (input == "rose") {
				fgColor = sf::Color{255, 200, 255};
				bgColor = sf::Color{66, 22, 35};
			} else if (input == "ghost") {
				fgColor = sf::Color{199, 199, 199};
				bgColor = sf::Color{89, 89, 89};
			} else if (input == "blippi") {
				fgColor = sf::Color{246, 193, 119};
				bgColor = sf::Color{156, 207, 216};
			} else if (input == "alien") {
				fgColor = sf::Color{185, 243, 54};
				bgColor = sf::Color{108, 45, 139};
			} else if (input == "river") {
				fgColor = sf::Color{0, 235, 180};
				bgColor = sf::Color{54, 133, 159};
			} else {
				std::cout << "\033[31mError processing theme argument: \033[33m"<< input << "\n" <<
							 "\033[31mRun `chip help themes` to see valid themes.\n" << std::endl;
				return -1;
			}
		}

		else if (flag == "--oncolor" || flag == "-on") {
			try {
				if (input.at(0) != '#') throw (std::runtime_error("Invalid Code. No leading \033[37m#."));

				if (input.length() != 7) throw(std::runtime_error("Invalid Length."));

				size_t pos;

				const unsigned char R = std::stoi(input.substr(1,2), &pos, 16);
				if (pos != 2) throw (std::runtime_error("Invalid Hex Digit in R component."));

				const unsigned char G = std::stoi(input.substr(3,2), &pos, 16);
				if (pos != 2) throw (std::runtime_error("Invalid Hex Digit in G component."));

				const unsigned char B = std::stoi(input.substr(5,2), &pos, 16);
				if (pos != 2) throw (std::runtime_error("Invalid Hex Digit in B component."));

				fgColor = sf::Color{R, G, B};
			} catch (std::exception& e) {
				std::cout << "\033[31mError processing oncolor argument: \033[33m"<< input << "\n" <<
							 "\033[31m" << e.what() << "\n\033[31mArgument must be 6 digit hex code starting with #.\n" <<
								 "For example, \033[37m#ABC123." << std::endl;
				return -1;
			}
		}

		else if (flag == "--offcolor" || flag == "-off") {
			try {
				if (input.at(0) != '#') throw (std::runtime_error("Invalid Code. No leading \033[37m#."));

				if (input.length() != 7) throw(std::runtime_error("Invalid Length."));

				size_t pos;

				const unsigned char R = std::stoi(input.substr(1,2), &pos, 16);
				if (pos != 2) throw (std::runtime_error("Invalid Hex Digit in R component."));

				const unsigned char G = std::stoi(input.substr(3,2), &pos, 16);
				if (pos != 2) throw (std::runtime_error("Invalid Hex Digit in G component."));

				const unsigned char B = std::stoi(input.substr(5,2), &pos, 16);
				if (pos != 2) throw (std::runtime_error("Invalid Hex Digit in B component."));

				bgColor = sf::Color{R, G, B};
			} catch (std::exception& e) {
				std::cout << "\033[31mError processing offcolor argument: \033[33m"<< input << "\n" <<
							 "\033[31m" << e.what() << "\n\033[31mArgument must be 6 digit hex code starting with #.\n" <<
								 "For example, \033[37m#ABC123." << std::endl;
				return -1;
			}
		}

		else if (flag == "-h" || flag == "-help") {
			i--;
			std::cout << "OUTPUT HELP MESSAGE";
		}

		else {
			std::cout << "\033[31mUnrecognized argument: \033[33m"<< flag << "\n" <<
						 "Run `chip -h` or `chip -help` for legal arguments." << std::endl;
		}

	}
	std::string fileName = argv[argc - 1];
	Emulator emu(fileName, scale, bgColor, fgColor, cpuhz, border, frequency);

	emu.MainLoop(step, debug);

	return 0;

};
