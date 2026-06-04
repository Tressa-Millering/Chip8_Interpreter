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


#include "../headers/Emulator.h"
#include "../headers/Chip8.h"
#include <SFML/Audio.hpp>
#include <iostream>
#include <cmath>

//These values never change, and so don't need to be members
constexpr unsigned int WIDTH_C = 64;
constexpr unsigned int HEIGHT_C = 32;
constexpr unsigned int TIMER_HZ_C = 60;
constexpr sf::Time FRAME_TIME_C = sf::seconds(1.f / TIMER_HZ_C);


Emulator::Emulator(const std::string& _romName,
                   const unsigned int _scale,
                   const sf::Color _bgColor,
                   const sf::Color _fgColor,
                   const bool _border,
                   const unsigned int frequency,
                   const unsigned int cpuhz) :

    romName(_romName),
    CYCLE_TIME_C(sf::seconds(1.f / cpuhz)),
    SCALE_C(_scale),
    bgColor(_bgColor),
    fgColor(_fgColor),
    screenTexture({WIDTH_C, HEIGHT_C}),
    screenSprite(screenTexture),
    BORDER_C(_border) {

    colorInit();
    soundInit(frequency);
    textureInit();
}

Emulator::~Emulator() {
    delete window;
    delete beep;
    delete soundBuffer;
    delete borderTexture;
    delete borderSprite;
}

void Emulator::updateScreenTexture(const std::vector<uint8_t>& buffer) {
    /* *
     * Updates screenTexture member with data from buffer
     *
     * @param buffer: vector<uint8_t>& holding screen data to
     * update screenTexture with.
     * */
    unsigned int size = buffer.size();
    if (size > HEIGHT_C*WIDTH_C)
        size = HEIGHT_C*WIDTH_C;

    for (int i = 0; i < size; i++) {
            screenBuffer.at(i) = (buffer.at(i));
            pixels.at(4*i+0) = (buffer.at(i)) ? fgColor.r : bgColor.r;
            pixels.at(4*i+1) = (buffer.at(i)) ? fgColor.g : bgColor.g;
            pixels.at(4*i+2) = (buffer.at(i)) ? fgColor.b : bgColor.b;
            pixels.at(4*i+3) = 255;
    }
    screenTexture.update(pixels.data());
}

void Emulator::flipAt(const unsigned int i) {
    /* *
    * XOR's the pixel at position i in screenTexture
    *
    * @param i: the position to flip
    * */
    sf::Color drawColor;
    try {
        if (screenBuffer.at(i))
            drawColor = bgColor;
        else
            drawColor = fgColor;

        screenBuffer.at(i) = !screenBuffer.at(i);

    } catch (std::out_of_range& e) {
        std::cout << "Error: " << e.what() << std::endl;
        return;
    }

    pixels.at(4*i+0) = drawColor.r;
    pixels.at(4*i+1) = drawColor.g;
    pixels.at(4*i+2) = drawColor.b;
    pixels.at(4*i+3) = drawColor.a;

    screenTexture.update(pixels.data());
}

void Emulator::flipAt(const unsigned int x, const unsigned int y) {
    /* *
    * XOR's the pixel at position (x, y) in screenTexture
    *
    * @param x: the x coordinate to flip
    * @param y: the y coordinate to flip
    * */
    const unsigned int clippedY = (y > 31 ? 31 : y);
    const unsigned int i = (x + WIDTH_C * clippedY);
    sf::Color drawColor;
    try {
        if (screenBuffer.at(i))
            drawColor = bgColor;
        else
            drawColor = fgColor;

        screenBuffer.at(i) = !screenBuffer.at(i);

    } catch (std::out_of_range& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return;
    }

    pixels.at(4*i+0) = drawColor.r;
    pixels.at(4*i+1) = drawColor.g;
    pixels.at(4*i+2) = drawColor.b;
    pixels.at(4*i+3) = drawColor.a;

    screenTexture.update(pixels.data());
}

void Emulator::MainLoop(const bool step) {
    /* *
     * Main emulator loop for Chip8.
     *
     * @param step: determines if stepper mode is enabled
     * */
    if (BORDER_C)
        borderInit();

    windowInit();

    Chip8 chip8(romName);

    sf::Clock clock;
    sf::Time frameTimer = sf::Time::Zero;
    sf::Time cycleTimer = sf::Time::Zero;

    bool stepRequested = false;

    while(window->isOpen()) {
        const sf::Time delta = clock.restart();
        frameTimer += delta;
        cycleTimer += delta;

        handleEvents(stepRequested);
        chip8.UpdateKeystate();

        if (!step) {
            while (cycleTimer >= CYCLE_TIME_C) {
                chip8.Cycle();
                cycleTimer -= CYCLE_TIME_C;
            }
        }
        else if (stepRequested) {
            chip8.Cycle();
            stepRequested = false;
        }

        while (frameTimer >= FRAME_TIME_C) {
            chip8.TickTimers();

            if (chip8.GetSoundTimer() > 0 && beep->getStatus() != sf::Sound::Status::Playing)
                beep->play();
            else if (chip8.GetSoundTimer() <= 0)
                beep->pause();

            frameTimer -= FRAME_TIME_C;
        }

        if (chip8.GetScreenChange()){
            updateScreenTexture(chip8.GetScreen());
            chip8.SetScreenChange(false);
        }

        drawScreen();
    }
}

void Emulator::handleEvents() const {
    /* *
     * Handles close event for main window
     * */
    while (const auto event = window->pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window->close();
        }
    }
}

void Emulator::handleEvents(bool& stepReq) const {
    /* *
     * Handles close event and step request for main window
     * */
    while (const auto event = window->pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window->close();
        }
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()){
            if (keyPressed->code == sf::Keyboard::Key::Space) {
                stepReq = true;
            }
        }
    }
}

void Emulator::borderInit() {
    /* *
     * Initializes member variables for pixel border
     * */
    std::vector<uint8_t> borderData(4*HEIGHT_C*WIDTH_C*SCALE_C*SCALE_C,0);

    for (int i = 0; i < borderData.size()/4; i++) {
        if (i % SCALE_C == 0 || (((i / (SCALE_C*WIDTH_C)) % SCALE_C) == 0) ||
            i % SCALE_C == 1 || (((i / (SCALE_C*WIDTH_C)) % SCALE_C) == 1) ||
            i % SCALE_C == 2 || (((i / (SCALE_C*WIDTH_C)) % SCALE_C) == 2))
        {
            borderData.at(4*i) = bgColor.r;
            borderData.at(4*i+1) = bgColor.g;
            borderData.at(4*i+2) = bgColor.b;
            borderData.at(4*i+3) = 255;
        }
    }

    borderTexture = new sf::Texture({WIDTH_C*SCALE_C, HEIGHT_C*SCALE_C});
    borderTexture->update(borderData.data());
    borderSprite = new sf::Sprite(*borderTexture);
}

std::string Emulator::trimRomName() const {
    /* *
     * Trims rom name for window title.
     * Given the format '/dir/dir2/Name.ch8',
     * it would return 'Name'.
     *
     * @return: trimmed rom title
     * */
    const size_t start = romName.rfind('/');
    const size_t end = romName.rfind('.');
    return romName.substr(start+1, end - start - 1);
}

void Emulator::windowInit() {
    /* *
     * Initializes main window.
     * (It's an ugly line, this is just to keep code readable).
     * */
    window = new sf::RenderWindow( sf::VideoMode( { WIDTH_C*SCALE_C, HEIGHT_C*SCALE_C} ), "Chip8 - " + trimRomName());
}

void Emulator::soundInit(const unsigned int frequency) {
    /* *
     * Initializes sound member variables.
     *
     * @param frequency: desired frequency of beep
     * */
    constexpr int SAMPLE_RATE = 44100;
    std::vector<int16_t> samples(SAMPLE_RATE);

    for (int i = 0; i < SAMPLE_RATE; i++) {
        constexpr int16_t max = 32767; //max amplitude of sound for scaling
        samples.at(i) = static_cast<int16_t>(max * sin(2* M_PI * frequency * i/SAMPLE_RATE));
    }

    const std::vector<sf::SoundChannel> channelMap = { sf::SoundChannel::Mono };
    soundBuffer = new sf::SoundBuffer(samples.data(), samples.size(), channelMap.size(), SAMPLE_RATE, channelMap);
    beep = new sf::Sound(*soundBuffer);
    beep->setLooping(true);
}

void Emulator::colorInit() {
    /* *
     * If the same color is given for BG and FG,
     * changes one to a different color.
     * */
    if (bgColor == fgColor){
        if (fgColor != sf::Color::Black) {
            bgColor = sf::Color::Black;
        } else {
            fgColor = sf::Color::White;
        }
    }
}

void Emulator::textureInit() {
    /* *
     * Initializes texture member variables.
     * */
    for (int i = 0; i < HEIGHT_C*WIDTH_C; i++) {
        screenBuffer.push_back(0);
        pixels.push_back(bgColor.r);
        pixels.push_back(bgColor.g);
        pixels.push_back(bgColor.b);
        pixels.push_back(255);
    }
    screenTexture.update(pixels.data());
    screenSprite.setScale({static_cast<float>(SCALE_C), static_cast<float>(SCALE_C)});
}

void Emulator::drawScreen() const {
    /* *
     * Draws current screen to window.
     * */
    window->clear();
    window->draw(screenSprite);
    if (BORDER_C)
        window->draw(*borderSprite);
    window->display();

}
