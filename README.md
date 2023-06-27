
This repository contains a simple x86 assembly bootloader code that displays the message "Hello World from RomOS!" on the screen. The code is designed to run as a boot sector, typically at memory address 0x7C00.

## Prerequisites

To assemble and run the bootloader, you'll need the following:

- NASM (Netwide Assembler): An assembler for x86 assembly language.
- QEMU (Quick EMUlator): A virtual machine emulator.

## Usage

1. Clone the repository and navigate to the project directory.

2. Assemble the bootloader code using NASM:

   ```bash
   nasm -f bin boot.asm -o boot.bin
   ```

   This command assembles the assembly code in `boot.asm` and outputs the raw binary file `boot.bin`.

3. Run the bootloader using QEMU:

   ```bash
   qemu-system-x86_64 -hda bootloader.bin
   ```

   This command runs QEMU and boots the virtual machine from the `boot.bin` file.

4. You should see the message "Hello World from RomOS!" displayed on the screen.

## Customization

- If you want to modify the message displayed by the bootloader, open the `boot.asm` file and change the contents of the `message` variable.

- Feel free to explore and modify the code to add additional functionality or experiment with different features.

## Notes

- Make sure to use appropriate versions of NASM and QEMU that are compatible with your system.

- The code is intended to be run as a boot sector and assumes that it will be loaded into memory address 0x7C00. Running it directly on a host operating system may have unintended effects.

- This code is provided for educational purposes and may not be suitable for production use.

```