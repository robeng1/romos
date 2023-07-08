# Variable Definitions

# FILES is a list of object files to be linked to create the final binary
FILES = ./build/kernel/kernel.asm.o \
./build/kernel/kernel.o \
./build/loaders/elf/elf.o \
./build/loaders/elf/loader.o \
./build/isr80h/isr80h.o \
./build/isr80h/process.o \
./build/isr80h/memory.o \
./build/drivers/keyboard/keyboard.o \
./build/drivers/keyboard/classic.o \
./build/isr80h/io.o \
./build/disk/disk.o \
./build/disk/stream.o \
./build/task/process.o \
./build/task/task.o \
./build/task/task.asm.o \
./build/task/tss.asm.o \
./build/fs/parser.o \
./build/fs/file.o \
./build/fs/fat/fat16.o \
./build/string/string.o \
./build/idt/idt.asm.o \
./build/idt/idt.o \
./build/mm/memory.o \
./build/io/io.asm.o \
./build/gdt/gdt.o \
./build/gdt/gdt.asm.o \
./build/mm/heap/heap.o \
./build/mm/heap/kernel_heap.o \
./build/mm/paging/paging.o \
./build/mm/paging/paging.asm.o \
./build/mm/blkm/blkm.o

# INCLUDES is a list of directories where the compiler can find header files
INCLUDES = -I./src

# FLAGS defines the compiler options
FLAGS = -g \
-ffreestanding \
-falign-jumps \
-falign-functions \
-falign-labels \
-falign-loops \
-fstrength-reduce \
-fomit-frame-pointer \
-finline-functions \
-Wno-unused-function \
-fno-builtin \
-Werror \
-Wno-unused-label \
-Wno-cpp \
-Wno-unused-parameter \
-nostdlib \
-nostartfiles \
-nodefaultlibs \
-Wall \
-O0 \
-Iinc

# CC, LD, and AS define the C compiler, linker, and assembler to be used
CC = i686-elf-gcc
LD = i686-elf-ld
AS = nasm

# CFLAGS, ASMFLAGS, and LDFLAGS are the flags to be used with the C compiler, assembler, and linker
CFLAGS = $(INCLUDES) $(FLAGS) -std=gnu99 -c
ASMFLAGS = -f elf -g
LDFLAGS = -g -relocatable

# BUILD_DIR and BIN_DIR define the directories for build files and binaries
BUILD_DIR = ./build
BIN_DIR = ./bin

# Pattern rules for building object files
# These rules tell make how to create .o and .asm.o files from .c and .asm files

$(BUILD_DIR)/%.o: ./src/%.c
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.asm.o: ./src/%.asm
	$(AS) $(ASMFLAGS) $< -o $@

# Build and clean rules

# .PHONY declares 'all' and 'clean' as phony targets. Phony targets are always out-of-date and always execute
.PHONY: all clean

# The 'all' target builds os.bin
all: $(BIN_DIR)/os.bin

# This rule describes how to build os.bin from boot.bin and kernel.bin
$(BIN_DIR)/os.bin: $(BIN_DIR)/boot.bin $(BIN_DIR)/kernel.bin
	rm -rf $(BIN_DIR)/os.bin
	dd if=$(BIN_DIR)/boot.bin >> $(BIN_DIR)/os.bin
	dd if=$(BIN_DIR)/kernel.bin >> $(BIN_DIR)/os.bin
	dd if=/dev/zero bs=1048576 count=16 >> $(BIN_DIR)/os.bin
	sudo mount -t vfat ./bin/os.bin /mnt/d

# This rule describes how to build kernel.bin from the object files listed in FILES
$(BIN_DIR)/kernel.bin: $(FILES)
	$(LD) $(LDFLAGS) $(FILES) -o $(BUILD_DIR)/romos.o
	$(CC) -T ./src/linker.ld -o $(BIN_DIR)/kernel.bin -ffreestanding -O0 -nostdlib $(BUILD_DIR)/romos.o

# This rule describes how to build boot.bin from boot.asm
$(BIN_DIR)/boot.bin: ./src/boot/boot.asm
	$(AS) -f bin ./src/boot/boot.asm -o $(BIN_DIR)/boot.bin

# The 'clean' target removes all the generated files
clean:
	rm -rf $(BIN_DIR)/*.bin
	rm -rf $(FILES)
	rm -rf $(BUILD_DIR)/*.o
