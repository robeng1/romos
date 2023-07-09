
# RomOS 

This repository contains a simple OS code.

## Prerequisites

To assemble and run the OS, you'll need the following:

- NASM (Netwide Assembler): An assembler for x86 assembly language.
- QEMU (Quick EMUlator): A virtual machine emulator.
- Cross Compile GCC. See [GCC Cross-Compiler](https://wiki.osdev.org/GCC_Cross-Compiler) for more information.

## Usage

1. Clone the repository and navigate to the project directory.

2. Build the OS code:

   ```bash
   sudo chmod +x ./build.sh  # Grant exec permission
   ./build.sh
   ```

3. Run the OS using QEMU:

   ```bash
   qemu-system-x86_64 -hda ./bin/os.bin
   ```

   This command runs QEMU and boots the virtual machine from the `os.bin` file.


## Customization

- Feel free to explore and modify the code to add additional functionality or experiment with different features.

## Notes

- Make sure to use appropriate versions of NASM and QEMU that are compatible with your system.

- The code is intended to be run as a boot sector and assumes that it will be loaded into memory address 0x7C00. Running it directly on a host operating system may have unintended effects.

- This code is provided for educational purposes and may not be suitable for production use.

```