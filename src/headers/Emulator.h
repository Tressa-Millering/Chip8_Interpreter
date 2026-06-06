//
// Created by Tressa Millering on 3/31/2026.
//

/******************************************
    SFML Chip8 Interpreter - Emulator

    This class provides the framework
    for actually running the Chip8
    interpreter. Primary purpose is
    handling the SFML side of code.
    Designed such that it could be
    reworked to run a different
    interpreter/emulator without too
    much difficulty.

    Code by Tressa Millering
*******************************************/

#ifndef CHIP8_V3_EMULATOR_H
#define CHIP8_V3_EMULATOR_H

#include <vector>
#include <SFML/Graphics.hpp>
#include "SFML/Audio/Sound.hpp"
#include "SFML/Audio/SoundBuffer.hpp"

class Emulator {
private:
    std::string romName;
    const sf::Time CYCLE_TIME_C;

    //Screen Variables
    const unsigned int SCALE_C;
    sf::Color bgColor, fgColor;
    sf::Texture screenTexture;
    sf::Sprite screenSprite;
    std::vector<uint8_t> pixels, screenBuffer;
    sf::RenderWindow* window;

    const bool BORDER_C;
    sf::Texture* borderTexture;
    sf::Sprite* borderSprite;

    //Sound Variables
    sf::SoundBuffer* soundBuffer;
    sf::Sound* beep;


    //FUNCTIONS
    void borderInit();
    void windowInit();
    void soundInit(unsigned int);
    void colorInit();
    void textureInit();
    std::string trimRomName() const;
    void updateScreenTexture(const std::vector<uint8_t>&);
    void flipAt(unsigned int);
    void flipAt(unsigned int, unsigned int);
    void drawScreen() const;
    void handleEvents() const;
    void handleEvents(bool&) const;

public:
    explicit Emulator(const std::string& _romName,
                      unsigned int _scale = 25,
                      sf::Color _bgColor = sf::Color::Black,
                      sf::Color _fgColor = sf::Color::White,
                      bool _border = false,
                      unsigned int frequency = 523,
                      unsigned int cpuhz = 700);
    ~Emulator();
    int MainLoop(bool step = false);

};

#endif //CHIP8_V3_EMULATOR_H