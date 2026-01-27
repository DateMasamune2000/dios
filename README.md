# DiOS - A Raspberry Pi operating system

*You expected an operating system but it was I, DIO!!!*

DiOS is an operating system for the raspberry Pi. It currently has a working command shell.

It currently works well inside QEMU, but has not yet been tested on real hardware.

## Functions

Currently, DiOS ships with a "virtual machine" for a basic Turning-complete
programming language. Operations can be performed by entering a command into
the prompt (over UART).

There are 256 bytes currently available to the machine to store data. This is
for JUST the data: the program (or "command" in DiOS terminology) is stored
separately.

State is preserved across successive command runs, so more complex computations
can be done in stages with multiple commands.

The following functions are currently supported:
- Manipulating the 256-byte memory array
- Taking text input from the user
- Printing text

## User guide

The command shell prompts the user to enter a command. A command can be 256
characters long, but changes to memory persist across commands.

The language is from the [P" family](`https://esolangs.org/wiki/P%E2%80%B2%E2%80%B2), which is a common family for many esolangs.

### The Tape

The Tape is a memory array. It is currently 256 "cells" long, each "cell" being a byte (though these limits may change soon).
Values on the Tape cannot be randomly addressed, and are modified via the Pointer.

### The Pointer

The Pointer points to a "cell" on the Tape. It starts at the first cell (number `0`), and can be advanced or sent back one cell at a time.
The only cell in memory that can be modified is the one pointed to by the Pointer.

### Commands

Commands are composed of one or more of the following characters, each of which performs a function

|character|function|
|-|-|
|`+`|Increment value in current cell|
|`-`|Decrement value in current cell|
|`>`|Advance pointer|
|`<`|Move pointer back|
|`[`|"anchor" point, used in combination with `]`|
|`]`|If current cell value is 0, move ahead to the next character. If not, jump back to the corresponding `[` to continue execution.|
|`.`|Send ASCII character corresponding to the current cell value over UART|
|`,`|Read a character over UART and store the corresponding ASCII character in the current cell|

## Build instructions

### Instructions

Running `make` builds and runs the OS in QEMU. UART data is sent and received on the terminal. Press Control-a and then C to open the QEMU console (from where it is possible to quit).

### Dependencies

- `arm-none-eabi-gcc` version 15
- `arm-none-eabi-binutils` version 2.45
- `qemu-system-arm`

### Additional files

The following Raspberry Pi files need to be sourced to be able to build the image:
- `bootcode.bin`
- `fixup.dat`

# Credits

Credits go to the [OSDev wiki](https://wiki.osdev.org/) for providing documentation and example code.
