//
// Created by Tressa Millering on 3/31/2026.
//

#include "../headers/Emulator.h"
#include <iostream>

#include "../headers/Chip8.h"
#include <SFML/Audio.hpp>
#include <cmath>

constexpr unsigned int WIDTH_C = 64;
constexpr unsigned int HEIGHT_C = 32;
constexpr double TIMER_HZ_C = 60;


Emulator::Emulator(const std::string& _romName,
                   const unsigned int _scale,
                   const sf::Color _bgColor,
                   const sf::Color _fgColor,
                   const unsigned int _cpuhz,
                   const unsigned int _frequency,
                   const bool _border)

                  : SCALE_C(_scale),
                    bgColor(_bgColor),
                    fgColor(_fgColor),
                    screenTexture({WIDTH_C, HEIGHT_C}),
                    screenSprite(screenTexture),
                    romName(_romName),
                    CPU_HZ_C(_cpuhz),
                    BEEP_FREQ_C(_frequency),
                    border(_border){

    if (bgColor == fgColor){
        if (fgColor != sf::Color::Black) {
            bgColor = sf::Color::Black;
        } else {
            fgColor = sf::Color::White;
        }
    }

    constexpr double SAMPLE_RATE = 44100;
    std::vector<int16_t> samples(SAMPLE_RATE);
    for (int i = 0; i < SAMPLE_RATE; i++) {
        samples.at(i) = static_cast<int16_t>(32767*sin(2* M_PI * BEEP_FREQ_C * i/SAMPLE_RATE));
    }
    const std::vector<sf::SoundChannel> channelMap = { sf::SoundChannel::Mono };
    soundBuffer = new sf::SoundBuffer(samples.data(), samples.size(), channelMap.size(), SAMPLE_RATE, channelMap);
    beep = new sf::Sound(*soundBuffer);
    beep->setLooping(true);

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

Emulator::~Emulator() {
    delete window;
    delete beep;
    delete soundBuffer;
    delete borderTexture;
    delete borderSprite;
}

void Emulator::updateScreen(const std::vector<uint8_t>& buffer) {
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

void Emulator::flipAt(unsigned int i) {
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

void Emulator::mainLoop() {


    const sf::Time FRAME_TIME = sf::seconds(1.f / TIMER_HZ_C);
    const sf::Time CYCLE_TIME = sf::seconds(1.f / CPU_HZ_C);


    window = new sf::RenderWindow( sf::VideoMode( { WIDTH_C*SCALE_C, HEIGHT_C*SCALE_C} ), "Chip8" );
    window->setVerticalSyncEnabled(false);
    window->setFramerateLimit(0);

    if (border) {
        std::vector<uint8_t> borderData(4*HEIGHT_C*WIDTH_C*SCALE_C*SCALE_C,0);

        for (int i = 0; i < borderData.size()/4; i++) {

            if (i % SCALE_C == 0 || (((i / (SCALE_C*WIDTH_C)) % SCALE_C) == 0) ||
                i % SCALE_C == 1 || (((i / (SCALE_C*WIDTH_C)) % SCALE_C) == 1) ||
                i % SCALE_C == 2 || (((i / (SCALE_C*WIDTH_C)) % SCALE_C) == 2)) {
                borderData.at(4*i) = bgColor.r;
                borderData.at(4*i+1) = bgColor.g;
                borderData.at(4*i+2) = bgColor.b;
                borderData.at(4*i+3) = 255;
            }
        }

        borderTexture = new sf::Texture({WIDTH_C*SCALE_C, HEIGHT_C*SCALE_C});
        borderTexture->update(borderData.data());
        borderSprite = new sf::Sprite(*borderTexture);
        //borderSprite->setScale({static_cast<float>(SCALE_C), static_cast<float>(SCALE_C)});

    }

    Chip8 chip8(romName);

    sf::Clock clock;
    sf::Time frameTimer = sf::Time::Zero;
    sf::Time cycleTimer = sf::Time::Zero;

    while(window->isOpen()) {
        const sf::Time delta = clock.restart();
        frameTimer += delta;
        cycleTimer += delta;




        while (cycleTimer >= CYCLE_TIME) {
            chip8.UpdateKeystate();
            handleEvents();
            chip8.Cycle();
            cycleTimer -= CYCLE_TIME;
        }


        while (frameTimer >= FRAME_TIME) {
            chip8.TickTimers();
            if (chip8.GetSoundTimer() > 0 && beep->getStatus() != sf::Sound::Status::Playing) {
                beep->play();
            } else if (chip8.GetSoundTimer() <= 0){
                beep->pause();
            }
            frameTimer -= FRAME_TIME;

        }

        if (chip8.GetScreenChange()){
            updateScreen(chip8.GetScreen());
            chip8.SetScreenChange(false);
        }

            window->clear();
            window->draw(screenSprite);
            if (border)
                window->draw(*borderSprite);
            window->display();


    }
}

void Emulator::handleEvents() {
    while (const auto event = window->pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window->close();
        }

    }
}



