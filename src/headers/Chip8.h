//
// Created by Tressa Millering on 3/31/2026.
//
#ifndef CHIP8_V3_CHIP8_H
#define CHIP8_V3_CHIP8_H

#include <vector>
#include <string>
#include <cstdint>
#include "SFML/Window/Keyboard.hpp"


class Chip8 {

public:
    Chip8(const std::string&);
    void Cycle();
    void TickTimers();
    void UpdateKeystate();
    bool GetScreenChange() const;
    void SetScreenChange(bool);
    const std::vector<uint8_t>& GetScreen() const;


private:
    static constexpr int MEM_START = 0x200;
    static constexpr int MEM_SIZE = 4096;
    static constexpr int FONTSET_START = 0x50;
    static constexpr int FONTSET_SIZE = 80;
    static constexpr uint8_t FONTSET[FONTSET_SIZE] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    static constexpr sf::Keyboard::Scan KEYMAP[16] = {

        sf::Keyboard::Scan::X,
        sf::Keyboard::Scan::Num1,
        sf::Keyboard::Scan::Num2,
        sf::Keyboard::Scan::Num3,
        sf::Keyboard::Scan::Q,
        sf::Keyboard::Scan::W,
        sf::Keyboard::Scan::E,
        sf::Keyboard::Scan::A,
        sf::Keyboard::Scan::S,
        sf::Keyboard::Scan::D,
        sf::Keyboard::Scan::Z,
        sf::Keyboard::Scan::C,
        sf::Keyboard::Scan::Num4,
        sf::Keyboard::Scan::R,
        sf::Keyboard::Scan::F,
        sf::Keyboard::Scan::V,

    /*  Each input is a hex value, mapped
        as follows.

        +-+-+-+-+    +-+-+-+-+
        |1|2|3|C|    |1|2|3|4|
        +-+-+-+-+    +-+-+-+-+
        |4|5|6|D|    |Q|W|E|R|
        +-+-+-+-+ => +-+-+-+-+
        |7|8|9|E|    |A|S|D|F|
        +-+-+-+-+    +-+-+-+-+
        |A|0|B|F|    |Z|X|C|V|
        +-+-+-+-+    +-+-+-+-+
    */

    };


    std::string romName;
    std::vector<uint8_t> memory = std::vector<uint8_t>(MEM_SIZE, 0);
    std::vector<uint8_t> registers = std::vector<uint8_t>(16, 0);
    std::vector<uint16_t> stack = std::vector<uint16_t>(16, 0);
    uint16_t index = 0;
    uint16_t progCounter = MEM_START;
    uint8_t stackPointer = 0;
    uint16_t opcode = 0;

    uint8_t soundTimer = 0;
    uint8_t delayTimer = 0;

    bool screenChange = false;
    std::vector<uint8_t> screen = std::vector<uint8_t>(64*32, 0);

    std::vector<bool> keys = std::vector<bool>(16, false);
    std::vector<bool> prevKeys = std::vector<bool>(16, false);;
    int waitingForKey = -1;


    //FUNCTIONS
    void (Chip8::*masterTable[0xF + 1])();
    void (Chip8::*table0[0xE + 1])();
    void (Chip8::*table8[0xE + 1])();
    void (Chip8::*tableE[0xE + 1])();
    void (Chip8::*tableF[0x65 + 1])();

    void loadRom();
    void loadFonts();
    void tableInit();
    void callOpcode();
    void access0();
    void access8();
    void accessE();
    void accessF();

    void OP_null();
    void OP_1nnn();
    void OP_2nnn();
    void OP_3xkk();
    void OP_4xkk();
    void OP_5xy0();
    void OP_6xkk();
    void OP_7xkk();
    void OP_9xy0();
    void OP_Annn();
    void OP_Bnnn();
    void OP_Cxkk();
    void OP_Dxyn();
    void OP_8xy0();
    void OP_8xy1();
    void OP_8xy2();
    void OP_8xy3();
    void OP_8xy4();
    void OP_8xy5();
    void OP_8xy6();
    void OP_8xy7();
    void OP_8xyE();
    void OP_00E0();
    void OP_00EE();
    void OP_ExA1();
    void OP_Ex9E();
    void OP_Fx07();
    void OP_Fx0A();
    void OP_Fx15();
    void OP_Fx18();
    void OP_Fx1E();
    void OP_Fx29();
    void OP_Fx33();
    void OP_Fx55();
    void OP_Fx65();

};


#endif //CMAKESFMLPROJECT_CHIP8_H