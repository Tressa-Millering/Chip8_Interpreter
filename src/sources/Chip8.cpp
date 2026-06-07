//
// Created by Tressa Millering on 3/31/2026.
//

#include "../headers/Chip8.h"
#include <fstream>

//debugging
#include <iostream>
#include <iomanip>
#include <bitset>

constexpr int MEM_START = 0x200;
constexpr int MEM_SIZE = 4096;
constexpr int FONTSET_START = 0x50;
constexpr int FONTSET_SIZE = 80;
constexpr uint8_t FONTSET[FONTSET_SIZE] =
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


Chip8::Chip8(const std::string& _romName) :
            romName(_romName),
            memory(MEM_SIZE, 0),
            registers(16, 0),
            stack(16, 0),
            index(0),
            progCounter(MEM_START),
            stackPointer(0),
            opcode(0),
            soundTimer(0),
            delayTimer(0),
            screenChange(false),
            screen(64*32, 0){

    loadRom();
    loadFonts();
    tableInit();



}

void Chip8::Cycle() {
    screenChange = false;
    opcode = (memory.at(progCounter) << 8) | memory.at(progCounter+1);
    progCounter += 2;
    callOpcode();

}

void Chip8::TickTimers() {
    if (soundTimer > 0)
        soundTimer--;

    if (delayTimer > 0)
        delayTimer--;
}

bool Chip8::GetScreenChange() const{
    return screenChange;
}

const std::vector<uint8_t>& Chip8::GetScreen() const{
    return screen;
}



void Chip8::loadRom() {
    try {
        std::ifstream romFile(romName, std::ios::ate | std::ios::binary);
        if(!romFile.is_open()) {
            throw std::runtime_error("Could not open file.");
        }

        const std::streamsize size = romFile.tellg();
        std::vector<char> buffer(size);
        romFile.seekg(0, std::ios::beg);
        romFile.read(buffer.data(), size);
        romFile.close();

        for (int i = 0; i < size; i++) {
            memory.at(MEM_START + i) = buffer[i];
        }


    }
    catch (std::exception& e) {
        std::cerr << "Error: " << e.what();
    }

}

void Chip8::loadFonts() {
    for (int i = 0; i < FONTSET_SIZE; i++) {
        memory.at(i + FONTSET_START) = FONTSET[i];
    }
}


void Chip8::tableInit() {
    masterTable[0x0] = &Chip8::access0;
    masterTable[0x1] = &Chip8::OP_1nnn;
    masterTable[0x2] = &Chip8::OP_2nnn;
    masterTable[0x3] = &Chip8::OP_3xkk;
    masterTable[0x4] = &Chip8::OP_4xkk;
    masterTable[0x5] = &Chip8::OP_5xy0;
    masterTable[0x6] = &Chip8::OP_6xkk;
    masterTable[0x7] = &Chip8::OP_7xkk;
    masterTable[0x8] = &Chip8::access8;
    masterTable[0x9] = &Chip8::OP_9xy0;
    masterTable[0xA] = &Chip8::OP_Annn;
    masterTable[0xB] = &Chip8::OP_Bnnn;
    masterTable[0xC] = &Chip8::OP_Cxkk;
    masterTable[0xD] = &Chip8::OP_Dxyn;
    masterTable[0xE] = &Chip8::accessE;
    masterTable[0xF] = &Chip8::accessF;

    for (int i = 0; i <= 0xE; i++) {
        table0[i] = &Chip8::OP_null;
        table8[i] = &Chip8::OP_null;
        tableE[i] = &Chip8::OP_null;
    }
    table0[0x0] = &Chip8::OP_00E0;
    table0[0xE] = &Chip8::OP_00EE;

    tableE[0x1] = &Chip8::OP_ExA1;
    tableE[0xE] = &Chip8::OP_Ex9E;

    table8[0x0] = &Chip8::OP_8xy0;
    table8[0x1] = &Chip8::OP_8xy1;
    table8[0x2] = &Chip8::OP_8xy2;
    table8[0x3] = &Chip8::OP_8xy3;
    table8[0x4] = &Chip8::OP_8xy4;
    table8[0x5] = &Chip8::OP_8xy5;
    table8[0x6] = &Chip8::OP_8xy6;
    table8[0x7] = &Chip8::OP_8xy7;
    table8[0xE] = &Chip8::OP_8xyE;

    for (int i = 0; i <= 0x65; i++) {
        tableF[i] = &Chip8::OP_null;
    }

    tableF[0x07] = &Chip8::OP_Fx07;
    tableF[0x0A] = &Chip8::OP_Fx0A;
    tableF[0x15] = &Chip8::OP_Fx15;
    tableF[0x18] = &Chip8::OP_Fx18;
    tableF[0x1E] = &Chip8::OP_Fx1E;
    tableF[0x29] = &Chip8::OP_Fx29;
    tableF[0x33] = &Chip8::OP_Fx33;
    tableF[0x55] = &Chip8::OP_Fx55;
    tableF[0x65] = &Chip8::OP_Fx65;
}

void Chip8::callOpcode() {
    //std::cout << "CALLING: " << std::hex << std::setfill('0') << std::setw(4) << opcode << std::endl;
    (this->*masterTable[(opcode & 0xF000) >> 12])();
}

void Chip8::access0() {
    (this->*table0[opcode & 0x000F])();
}

void Chip8::access8() {
    (this->*table8[opcode & 0x000F])();
}

void Chip8::accessE() {
    (this->*tableE[(opcode & 0x000F)])();
}

void Chip8::accessF() {
    (this->*tableF[opcode & 0x00FF])();
}



//Opcode NULL
void Chip8::OP_null() {
    //std::cout << "OP_NULL\n";
}

//Opcode 1nnn
//    Jump to mem location nnn
void Chip8::OP_1nnn() {
    //std::cout << "OP_1nnn\n";
    const int nnn = opcode & 0x0FFF;
    progCounter = nnn;
}

//Opcode 2nnn
//    Call subroutine at nnn
void Chip8::OP_2nnn() {
   //std::cout << "OP_2nnn\n";
    const int nnn = opcode & 0x0FFF;
    stackPointer++;
    stack.at(stackPointer) = progCounter;
    progCounter = nnn;
}

//Opcode 3xkk
//    Skip next instruction if x = kk
void Chip8::OP_3xkk() {
    //std::cout << "OP_3xkk\n";
    const int x = (opcode & 0x0F00) >> 8;
    const int kk = (opcode & 0x00FF);
    if (registers[x] == kk) {
        progCounter+=2;
    }
}

//Opcode 4xkk
//    skip next instruction if x != kk
void Chip8::OP_4xkk() {
    //std::cout << "OP_4xkk\n";
    const int x = (opcode & 0x0F00) >> 8;
    const int kk = (opcode & 0x00FF);
    if (registers[x] != kk) {
        progCounter+=2;
    }
}

//Opcode 5xy0
//    skip next if reg[x] == reg[y]
void Chip8::OP_5xy0() {
    //std::cout << "OP_5xy0\n";
    const int x = (opcode & 0x0F00) >> 8;
    const int y = (opcode & 0x00F) >> 4;
    if (registers[x] == registers[y]) {
        progCounter+=2;
    }
}

//Opcode 6xkk
//    Put kk in reg[x]
void Chip8::OP_6xkk() {
    //std::cout << "OP_6xkk\n";
    const int x = (opcode & 0x0F00) >> 8;
    const int kk = (opcode & 0x00FF);
    registers[x] = kk;
}

//Opcode 7xkk
//    Vx += kk
void Chip8::OP_7xkk() {
    //std::cout << "OP_7xkk\n";
    const int x = (opcode & 0x0F00) >> 8;
    const int kk = (opcode & 0x00FF);
    registers[x] += kk;
}

//Opcode 9xy0
//    skip next if vx != vy
void Chip8::OP_9xy0() {
    //std::cout << "OP_9xy0\n";
    const int x = (opcode & 0x0F00) >> 8;
    const int y = (opcode & 0x00F0) >> 4;
    if (registers[x] != registers[y]) {
        progCounter+=2;
    }
}

//Opcode Annn
//    Set index to nnn
void Chip8::OP_Annn() {
    //std::cout << "OP_Annn\n";
    const int nnn = opcode & 0x0FFF;
    index = nnn;
}

//Opcode Bnnn
//    jump to nnn + v0
void Chip8::OP_Bnnn() {
    //std::cout << "OP_Bnnn\n";
    const int nnn = opcode & 0x0FFF;
    opcode = registers[0] + nnn;
}

//Opcode Cxkk
//    set vx = random byte & kk
void Chip8::OP_Cxkk() {
    //std::cout << "OP_Cxkk\n";
    const int randNum = rand() % 256;
    const int x = (opcode & 0x0F00) >> 8;
    const int kk = (opcode & 0x00FF);

    registers[x] = randNum & kk;
}

//Opcode Dxyn
//    Display n-byte sprite starting at memory
//    location I at (Vx, Vy), set VF = collision.
void Chip8::OP_Dxyn() {
    const int x = (opcode & 0x0F00) >> 8;
    const int y = (opcode & 0x00F0) >> 4;
    const int n = opcode & 0x000F;
    //std::cout << "OP_Dxyn    " << std::hex << x <<" " << y << " " << n << " \n";

    registers[0xF] = 0;

    for (int row = 0; row < n; row++) {
        for (int col = 0; col < 8; col++) {
            const int coord = ((registers[x]+col)%64) + 64 * ((registers[y]+row)%32);
            if (memory[index + row] & (0x80 >> col)) {
                if (screen.at(coord))
                    registers[0xF] = 1;

                screen.at(coord) = !screen.at(coord);
            }
        }
    }

    screenChange = true;
}

//Opcode 8xy0
//    set vx = vy
void Chip8::OP_8xy0() {
    //std::cout << "OP_8xy0\n";
    const int x = (opcode & 0x0F00) >> 8;
    const int y = (opcode & 0x00F0) >> 4;
    registers[x] = registers[y];
}

//Opcode 8xy1
//    set vx |= vy
void Chip8::OP_8xy1() {
    //std::cout << "OP_8xy1\n";
    const int x = (opcode & 0x0F00) >> 8;
    const int y = (opcode & 0x00F0) >> 4;
    registers[x] |= registers[y];
}

//Opcode 8xy2
//    vx &= vy
void Chip8::OP_8xy2() {
    //std::cout << "OP_8xy2\n";
    const int x = (opcode & 0x0F00) >> 8;
    const int y = (opcode & 0x00F0) >> 4;
    registers[x] &= registers[y];
}

//Opcode 8xy3
//    vx nor= vy
void Chip8::OP_8xy3() {
    //std::cout << "OP_8xy3\n";
    const int x = (opcode & 0x0F00) >> 8;
    const int y = (opcode & 0x00F0) >> 4;
    registers[x] ^= registers[y];
}

//Opcode 8xy4
//    vx += vy, vf = carry
void Chip8::OP_8xy4() {
    //std::cout << "OP_8xy4\n";
    const int x = (opcode & 0x0F00) >> 8;
    const int y = (opcode & 0x00F0) >> 4;
    registers[0] = (registers[x] + registers[y] > 8) ? 1 : 0;
    registers[x] += registers[y];

}

//Opcode 8xy5
//    vx -= vy, vf = not borrow
void Chip8::OP_8xy5() {
    //std::cout << "OP_8xy5\n";
    const int x = (opcode & 0x0F00) >> 8;
    const int y = (opcode & 0x00F0) >> 4;
    registers[0] = (registers[x] > registers[y]) ? 1 : 0;
    registers[x] -= registers[y];
}

//Opcode 8xy6
//    Set Vx = Vx SHR 1.
//    If the least-significant bit of Vx
//    is 1, then VF is set to 1, otherwise 0.
//    Then Vx is divided by 2.
void Chip8::OP_8xy6() {
    //std::cout << "OP_8xy6\n";
    const int x = (opcode & 0x0F00) >> 8;

    if (registers[x] & 0x1)
        registers[0xF] = 1;
    else
        registers[0xF] = 0;

    registers[x] >>= 1;


}

//Opcode 8xy7
//    Set Vx = Vy - Vx, set VF = NOT borrow
void Chip8::OP_8xy7() {
    //std::cout << "OP_8xy7\n";
    const int x = (opcode & 0x0F00) >> 8;
    const int y = (opcode & 0x00F0) >> 4;
    if (registers[y] > registers[x])
        registers[0xF] = 1;
    registers[x] = registers[y] - registers[x];

}

//Opcode 8xyE
//    Set Vx = Vx SHL 1.
//    If the most-significant bit of
//    Vx is 1, then VF is set to 1,
//    otherwise to 0. Then Vx is
//    multiplied by 2.
void Chip8::OP_8xyE() {
    //std::cout << "OP_8xyE\n";
    const int x = (opcode & 0x0F00) >> 8;
    if (registers[x] & 0x1000)
        registers[0xF] = 1;
    else
        registers[0xF] = 0;

    registers[x] <<= 1;

}

//Opcode 00E0
//     Clear Screen
void Chip8::OP_00E0() {
    //std::cout << "OP_00E0\n";
    for (auto& i : screen) {
        i = 0;
    }
    screenChange = true;

}

//Opcode 00EE
//    Return from subroutine
void Chip8::OP_00EE() {
    //std::cout << "OP_00EE\n";
    progCounter = stack[stackPointer--];


}

//Opcode ExA1
void Chip8::OP_ExA1() {
    std::cout << "OP_ExA1\n";


}

//Opcode Ex9E
void Chip8::OP_Ex9E() {
    std::cout << "OP_Ex9E\n";

}

//Opcode Fx07
//    Set Vx = delay timer value.
void Chip8::OP_Fx07() {
    //std::cout << "OP_Fx07\n";
    const int x = (opcode & 0x0F00) >> 8;
    registers[x] = delayTimer;

}

//Opcode Fx0A
void Chip8::OP_Fx0A() {
    std::cout << "OP_Fx0A\n";

}

//Opcode Fx15
//    set delaytimer = vx
void Chip8::OP_Fx15() {
    std::cout << "OP_Fx15\n";
    const int x = (opcode & 0x0F00) >> 8;
    delayTimer = registers[x];


}

//Opcode Fx18
//    Set sound timer = Vx.
void Chip8::OP_Fx18() {
    //std::cout << "OP_Fx18\n";
    const int x = (opcode & 0x0F00) >> 8;
    soundTimer = registers[x];
}

//Opcode Fx1E
//      Set I = I + Vx.
void Chip8::OP_Fx1E() {
    //std::cout << "OP_Fx1E\n";
    const int x = (opcode & 0x0F00) >> 8;
    index += registers[x];
}

//Opcode Fx29
//    Set I = location of sprite for digit Vx.
void Chip8::OP_Fx29() {
    //std::cout << "OP_Fx29\n";
    const int x = (opcode & 0x0F00) >> 8;
    index = 5 * x + FONTSET_START;
}

//Opcode Fx33
//    Store BCD representation of
//    Vx in memory locations I, I+1, and I+2.

void Chip8::OP_Fx33() {
    //std::cout << "OP_Fx33\n";
    int vx = registers[(opcode & 0x0F00) >> 8];

    memory[index+2] = vx % 10;
    vx /= 10;
    memory[index+1] = vx % 10;
    vx /= 10;
    memory[index] = vx % 10;
}

//Opcode Fx55
//    Store registers V0 through Vx in memory
//    starting at location I.
void Chip8::OP_Fx55() {
    //std::cout << "OP_Fx55\n";
    const int x = (opcode & 0x0F00) >> 8;
    for (int i = 0; i <= x; i++) {
        memory[index + i] = registers[i];
    }

}

//Opcode Fx65
void Chip8::OP_Fx65() {
   //std::cout << "OP_Fx65\n";
    const int x = (opcode & 0x0F00) >> 8;
    for (int i = 0; i <= x; i++) {
        registers[i] = memory[index + i];
    }
}

