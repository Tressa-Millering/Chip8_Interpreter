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

    std::string romName;

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

    sf::Time CYCLE_TIME;

    //FUNCTIONS
    void borderInit();
    std::string trimRomName() const;
    void windowInit();
    void soundInit(unsigned int);
    void colorInit();
    void textureInit();
    void drawScreen() const;
    void updateScreen(const std::vector<uint8_t>&);
    void flipAt(unsigned int);
    void flipAt(unsigned int, unsigned int);
    void handleEvents();
    void handleEvents(bool&);

public:
    Emulator(const std::string& _romName,
                   const unsigned int _scale = 25,
                   const sf::Color _bgColor = sf::Color::Black,
                   const sf::Color _fgColor = sf::Color::White,
                   const bool _border = false,
                   const unsigned int frequency = 523,
                   const unsigned int cpuhz = 700);
    ~Emulator();
    void MainLoop(bool step = false, bool debug = false);

};

#endif //CHIP8_V3_EMULATOR_H