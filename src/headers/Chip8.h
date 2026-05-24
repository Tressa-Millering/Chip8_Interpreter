//
// Created by Tressa Millering on 3/31/2026.
//
#ifndef CHIP8_V3_CHIP8_H
#define CHIP8_V3_CHIP8_H

#include <vector>
#include <string>
#include <cstdint>

class Chip8 {

public:
    Chip8(const std::string&);
    void Cycle();
    void TickTimers();
    void UpdateKeystate();
    bool GetScreenChange() const;
    const std::vector<uint8_t>& GetScreen() const;


private:
    std::string romName;
    std::vector<uint8_t> memory, registers;
    std::vector<uint16_t> stack;
    uint16_t index;
    uint16_t progCounter;
    uint8_t stackPointer;
    uint16_t opcode;

    uint8_t soundTimer;
    uint8_t delayTimer;

    bool screenChange;
    std::vector<uint8_t> screen;

    std::vector<bool> keys;
    int waitingForKey;

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