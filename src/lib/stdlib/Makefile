ASM_SRC = $(wildcard ./src/*.asm)
C_SRC = $(wildcard ./src/*.c)
ASM_OBJ = $(ASM_SRC:./src/%.asm=./build/%.asm.o)
C_OBJ = $(C_SRC:./src/%.c=./build/%.o)
OBJS = $(ASM_OBJ) $(C_OBJ)

INCLUDES=-I./src
FLAGS= -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: stdlib.elf

stdlib.elf: $(OBJS)
	i686-elf-ld -m elf_i386 -relocatable $^ -o $@

./build/%.asm.o: ./src/%.asm
	nasm -f elf $< -o $@

./build/%.o: ./src/%.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c $< -o $@

clean:
	rm -rf $(OBJS)
	rm -f stdlib.elf
