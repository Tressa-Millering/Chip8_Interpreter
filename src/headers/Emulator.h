//
// Created by Tressa Millering on 3/31/2026.
//

#ifndef CHIP8_V3_EMULATOR_H
#define CHIP8_V3_EMULATOR_H

#include <queue>
#include <vector>
#include <SFML/Graphics.hpp>

class Emulator {
private:
    const unsigned int SCALE_C;
    sf::Color bgColor, fgColor;
    sf::Texture screenTexture;
    sf::Sprite screenSprite;
    std::vector<uint8_t> pixels, screenBuffer;
    sf::RenderWindow* window = nullptr;
    std::string romName;
    std::vector<bool> prevKeys;


public:
    Emulator(const std::string&, unsigned int, sf::Color, sf::Color);
    ~Emulator();
    void updateScreen(const std::vector<uint8_t>&);
    void flipAt(unsigned int);
    void flipAt(unsigned int, unsigned int);
    void mainLoop();
    void handleEvents();


};




#endif //CHIP8_V3_EMULATOR_H