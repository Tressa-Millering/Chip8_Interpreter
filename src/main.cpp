/***************************************
	SFML Chip8 Interpreter - Main

	The main function for this program
	fetches and processes all command
	line arguments, instantiates an
	Emulator object, and calls it's
	main loop.

	Code by Tressa Millering
***************************************/

#include <iostream>
#include <iomanip>
#include <SFML/Graphics.hpp>
#include "headers/Emulator.h"

constexpr double VERSION_C = 1.0001;

void PrintVersion();
void PrintHelpMessage();
void PrintThemesList();

int main(const int argc, const char** argv) {
	std::string fileName = argv[argc - 1];

	sf::Color bgColor,
			  fgColor;

	unsigned int scale = 25,
				 cpuhz = 700,
				 frequency = 523;

	bool border = false,
		 step = false;

	if (argc < 2) {
		std::cout << "\033[31mNo arguments provided. Run `chip --help` to see usage.\033[0m\n";
		return -1;
	}


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

		else if (flag == "--cpuhz" || flag == "-c"){
			try {
				const int _hz = std::stoi(input);
				if (_hz < 1)
					throw std::exception();
				cpuhz = _hz;
			}
			catch (std::exception&) {
				std::cout << "\033[31mError processing CPU speed argument: \033[33m"<< input << "\n" <<
							 "\033[31mCPU Hz argument must be positive integer.\033[0m\n";
				return -1;
			}
		}

		else if (flag == "--beep" || flag == "-f"){
			try {
				const int freq = std::stoi(input);
				if (freq < 1)
					throw std::exception();
				frequency = freq;
			}
			catch (std::exception&) {
				std::cout << "\033[31mError processing beep frequency argument: \033[33m"<< input << "\n" <<
							 "\033[31mBeep frequency argument must be positive integer.\033[0m\n";
				return -1;
			}
		}

		else if (flag == "--scale" || flag == "-s"){
			try {
				const int sc = std::stoi(input);
				if (sc < 1) { throw std::exception(); }
				scale = sc;
			}
			catch (std::exception&) {
				std::cout << "\033[31mError processing scale argument: \033[33m"<< input << "\n" <<
							 "\033[31mScale argument must be positive integer.\033[0m\n";
				return -1;
			}
		}

		else if (flag == "--theme" || flag == "-t"){
			if (input == "red") {
				fgColor = sf::Color::Red;
				bgColor = sf::Color::Black;
			}
			else if (input == "green") {
				fgColor = sf::Color::Green;
				bgColor = sf::Color::Black;
			}
			else if (input == "blue") {
				fgColor = sf::Color::Blue;
				bgColor = sf::Color::Black;
			}
			else if (input == "pink") {
				fgColor = sf::Color{255, 175, 255};
				bgColor = sf::Color::Black;
			}
			else if (input == "octo") {
				fgColor = sf::Color{255, 196, 0};
				bgColor = sf::Color{176, 94, 0};
			}
			else if (input == "pastel") {
				fgColor = sf::Color{66, 159, 196};
				bgColor = sf::Color{196, 167, 231};
			}
			else if (input == "rose") {
				fgColor = sf::Color{255, 200, 255};
				bgColor = sf::Color{66, 22, 35};
			}
			else if (input == "ghost") {
				fgColor = sf::Color{199, 199, 199};
				bgColor = sf::Color{89, 89, 89};
			}
			else if (input == "blippi") {
				fgColor = sf::Color{246, 193, 119};
				bgColor = sf::Color{156, 207, 216};
			}
			else if (input == "alien") {
				fgColor = sf::Color{185, 243, 54};
				bgColor = sf::Color{108, 45, 139};
			}
			else if (input == "river") {
				fgColor = sf::Color{0, 235, 180};
				bgColor = sf::Color{54, 133, 159};
			}
			else {
				std::cout << "\033[31mError processing theme argument: \033[33m"<< input << "\n" <<
							 "\033[31mRun `chip --help themes` to see valid themes.\033[0m\n";
				return -1;
			}
		}

		else if (flag == "--oncolor" || flag == "-o") {
			try {
				if (input.at(0) != '#')
					throw (std::runtime_error("Invalid Code. No leading \033[37m#."));

				if (input.length() != 7)
					throw(std::runtime_error("Invalid Length."));

				size_t pos;

				const unsigned char R = std::stoi(input.substr(1,2), &pos, 16);
				if (pos != 2)
					throw (std::runtime_error("Invalid Hex Digit in R component."));

				const unsigned char G = std::stoi(input.substr(3,2), &pos, 16);
				if (pos != 2)
					throw (std::runtime_error("Invalid Hex Digit in G component."));

				const unsigned char B = std::stoi(input.substr(5,2), &pos, 16);
				if (pos != 2)
					throw (std::runtime_error("Invalid Hex Digit in B component."));

				fgColor = sf::Color{R, G, B};
			}
			catch (std::exception& e) {
				std::cout << "\033[31mError processing oncolor argument: \033[33m"<< input << "\n" <<
							 "\033[31m" << e.what() <<
							 "\n\033[31mArgument must be 6 digit hex code starting with #.\n" <<
							 "For example, \033[37m#ABC123.\n" << 
							 "Note that on some systems, this will need to be in quotes.\033[0m\n"  << std::endl;
				return -1;
			}
		}

		else if (flag == "--offcolor" || flag == "-x") {
			try {
				if (input.at(0) != '#')
					throw (std::runtime_error("Invalid Code. No leading \033[37m#."));

				if (input.length() != 7)
					throw(std::runtime_error("Invalid Length."));

				size_t pos;

				const unsigned char R = std::stoi(input.substr(1,2), &pos, 16);
				if (pos != 2)
					throw (std::runtime_error("Invalid Hex Digit in R component."));

				const unsigned char G = std::stoi(input.substr(3,2), &pos, 16);
				if (pos != 2)
					throw (std::runtime_error("Invalid Hex Digit in G component."));

				const unsigned char B = std::stoi(input.substr(5,2), &pos, 16);
				if (pos != 2)
					throw (std::runtime_error("Invalid Hex Digit in B component."));

				bgColor = sf::Color{R, G, B};
			}
			catch (std::exception& e) {
				std::cout << "\033[31mError processing offcolor argument: \033[33m"<< input << "\n" <<
							 "\033[31m" << e.what() <<
							 "\n\033[31mArgument must be 6 digit hex code starting with #.\n" <<
							 "For example, \033[37m#ABC123.\n" << 
							 "Note that on some systems, this will need to be in quotes.\033[0m\n"  << std::endl;
				return -1;
			}
		}

		else if (flag == "-h" || flag == "--help") {
			if (input == "themes") {
				PrintThemesList();
				continue;
			}

			i--;
			PrintHelpMessage();
		}

		else if (flag == "-v" || flag == "--version") {
			i--;
			PrintVersion();
		}

		else if (flag == "--color-flip") {
			i--;
			sf::Color temp = bgColor;
			bgColor = fgColor;
			fgColor = temp;
		}

		else {
			std::cout << "\033[31mUnrecognized argument: \033[33m"<< flag << "\n" <<
						 "Run `chip -h` or `chip -help` for legal arguments.\033[0m\n";
			return -1;
		}
	}


	if (fileName == "-h" || fileName == "--help") {
		PrintHelpMessage();
	} else if (fileName == "themes" && std::string(argv[argc - 2]) == "-h" || std::string(argv[argc - 2]) == "--help") {
		return 0;
	}
	else if (fileName == "-v" || fileName == "--version"){
		PrintVersion();
	}
	else if (fileName.length() >= 4 && fileName.substr(fileName.length()-4, fileName.length()-1) == ".ch8") {
		Emulator emu(fileName, scale, bgColor, fgColor, border, frequency, cpuhz);
		return emu.MainLoop(step);
	}
	else {
		std::cout << "\033[31mError: \033[33m"<< fileName << " is not a Chip8 ROM.\n" <<
						 "\033[31mROM name must end in the `.ch8` suffix.\n" << 
						 "If trying to input a flag, type `chip --help` for legal options.\033[0m\n";
		return -1;
	}

	return 0;

};

void PrintVersion() {
	std::cout << "Tressa Millering's Chip8 Interpreter - Ver. " <<  std::fixed << std::setprecision(1) << VERSION_C
		  << "\nUses SFML Ver. 3.0.2\n";
}

void PrintHelpMessage() {
	std::cout << "Chip: An SFML based Chip8 Interpreter by Tressa Millering. Ver. " << std::fixed << std::setprecision(1) << VERSION_C << "\n\n" <<
						 "<> denote required arguments, [] denote optional arguments.\n"
						 "Options and arguments are case insensitive. Order of options is not enforced.\n" <<
						 "They are evaluated left-to-right, overwriting duplicates with the most recent.\n\n" <<
						 "usage: `chip [options] <path-to-rom>`\n\n" <<
						 "Legal options are as follows:\n" <<
						 "\t-h | --help       [themes]          Outputs help message detailing program usage.\n" <<
						 "\t                                    Passing argument \"themes\" will list legal themes.\n" <<
						 "\t-v | --version                      Outputs version number of interpreter.\n" <<
						 "\t-b | --border                       Enabling draws a grid between each pixel on the screen. Off by default.\n" <<
						 "\t-i | --step                         Enables step mode. Advance to next opcode by pressing space. Off by default.\n" <<
						 "\t-c | --cpuhz      <unsigned_int>    Set CPU Hz of interpreter. 700 by default.\n" <<
						 "\t-f | --beep       <unsigned_int>    Set beep frequency. 523 by default.\n" <<
						 "\t-t | --theme      <string>          Set color theme of interpreter.\n" <<
						 "\t-o | --oncolor    <#rrggbb>	    Set on-color of pixels. Expects a 6 digit hex-code preceded by a #.\n" <<
						 "\t-x | --offcolor   <#rrggbb>        Set off-color of pixels. Expects a 6 digit hex-code preceded by a #.\n" <<
						 "\t--color-flip                        Flip background and foreground colors of theme.\n" <<
						 	"\n";

}

void PrintThemesList() {
	std::cout << "The following themes can be used with the -t option.\n" <<
			     "NOTE: Text colors do not perfectly match theme colors.\n\n" <<
				 "THEME NAME  ---  BACKGROUND  ---  FOREGROUND\n" <<
				 "\033[37mRed              \e[38;2;120;120;120mBlack            \e[38;2;255;0;0mRed				\n" <<
				 "\033[37mGreen            \e[38;2;120;120;120mBlack            \e[38;2;0;255;0mGreen			\n" <<
				 "\033[37mBlue             \e[38;2;120;120;120mBlack            \e[38;2;0;0;255mBlue			\n" <<
				 "\033[37mPink             \e[38;2;120;120;120mBlack            \e[38;2;255;175;255mPink        \n" <<
				 "\033[37mOcto             \e[38;2;176;94;0mDark Orange      \e[38;2;255;196;0mLight Orange		\n" <<
				 "\033[37mPastel           \e[38;2;196;167;231mLavender         \e[38;2;66;159;196mTurquoise	\n" <<
				 "\033[37mRose             \e[38;2;96;52;65mDark Pink        \e[38;2;255;200;255mLight Pink		\n" <<
				 "\033[37mGhost            \e[38;2;120;120;120mDark Gray        \e[38;2;199;199;199mLight Gray  \n" <<
				 "\033[37mBlippi           \e[38;2;156;207;216mLight Orange     \e[38;2;246;193;119mLight Blue  \n" <<
				 "\033[37mAlien            \e[38;2;108;45;139mMagenta          \e[38;2;185;243;54mLime			\n" <<
				 "\033[37mRiver            \e[38;2;54;133;159mAqua          \e[38;2;0;235;180mTurquoise			\n" <<
				 	"\n";
}
