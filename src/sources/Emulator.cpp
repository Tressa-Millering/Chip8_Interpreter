//
// Created by Tressa Millering on 3/31/2026.
//

#include "../headers/Emulator.h"
#include <iostream>

#include "../headers/Chip8.h"

constexpr unsigned int WIDTH_C = 64;
constexpr unsigned int HEIGHT_C = 32;
constexpr double TIMER_HZ_C = 60;
constexpr double CPU_HZ_C = 700;




Emulator::Emulator(const std::string& _romName,
                   const unsigned int _scale = 25,
                   const sf::Color _bgColor = sf::Color::Black,
                   const sf::Color _fgColor = sf::Color::White)

                  : SCALE_C(_scale),
                    bgColor(_bgColor),
                    fgColor(_fgColor),
                    screenTexture({WIDTH_C, HEIGHT_C}),
                    screenSprite(screenTexture),
                    romName(_romName){

    if (bgColor == fgColor){
        if (fgColor != sf::Color::Black) {
            bgColor = sf::Color::Black;
        } else {
            fgColor = sf::Color::White;
        }
    }
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
    const unsigned int i = (x + WIDTH_C * y);
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
            frameTimer -= FRAME_TIME;
        }

        if (chip8.GetScreenChange())
            updateScreen(chip8.GetScreen());

        window->clear();
        window->draw(screenSprite);
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




