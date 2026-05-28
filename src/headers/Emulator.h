//
// Created by Tressa Millering on 3/31/2026.
//

#ifndef CHIP8_V3_EMULATOR_H
#define CHIP8_V3_EMULATOR_H


#include <vector>
#include <SFML/Graphics.hpp>

#include "SFML/Audio/Sound.hpp"
#include "SFML/Audio/SoundBuffer.hpp"

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
    const unsigned int CPU_HZ_C;
    const unsigned int BEEP_FREQ_C;
    sf::SoundBuffer* soundBuffer;
    sf::Sound* beep;


public:
    Emulator(const std::string& _romName,
                   const unsigned int _scale = 25,
                   const sf::Color _bgColor = sf::Color::Black,
                   const sf::Color _fgColor = sf::Color::White,
                   const unsigned int _cpuhz = 700,
                   const unsigned int _frequency = 523);
    ~Emulator();
    void updateScreen(const std::vector<uint8_t>&);
    void flipAt(unsigned int);
    void flipAt(unsigned int, unsigned int);
    void mainLoop();
    void handleEvents();



};




#endif //CHIP8_V3_EMULATOR_H