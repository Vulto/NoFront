# No Front 

No Front Menu is a simple program written in C that allows you to browse and launch ROMs for different gaming consoles using a terminal-based menu interface.

## Features

- Supports multiple gaming consoles: Nintendo, Super Nintendo, and Nintendo 64.
- Displays a menu of available ROMs for the selected console.
- Allows navigation through the ROM list using arrow keys or a gamepad (D-pad).
- Launches selected ROMs using the Mednafen emulator.

## Requirements

To compile and run the ROM Menu program, you need to have the following dependencies installed:

- C Compiler (e.g., GCC)
- Ncurses library
- Mednafen emulator

## Compilation

1. Clone the repository:
$ git clone https://github.com/your-username/rom-menu.git
$ cd rom-menu


2. Compile the source code:
$ ./c

3. Ensure that the Mednafen emulator is installed and accessible in your system's PATH.

## Usage

1. Run the compiled executable:
$ ./nf

2. The ROM Menu will be displayed, showing the available ROMs for the selected console.

3. Use the arrow keys or D-pad to navigate through the ROM list.

4. Press Enter or the gamepad button to launch the selected ROM using the Mednafen emulator.

5. Use the left and right arrow keys or gamepad buttons to switch between different gaming consoles.

6. Press 'q' to exit the ROM Menu.

## Customization

- To add or modify ROMs for each gaming console, update the ROM directory paths in the `main.h` file (`RomsDir` array).
- You can customize the supported gaming consoles and their names by modifying the `Console` array in the `main.h` file.

## License

This project is licensed under the MIT License.