C_SRC = $(wildcard ./src/*.c)
C_OBJ = $(C_SRC:./src/%.c=./build/%.o)

INCLUDES= -I../../lib/stdlib/src
FLAGS= -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: shell.elf

shell.elf: $(C_OBJ)
	i686-elf-gcc -g -T ./linker.ld -o $@ -ffreestanding -O0 -nostdlib -fpic -g $^ ../../lib/stdlib/stdlib.elf

./build/%.o: ./src/%.c
	i686-elf-gcc $(INCLUDES) -I./src $(FLAGS) -std=gnu99 -c $< -o $@

clean:
	rm -rf $(C_OBJ)
	rm -f shell.elf
