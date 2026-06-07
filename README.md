# Tressa Millering's Chip8 Interpreter
***
## Overview
This is a command line interpreter for the Chip8, a programming language
developed for early video game development. It is written in C++, using 
SFML 3.0.2 for the screen and user interaction.
***

## Usage
To run the program, execute `chip [options] <path-to-rom>`. Executables for both 
Windows and Linux machines can be found in the Bin-Windows and Bin-Linux folders 
respectively. 

To build your own from the source code, follow these steps:

> **1.** Clone down the repository to your machine  
> **2.** Though not strictly necessary, it is recommended to create a 
>        separate directory called `build` in the same directory as the repository you cloned.   
> **3.** Run `cmake -S . -B ./build`, and then `cmake --build ./build`  
> **4.** Your executable will be in the `bin` directory inside `build`.  

This assumes that your CMake is configured to know where the compiler and make system is. 
***
## Command Line Options
`<>` denote required arguments, `[]` denote optional arguments. `()` are not necessary, only shown to group flags that have multiple usage options.   
Options and arguments are case insensitive. Order of options is not enforced.  
They are evaluated left-to-right, overwriting duplicates with the most recent.

Legal options are as follows:

| Option | Argument         | Description | Default |
|---|------------------|---|---|
| `-h`, `--help` | `[themes]`       | Outputs help message. Pass `themes` to list legal themes. | — |
| `-v`, `--version` |                  | Outputs version number. | — |
| `-b`, `--border` |                  | Draws a grid between each pixel on screen. | off |
| `-i`, `--step` |                  | Step mode — advance per opcode with Space. | off |
| `-c`, `--cpuhz` | `<unsigned_int>` | Set CPU Hz. | `700` |
| `-f`, `--beep` | `<unsigned_int>` | Set beep frequency (Hz). | `523` |
| `-t`, `--theme` | `<string>`       | Set color theme. | — |
| `-o`, `--oncolor` | `<#rrggbb>`      | Set pixel on-color. 6-digit hex preceded by `#`. | — |
| `-x`, `--offcolor` | `<#rrggbb>`      | Set pixel off-color. 6-digit hex preceded by `#`. | — |
| `--color-flip` |                  | Flip foreground and background colors of theme. | — |

***
## Theme List
Refer to the examples directory to view screenshots.  
The following themes can be used with the -t option.

| THEME NAME |  BACKGROUND  |  FOREGROUND  |
|:----------:|:------------:|:------------:|
|    Red     |    Black     |     Red      |
|    Blue    |    Black     |    Green     |
|   Green    |    Black     |     Blue     |
|    Pink    |    Black     |     Pink     |
|    Octo    | Dark Orange  | Light Orange |
|   Pastel   |   Lavender   |  Turquoise   |
|    Rose    |  Dark Pink   |  Light Pink  |
|   Ghost    |  Dark Gray   |  Light Gray  |
|   Blippi   | Light Orange |  Light Blue  |
|   Alien    |   Magenta    |     Lime     |
|   River    |     Aqua     |  Turquoise   |

***
## User Input
Input uses the following keymap:
        
        ---------    ---------    
        |1|2|3|C|    |1|2|3|4|
        ---------    ---------
        |4|5|6|D|    |Q|W|E|R|
        --------- => ---------
        |7|8|9|E|    |A|S|D|F|
        ---------    ---------
        |A|0|B|F|    |Z|X|C|V|
        ---------    ---------

***

## References
These sites were the primary references used for development.    
>[SFML Documentation](https://www.sfml-dev.org/documentation/3.0.2/index.html)    

>[Matthew Mikolay's Chip8 Wiki](https://github.com/mattmikolay/chip-8/wiki)  

>[These were used for testing](https://github.com/Timendus/chip8-test-suite?tab=readme-ov-file), alongside the games in the ROMs folder.     